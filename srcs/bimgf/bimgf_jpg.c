#include "bimgf.h"

/*
 * TODO: Improvement:
 * 	Read length first, then read length amount of bytes;
 *		Then assign the variables accordingly;
 */

#define BIMGF_JPG_SOI	0xD8	// Start Of Image
#define BIMGF_JPG_SOF0	0xC0	// Start Of Frame // Baseline DCT
#define BIMGF_JPG_SOF2	0xC2	// Start Of Frame // Progressive DCT
#define BIMGF_JPG_DHT	0xC4	// Define Huffman Table
#define BIMGF_JPG_DQT	0xDB	// Define Quantization Table
#define BIMGF_JPG_DRI	0xDD	// Define Restart Interval
#define BIMGF_JPG_SOS	0xDA	// Start Of Scan
#define BIMGF_JPG_RST	0xD0	// D0..D7 Restart
#define BIMGF_JPG_JFIF	0xE0	// Application Specific
#define BIMGF_JPG_COM	0xFE	// Comment
#define BIMGF_JPG_EOI	0xD9	// End Of Image

// Huffman table
#define BIMGF_HM_DC		0
#define BIMGF_HM_AC		1

// JPEG/JFIF Image Segment
typedef struct s_jfif
{
	uint16_t	length; // length of this segment excluding marker 2 bytes;
	uint8_t		identifier[5]; // 4A 46 49 46 00 , aka "JFIF\0";
	uint16_t	version; // 1st byte major version, 2nd byte minor version;
	uint8_t		density; // 00 : widht / height pixel aspect ratio; 01 : pixels per inch??; 02 : pixels per centiemer??!?!?
	uint16_t	x_density; // horizontal pixel density;
	uint16_t	y_density; // vertical pixel density;
	uint8_t		x_thumbnail; // horizontal pixel density for thumbnail;
	uint8_t		y_thumbnail; // vertical pixel density for thumbnail;
	uint8_t		thumbnail_size; // 3 * x_thumbnail * y_thumbnail;
	uint8_t		*thumbnail; // 3 * n; r,g,b; n = x_thumbnail * y_thumbnail;

	// not in file;
	uint8_t		got; // if we have already got jfif, so we know if there is one more of the same marker, that its the jfxx;
	uint16_t	bytes_read;
}	t_jfif;

// Comment
typedef struct s_com
{
	uint16_t	length; // length in bytes of segment;
	uint8_t		*data; // comment data; size == length
}	t_com;

// Define Quantization Table
typedef struct s_dqt
{
	uint16_t	length; // lenght of segment in bytes;
	uint8_t		info; // Pq, 1st 4 bits represent precision (0 = 8-bit, 1 = 16-bit), Tq, last 4 bits represent quantiziation table num;
	uint16_t	data[64]; // quantization data, vector of 64 elements in zig-zag order; size == count of unique Tq's given in info;

	// not in file;
	// Saving the amount of bytes read in this segment, should match up to length;
	uint16_t	bytes_read;
}	t_dqt;

// Start of Frame-0
typedef struct s_sof
{
	uint16_t	length; // length in bytes of segment;
	uint8_t		precision; // of frame data;
	uint16_t	height; // image height in pixels;
	uint16_t	width; // image width in pixels;
	uint8_t		comp_amount; // component amount in frame, 3 for RGB images;

	// ID 8 / Horizontal sampling 4 / Vertical- 4 / qtable 8
	uint8_t		*components; // size = ( 3 x comp_amount ) ; 1st byte id of component, 2nd byte sampling factpr (1st 4 bits represent horizontal, last 4 represent vertical), 3rd byte represent which quantization table to use;

	// Saving the amount of bytes read in this segment, should match up to length;
	uint16_t	bytes_read;
}	t_sof;

// Define Huffman Table
typedef struct s_dht
{
	uint16_t	length; // length in bytes of segment;

	uint8_t		info; // 1st 4 bits represent table type (0 : DC, 1 : AC), last four represent table num;
	uint8_t		symbol_count[16]; // 
	uint8_t		*symbols; // (size <= 256)??? source : http://mykb.cipindanci.com/archive/SuperKB/1294/JPEG%20File%20Layout%20and%20Format.htm

	// not in the file;
	uint16_t	total_symbol_count; // total_symbol_count += symbol_count[i];
	t_hm_node	*tree;

	// not in file
	uint16_t	bytes_read;
}	t_dht;

// Start of Scan
typedef struct s_sos
{
	uint16_t	length; // length in bytes of segment;
	uint8_t		count; // component count, pretty self explanatory; (count > 1 && count < 4)??? source : https://koushtav.me/jpeg/tutorial/c++/decoder/2019/03/02/lets-write-a-simple-jpeg-library-part-2/#parsesossegment
	uint8_t		*data; // component data; (size = 2 * count);... 
	// ... scan component selector, 8 bits
	// ... DC entropy coding table selector, 4 bits
	// ... AC entropy coding table selector, 4 bits
	uint8_t		skip_bytes[3]; // mandatory bytes to skip; ...
	// ... start of spectral selection, 8bits
	// ... end of spectral selection,	8bits
	// ... successive approximation bit high, 4bits
	// ... successive approximation bit low, 4bits

	// not in file;
	uint16_t	bytes_read;
}	t_sos;

// Image data
// This site is godlike : https://www.impulseadventure.com/photo/jpeg-huffman-coding.html
typedef struct s_imgdata
{
	uint8_t		*data; // bytified bits...; size == 'amount' * 8;
	uint32_t	amount; // amount of 'bits';

	// not in file;
	uint32_t	bytes_read;
	// from file
	uint8_t		*unprocessed_data; // used for debugging;
}	t_imgdata;

typedef struct s_mcu
{
	int		rgb[3][256];
	float	data[3][256]; // end result of this are pixels;
	int		zeros[3][256];
	float	coeffs[3][256]; // Y : 0, Cb : 1, Cr : 2;
	int		amount[3]; // amount of data in variable, should be the same in all;
}	t_mcu;

const int	zigzag[] = {
		0, 1, 5, 6, 14, 15, 27, 28,
		2, 4, 7, 13, 16, 26, 29, 42,
		3, 8, 12, 17, 25, 30, 41, 43,
		9, 11, 18, 24, 31, 40, 44, 53,
		10, 19, 23, 32, 39, 45, 52, 54,
		20, 22, 33, 38, 46, 51, 55, 60,
		21, 34, 37, 47, 50, 56, 59, 61,
		35, 36, 48, 49, 57, 58, 62, 63
	};


const float	cos_thing[] = { // cos();
	1.000000, 0.980785, 0.923880, 0.831470, 0.707107, 0.555570, 0.382683, 0.195090, 1.000000, 0.831470, 0.382683, -0.195090, -0.707107, -0.980785, -0.923880, -0.555570, 1.000000, 0.555570, -0.382683, -0.980785, -0.707107, 0.195090, 0.923880, 0.831470, 1.000000, 0.195090, -0.923880, -0.555570, 0.707107, 0.831470, -0.382684, -0.980785, 1.000000, -0.195090, -0.923880, 0.555570, 0.707107, -0.831470, -0.382683, 0.980785, 1.000000, -0.555570, -0.382683, 0.980785, -0.707107, -0.195090, 0.923879, -0.831470, 1.000000, -0.831470, 0.382684, 0.195090, -0.707107, 0.980785, -0.923880, 0.555571, 1.000000, -0.980785, 0.923880, -0.831470, 0.707107, -0.555571, 0.382684, -0.195091};
const float cos_thing_1[] = { // cos(); * 0.707106f;
	0.707106, 0.693519, 0.653281, 0.587937, 0.500000, 0.392847, 0.270597, 0.137949, 0.707106, 0.587937, 0.270597, -0.137949, -0.500000, -0.693519, -0.653281, -0.392847, 0.707106, 0.392847, -0.270597, -0.693519, -0.500000, 0.137949, 0.653281, 0.587937, 0.707106, 0.137949, -0.653281, -0.392847, 0.500000, 0.587937, -0.270598, -0.693519, 0.707106, -0.137949, -0.653281, 0.392847, 0.500000, -0.587937, -0.270597, 0.693519, 0.707106, -0.392847, -0.270597, 0.693519, -0.500000, -0.137949, 0.653280, -0.587937, 0.707106, -0.587937, 0.270598, 0.137949, -0.500000, 0.693519, -0.653281, 0.392848, 0.707106, -0.693519, 0.653281, -0.587937, 0.500000, -0.392848, 0.270598, -0.137950
};

void	idct_vol0(float *coeffs, float *data)
{
	for (int i = 0; i < 64; i++)
	{
		float	sum = 0.0f;
		int		y = i / 8;
		int		x = i % 8;
		for (int j = 0; j < 64; j++)
		{
			int		u = j / 8;
			int		v = j % 8;
			float	Cu = u == 0 ? 0.707106 : 1.0f;	
			float	Cv = v == 0 ? 0.707106 : 1.0f;	

			sum += Cu * Cv * data[j]
				* cos((2 * x + 1) * u * M_PI / 16.0f)
				* cos((2 * y + 1) * v * M_PI / 16.0f);
		}
		coeffs[i] = 0.25f * sum;
	}
}

void	idct_vol1(float *coeffs, float *data, float *zeros)
{
	for (int y = 0; y < 8; y++)
	{
		for (int x = 0; x < 8; x++)
		{
			float	sum = 0.0f;
			int		uv_zero_total = 0;
			int	u = 0;
			int v = 0;
			int uv = 0;
			float f = cos_thing_1[x * 8 + u];
			for (; u < 8; u++)
			{
				float e = cos_thing_1[y * 8 + v];
				for (; v < 8; v++)
				{
					uv = u * 8 + v;
					if (uv + uv_zero_total >= 64)
						break ;
					if (zeros[uv] != 0)
					{
						uv_zero_total += zeros[uv];
						continue ;
					}
					sum += data[uv + uv_zero_total] * f * e;
					e = cos_thing[y * 8 + v];
				}
				f = cos_thing[x * 8 + u];
			}
			coeffs[y * 8 + x] = sum * 0.25f;
		}
	}
}

const float zero_thing[] = {
		1.000000, 0.980785, 0.923880, 0.831470, 0.707107, 0.555570, 0.382683, 0.195090, 1.000000, 0.831470, 0.382683, -0.195090, -0.707107, -0.980785, -0.923880, -0.555570, 1.000000, 0.555570, -0.382684, -0.980785, -0.707107, 0.195091, 0.923880, 0.831469, 1.000000, 0.195090, -0.923880, -0.555570, 0.707107, 0.831469, -0.382684, -0.980785, 1.000000, -0.195090, -0.923880, 0.555570, 0.707107, -0.831470, -0.382683, 0.980785, 1.000000, -0.555570, -0.382683, 0.980785, -0.707107, -0.195090, 0.923879, -0.831470, 1.000000, -0.831470, 0.382684, 0.195090, -0.707107, 0.980785, -0.923880, 0.555571, 1.000000, -0.980785, 0.923880, -0.831470, 0.707107, -0.555570, 0.382683, -0.195090
	};
/* This generated the values above;
for (int k = 0; k < 8; k++)
		for (int i = 0; i < 8; i++)
			printf("%f, ", cos(eight_pi * ((float)k + 0.5f) * i));
*/
const float row_zero_thing[] = {
		128.000000, 125.540516, 118.256580, 106.428110, 90.509668, 71.112990, 48.983479, 24.971561, 128.000000, 106.428110, 48.983479, -24.971561, -90.509668, -125.540516, -118.256580, -71.112990, 128.000000, 71.112990, -48.983479, -125.540516, -90.509668, 24.971561, 118.256580, 106.428110, 128.000000, 24.971561, -118.256580, -71.112990, 90.509668, 106.428110, -48.983479, -125.540516, 128.000000, -24.971561, -118.256580, 71.112990, 90.509668, -106.428110, -48.983479, 125.540516, 128.000000, -71.112990, -48.983479, 125.540516, -90.509668, -24.971561, 118.256580, -106.428110, 128.000000, -106.428110, 48.983479, 24.971561, -90.509668, 125.540516, -118.256580, 71.112990, 128.000000, -125.540516, 118.256580, -106.428110, 90.509668, -71.112990, 48.983479, -24.971561
	};
/*
	for (int k = 0; k < 8; k++)
		for (int i = 0; i < 8; i++)
			printf("%f, ", 128.0f * cos((M_PI / 8.0f) * ((float)k + 0.5f) * i));
*/
const float row_thing[] = {
		181.019333, 177.541097, 167.240057, 150.512075, 127.999998, 100.568953, 69.273100, 35.315120, 181.019333, 150.512075, 69.273100, -35.315120, -127.999998, -177.541097, -167.240057, -100.568953, 181.019333, 100.568953, -69.273100, -177.541097, -127.999998, 35.315120, 167.240057, 150.512075, 181.019333, 35.315120, -167.240057, -100.568953, 127.999998, 150.512075, -69.273100, -177.541097, 181.019333, -35.315120, -167.240057, 100.568953, 127.999998, -150.512075, -69.273100, 177.541097, 181.019333, -100.568953, -69.273100, 177.541097, -127.999998, -35.315120, 167.240057, -150.512075, 181.019333, -150.512075, 69.273100, 35.315120, -127.999998, 177.541097, -167.240057, 100.568953, 181.019333, -177.541097, 167.240057, -150.512075, 127.999998, -100.568953, 69.273100, -35.315120
	};
/* This generated the values above;
for (int k = 0; k < 8; k++)
		for (int i = 0; i < 8; i++)
			printf("%f, ", 181.019336f * cos((M_PI / 8.0f) * ((float)k + 0.5f) * i));
*/
const float col_zero_thing[] = {
		0.000976, 0.000957, 0.000902, 0.000812, 0.000690, 0.000542, 0.000373, 0.000190, 0.000976, 0.000812, 0.000373, -0.000190, -0.000690, -0.000957, -0.000902, -0.000542, 0.000976, 0.000542, -0.000373, -0.000957, -0.000690, 0.000190, 0.000902, 0.000812, 0.000976, 0.000190, -0.000902, -0.000542, 0.000690, 0.000812, -0.000373, -0.000957, 0.000976, -0.000190, -0.000902, 0.000542, 0.000690, -0.000812, -0.000373, 0.000957, 0.000976, -0.000542, -0.000373, 0.000957, -0.000690, -0.000190, 0.000902, -0.000812, 0.000976, -0.000812, 0.000373, 0.000190, -0.000690, 0.000957, -0.000902, 0.000542, 0.000976, -0.000957, 0.000902, -0.000812, 0.000690, -0.000542, 0.000373, -0.000190
	};
/* This generated the values above;
	for (int k = 0; k < 8; k++)
		for (int i = 0; i < 8; i++)
			printf("%f, ", 0.000976f * cos((m_pi / 8.0f) * ((float)k + 0.5f) * i));
*/
const float col_thing[] = {
		0.001381, 0.001354, 0.001276, 0.001148, 0.000977, 0.000767, 0.000528, 0.000269, 0.001381, 0.001148, 0.000528, -0.000269, -0.000977, -0.001354, -0.001276, -0.000767, 0.001381, 0.000767, -0.000528, -0.001354, -0.000977, 0.000269, 0.001276, 0.001148, 0.001381, 0.000269, -0.001276, -0.000767, 0.000977, 0.001148, -0.000528, -0.001354, 0.001381, -0.000269, -0.001276, 0.000767, 0.000977, -0.001148, -0.000528, 0.001354, 0.001381, -0.000767, -0.000528, 0.001354, -0.000977, -0.000269, 0.001276, -0.001148, 0.001381, -0.001148, 0.000528, 0.000269, -0.000977, 0.001354, -0.001276, 0.000767, 0.001381, -0.001354, 0.001276, -0.001148, 0.000977, -0.000767, 0.000528, -0.000269
	};
/* This generated the values above;
	for (int k = 0; k < 8; k++)
		for (int i = 0; i < 8; i++)
			printf("%f, ", 0.001381f * cos((M_PI / 8.0f) * ((float)k + 0.5f) * i));
*/
/*
if (i == 0)
	sum += thing[i] * 128.0f * cos(eight_pi * ((float)k + 0.5f) * i);
else
	sum += thing[i] * 181.019336f * cos(eight_pi * ((float)k + 0.5f) * i);
*/
/*
	for (int i = 1; i < 8; i++)
		sum += thing[i] * 181.019336f * row_thing[k * 8 + i];
	data[k] = sum;
*/

void	idct_row(float *data)
{
	float	thing[8];
	int		non_zero_found;
	int		ind;
	int		k;

	non_zero_found = 0;
	k = -1;
	while (++k < 8)
	{
		thing[k] = data[k];
		if (thing[k] != 0)
			non_zero_found = 1;
	}
	if (!non_zero_found)
		return ;
	k = -1;
	while (++k < 8)
	{
		ind = k * 8;
		data[k]
			= (thing[0] * 128 * zero_thing[ind])
			+ (thing[1] * row_thing[ind + 1])
			+ (thing[2] * row_thing[ind + 2])
			+ (thing[3] * row_thing[ind + 3])
			+ (thing[4] * row_thing[ind + 4])
			+ (thing[5] * row_thing[ind + 5])
			+ (thing[6] * row_thing[ind + 6]);
			+ (thing[7] * row_thing[ind + 7]);
	}
}

/*
for (int i = 1; i < 8; i++)
	sum += thing[i] * 0.001381f * row_thing[k * 8 + i];
data[k * 8] = sum;
*/

void	idct_col(float *data)
{
	float	thing[8];
	int		ind;
	int		k;
	int		non_zero_found;

	non_zero_found = 0;
	k = -1;
	while (++k < 8)
	{
		thing[k] = data[8 * k];
		if (thing[k] != 0.0f)
			non_zero_found = 1;
	}
	if (!non_zero_found)
		return ;
	k = -1;
	while (++k < 8)
	{
		ind = k * 8;
		data[ind]
			= (thing[0] * col_zero_thing[ind])
			+ (thing[1] * col_thing[ind + 1])
			+ (thing[2] * col_thing[ind + 2])
			+ (thing[3] * col_thing[ind + 3])
			+ (thing[4] * col_thing[ind + 4])
			+ (thing[5] * col_thing[ind + 5])
			+ (thing[6] * col_thing[ind + 6])
			+ (thing[7] * col_thing[ind + 7]);
	}
}
/*
 * Makes all the bits in a byte, a byte;
 * 'dest' should be allocated already; (an amount of 8 obviously);
 * Returns success;
 * 
 * NOTE:
 * 	We are looping backwards because of endianness;
 */
int	bytify_bits(uint8_t *dest, uint8_t byte)
{
	for (int i = 0; i < 8; i++)
		dest[i] = ((byte >> (7 - i)) & 1) + 48; // +48 because we want ascii num;
	return (1);
}

uint16_t	swap_uint16(uint16_t a)
{
	return ((a << 8) | (a >> 8));
}

/*
void	read_jfif(FILE *fd, t_jfif *jfif)
{
	printf("[%s] Reading.\n", __FUNCTION__);
	jfif->bytes_read = 0;
	jfif->got = 1;

	jfif->bytes_read += fread(&jfif->length, 1, sizeof(uint16_t), fd);
	jfif->length = swap_uint16(jfif->length);

	jfif->bytes_read += fread(&jfif->identifier, 1, sizeof(uint8_t) * 5, fd);

	jfif->bytes_read += fread(&jfif->version, 1, sizeof(uint16_t), fd);
	jfif->version = swap_uint16(jfif->version);

	jfif->bytes_read += fread(&jfif->density, 1, sizeof(uint8_t), fd);

	jfif->bytes_read += fread(&jfif->x_density, 1, sizeof(uint16_t), fd);
	jfif->x_density = swap_uint16(jfif->x_density);

	jfif->bytes_read += fread(&jfif->y_density, 1, sizeof(uint16_t), fd);
	jfif->y_density = swap_uint16(jfif->y_density);

	jfif->bytes_read += fread(&jfif->x_thumbnail, 1, sizeof(uint8_t), fd);

	jfif->bytes_read += fread(&jfif->y_thumbnail, 1, sizeof(uint8_t), fd);

	jfif->thumbnail_size = 3 * jfif->x_thumbnail * jfif->y_thumbnail;
	jfif->thumbnail = malloc(sizeof(uint8_t) * jfif->thumbnail_size);
	for (int i = 0; i < jfif->thumbnail_size; i++)
		jfif->bytes_read += fread(&jfif->thumbnail[i], 1, sizeof(uint8_t), fd);

	printf("[%s] Read.\n", __FUNCTION__);
}
*/

void	show_jfif(t_jfif *jfif)
{
	printf("[%s]\n", __FUNCTION__);
	printf("\tlength : %d (0x%x)\n", jfif->length, jfif->length);
	printf("\tbytes_read : %d\n", jfif->bytes_read);
	printf("\tidentifier : %s (0x%x %x %x %x %x)\n", jfif->identifier, jfif->identifier[0], jfif->identifier[1], jfif->identifier[2], jfif->identifier[3], jfif->identifier[4]);
	printf("\tversion : %d %d\n",
		(jfif->version >> (8 * 0)) & 0xff, (jfif->version >> (8 * 1)) & 0xff);
	printf("\tdensity : %d\n", jfif->density);
	printf("\tx_density : %d\n", jfif->x_density);
	printf("\ty_density : %d\n", jfif->y_density);
	printf("\tx_thumbnail : %d\n", jfif->x_thumbnail);
	printf("\ty_thumbnail : %d\n", jfif->y_thumbnail);
}

void	show_dqt(t_dqt *dqt)
{
	if (dqt->length == 0)
		return ;
	printf("[%s]\n", __FUNCTION__);
	printf("\tlength : %d\n", dqt->length);
	printf("\tbytes read : %d\n", dqt->bytes_read);
	printf("\tinfo : precision : %d, table num : %d\n", dqt->info >> 4, dqt->info & 0xff);
	printf("\tdata :");
	for (int i = 0; i < 64; i++)
	{
		if (i % 8 == 0)
			printf("\n\t");
		printf("%-5d ", dqt->data[i]);
	}
	printf("\n");
}

void	show_sof(t_sof *sof0)
{
	printf("[%s]\n", __FUNCTION__);
	printf("\tlength : %d\n", sof0->length);
	printf("\tbytes read : %d\n", sof0->length);
	printf("\tprecision : %d\n", sof0->precision);
	printf("\theight : %d\n", sof0->height);
	printf("\twidth : %d\n", sof0->width);
	printf("\tcomp_amount : %d\n", sof0->comp_amount);
	for (int i = 0; i < sof0->comp_amount; i++)
		printf("\tComp[%d] ID=0x%02x, Samp Fac=0x%02x, QTable=0x%02x\n",
			i,
			sof0->components[i * 3 + 0],
			sof0->components[i * 3 + 1],
			sof0->components[i * 3 + 2]);
}

void	show_dht(t_dht *dht, uint8_t show_tree)
{
	if (dht->length == 0)
		return ;
	printf("[%s]\n", __FUNCTION__);
	printf("\tlength : %d\n", dht->length);
	printf("\tbytes_read : %d\n", dht->bytes_read);
	printf("\tinfo : %s %d\n", (dht->info >> 4) & 1 ? "AC" : "DC", dht->info & 0x0f);

// Printing the symbol and how many of those there are;
	int syms_done = -1; // symbols we have given their corresponding code len from 'symbols';
	for (int i = 0; i < 16; i++) // symbol count size == 16;
	{
		if (dht->symbol_count[i] == 0) // If we have no symbols of that code len;
			continue ;
		for (int j = 0; j < dht->symbol_count[i]; j++)
		{
			++syms_done;
			printf("Sym '%d' has %d code len (syms checked %d)\n", dht->symbols[syms_done], i + 1, syms_done);
		}
	}
	if (show_tree)
		print_hm_tree(dht->tree, 0);
}

void	show_imgdata(t_imgdata *imgdata)
{
	if (imgdata->amount == 0)
		return ;
	printf("[%s]\n", __FUNCTION__);
	printf("\tamount : %d\n", imgdata->amount);

	// Show actual value of byte, and then stringified byte;
	// Only showing 10 first, and 10 last
	if (BEDUGGER)
	{
		for (int i = 0; i < 10; i++)
		{
			printf("%02X\t", imgdata->unprocessed_data[i]);
			for (int j = 0; j < 8; j++)
				printf("%c", imgdata->data[i * 8 + j]);
			printf("\n");
		}
		for (int i = 0; i < 3; i++)
			printf(".\n");
		for (int i = imgdata->amount - 10; i < imgdata->amount; i++)
		{
			printf("%02X\t", imgdata->unprocessed_data[i]);
			for (int j = 0; j < 8; j++)
				printf("%c", imgdata->data[i * 8 + j]);
			printf("\n");
		}
	}

	printf("\n");
}

void	show_sos(t_sos *sos)
{
	if (sos->length == 0) // if segfault, set sos->length = 0;
		return ;
	printf("[%s]\n", __FUNCTION__);
	printf("\tlength : %d\n", sos->length);
	printf("\tbytes_read : %d\n", sos->bytes_read);
	printf("\tcount : %d\n", sos->count);
	for (int i = 0; i < sos->count * 2; i += 2)
		printf("\tData[%d] CompSelect=%d DCECTS=%d ACECTS=%d\n", i / 2,
			sos->data[i + 0], sos->data[i + 1] >> 4, sos->data[i + 1] & 0x0f);
	for (int i = 0; i < 3; i++)
		printf("\tSkip Byte[%d]=0x%x\n", i, sos->skip_bytes[i]);
}

void	idct(t_mcu *mcu)
{
	for (int comp = 0; comp < 3; comp++)
	{
		for (int row = 0; row < 8; row++)
			idct_row(mcu->data[comp] + row * 8);
		for (int col = 0; col < 8; col++)
			idct_col(mcu->data[comp] + col);
	}
}

void	ycbcr_to_rgb(t_mcu *mcu)
{
	float	y;
	float	cb;
	float	cr;

	for (int i = 0; i < 64; i++)
	{
		y	= mcu->data[0][i] + 128;
		cb	= mcu->data[1][i];
		cr	= mcu->data[2][i];

		mcu->rgb[0][i] = ft_clamp((y + (1.402f * cr)), 0, 255);
		mcu->rgb[1][i] = ft_clamp((y - (0.344136f * cb) - (0.71414f * cr)), 0, 255);
		mcu->rgb[2][i] = ft_clamp((y + (1.772f * cb)), 0, 255);
	}
}

/*
 * 'dqt' : array of 't_dqt' * 4;
*/
void new_mcu(t_mcu *mcu, t_dqt *dqt)
{
	/*
	int		dqt_index;

	dqt_index = 0;
	for (int comp = 0; comp < 3; comp++) 
	{
		int i = 0;
		int total_zeros = 0;
		for (; i < mcu->amount[comp]; i++)
		{
			if (mcu->zeros[comp][i] > 0)
			{
				for (int y = 0; y < mcu->zeros[comp][i]; y++, total_zeros++)
					mcu->data[comp][zigzag[i + total_zeros]] = 0;
			}
			if (i + total_zeros == 64)
				break ;
			mcu->data[comp][zigzag[i + total_zeros]] = mcu->coeffs[comp][i] * dqt[dqt_index].data[i + total_zeros];
		}
		for (; i + total_zeros < 64; i++)
			mcu->data[comp][zigzag[i + total_zeros]] = 0;
		dqt_index = 1;
	}
	*/
	int		dqt_index;

	dqt_index = 0;
	for (int comp = 0; comp < 3; comp++) 
	{
		for (int i = 0; i < 64; i++)
			mcu->data[comp][i] = dqt[dqt_index].data[zigzag[i]] * mcu->coeffs[comp][zigzag[i]];
		dqt_index = 1;
	}

	idct(mcu);
	ycbcr_to_rgb(mcu);
}

#define THREAD_AMOUNT 4

typedef struct s_temp_struct
{
	t_mcu	*mcu;
	int		per_thread_mcu;
	t_dqt	*dqt;
}	t_temp_struct;

void	*threaded_mcu_math(void	*structelini)
{
	t_temp_struct	*tempereloni = structelini;

	for (int i = 0; i < tempereloni->per_thread_mcu; i++)
		new_mcu(tempereloni->mcu + i, tempereloni->dqt);
}

int16_t	bit_str_to_value(char *bitstr, int len)
{
	int16_t	val;
	uint8_t	sign;
	int8_t	factor;

    val = 0x0000;
    sign = bitstr[0];
    factor = sign == '0' ? -1 : 1;
	/*
	write(1, bitstr, len);write(1, " ", 1);
	printf("sign : %d\n", sign);
	printf("factor : %d\n", factor);
	*/
    for (int i = 0; i < len; i++)
    {
        if (bitstr[i] == sign)
		{
		//	printf("in %d %d %d\n", len, i, len - 1 - i);
            val += ft_pow(2, len - 1 - i);
		}
    }
//	printf("%d\n", factor * val);
    return (factor * val);
}

int16_t	bit_str_to_value_v2(int16_t value, int len)
{
	int16_t	val;
	uint8_t	sign;
	int8_t	factor;

    val = 0x0000;
	sign = 0x00;
    if ((value >> len - 1) & 1)
		sign = 0x01;
    factor = sign == 0 ? -1 : 1;
	/*
	print_binary(value, len);
	printf(" sign : %d\n", sign);
	printf("factor : %d\n", factor);
	*/
    for (int i = len - 1; i >= 0; i--)
    {
		//printf("%d ? %d == %d\n", (value >> i) & 1, (value >> (len - 1)) & 1, !( ((value >> i) & 1) ^ ((value >> (len - 1)) & 1) ));
        if (!( ((value >> i) & 1) ^ ((value >> (len - 1)) & 1) ))
		{
		//	printf("in %d %d %d\n", len, i, len - 1 - i);
            val += ft_pow(2, i);
		}
    }
//	printf("%d\n", factor * val);
    return (factor * val);
}

void	bimgf_strncpy(char *dest, char *src, size_t n)
{
	size_t	i;

	i = 0;
	while (i < n)
	{
		dest[i] = src[i];
		i++;
	}
}

void	print_binary(int num, int n)
{
    int d, t;
    
    t = 0;
    while (--n >= 0)
    {
        d = num >> n;
        if (d & 1)
            write(1, "1", 1);
        else
            write(1, "0", 1);
        t++;
    }
}

/*
 * This 'curr_bit' means how many:eth bit in the byte youre looking at in 'data';
 */
uint16_t	get_n_bits(uint8_t *data, uint32_t curr_bit, uint8_t n)
{
	uint16_t	code;

	printf("in : %d <-\n", *data);
	code = 0;
	printf("%d %d\n", curr_bit, n);
	for (int i = 0; i < n; i++)
	{
		printf("%d", data[0]);
		code |= (data[i / 8] >> (7 - (i % 8))) << n - 1 - i;
		printf("i : %d\n", i);
	}
	printf("danskanen\n");
	return (code);
}

void	create_mcus_v2(t_mcu *mcu, t_imgdata *imgdata, t_dht *dht, int mcu_amount, t_sof *sof)
{
	uint16_t	code = 0;
	uint32_t	curr_bit = 0;

	int		count = 0;
	int		table_index = -1;

	uint8_t	comp_count_amount[3];

	for (int i = 0; i < sof->comp_amount; i++)
	{
		comp_count_amount[i] = 64 * (sof->components[i * 3 + 1] >> 4) * (sof->components[i * 3 + 1] & 0x0f);
		LG_INFO("Comp[%d] mcu size : %d", i , comp_count_amount[i]);
	}

	for (int nth_mcu = 0; nth_mcu < mcu_amount; nth_mcu++)
	{
		for (int comp = 0; comp < sof->comp_amount; comp++)
		{
			count = 0;
			while (count < 64)
			{
				table_index = count == 0 ? (comp == 0 ? 0 : 2) : (comp == 0 ? 1 : 3);
				uint8_t	symbol = 0;
				int len = 0;

				// Create the code from the next 16 bits;	
				code = 0;
				for (int i = 0; i < 16; i++)
				{
					code |= (imgdata->unprocessed_data[curr_bit / 8] >> (7 - (curr_bit % 8))) << 15 - i;
					curr_bit++;
				}
				
				if (!get_hm_symbol_with_code_v2(&symbol, code, &len, dht[table_index].tree))
				{
					printf("Count : %d, comp : %d, table_index : %d, curr_bit : %d, nth_mcu : %d\n", count, comp, table_index, curr_bit, nth_mcu);
					printf("0x%02X\n", imgdata->unprocessed_data[curr_bit / 8]);
					LG_WARN("code : %d", code);
					print_binary(code, 16);
					printf("\n");
					LG_ERROR("Len : %d, symbol : %d", len, symbol);
				}

				curr_bit -= 16 - len;

				int	zero_amount = -1;
				int	additional_bit_len = -1;
				int16_t	value = 0;

				if (symbol != 0x0000) // if not EOB;
				{
					zero_amount = symbol >> 4;
					additional_bit_len = symbol & 0x0F;

					for (int i = 0; i < additional_bit_len; i++)
					{
						value |= (imgdata->unprocessed_data[curr_bit / 8] >> (7 - (curr_bit % 8))) << additional_bit_len - 1 - i;
						curr_bit++;
					}
					value = bit_str_to_value_v2(value, additional_bit_len);

					for (int i = 0; i < zero_amount; i++, count++)
						mcu[nth_mcu].coeffs[comp][count] = 0;
					mcu[nth_mcu].coeffs[comp][count] = value;
					count++;

				}
				else // EOB found;
				{
					if (count != 0)
					{
						for (int i = count; i < 64; i++, count++)
							mcu[nth_mcu].coeffs[comp][count] = 0;
					}
					else // if the dc is EOB, dont quit yet, only if ac is eob;
					{
						mcu[nth_mcu].coeffs[comp][count] = 0;
						count++;
					}
				}
			}
			//printf("Comp %d had %d count;\n", comp, count);
		}
		if (nth_mcu > 0)
		{
			mcu[nth_mcu].coeffs[0][0] += mcu[nth_mcu - 1].coeffs[0][0];
			mcu[nth_mcu].coeffs[1][0] += mcu[nth_mcu - 1].coeffs[1][0];
			mcu[nth_mcu].coeffs[2][0] += mcu[nth_mcu - 1].coeffs[2][0];
		}
	}
}

typedef struct s_image_create_info
{
	t_mcu	*mcus;
	int		start_h;
	int		mcu_on_w;
	int		mcu_on_h;
	t_bimgf	*image;
}	t_image_create_info;

void	*threaded_img_creation(void *arg)
{
	t_image_create_info	*info;
	t_bimgf				*img;

	info = arg;
	img = info->image;
	for (int i = info->start_h; i < info->start_h + info->mcu_on_h; i++)
	{
		for (int j = 0; j < info->mcu_on_w; j++)
		{
			for (int y = 0; y < 8; y++)
			{
				int	y_pix = (i * 8 * img->line_w) + (y * img->line_w);
				for (int x = 0; x < 8; x++)
				{
					int x_pix = (j * 8 * 4) + (x * 4);
					int coord = y * 8 + x;
					int mcu_coord = (i * info->mcu_on_w) + j;
					int pix = y_pix + x_pix;
					img->pixels[pix + 0] = 255;
					img->pixels[pix + 1] = info->mcus[mcu_coord].rgb[2][coord];
					img->pixels[pix + 2] = info->mcus[mcu_coord].rgb[1][coord];
					img->pixels[pix + 3] = info->mcus[mcu_coord].rgb[0][coord];
				}
			}
		}
	}
}

void	create_image_from_mcus_v2(t_bimgf *image, t_mcu *mcus)
{
	int	pixel_amount;

	pixel_amount = image->w * image->h;
	image->bpp = 4;
	image->line_w = image->w * image->bpp;
	image->pixels = malloc(sizeof(unsigned char) * (image->line_w * image->h));
	printf("w : %d, h : %d, Pixel Amount : %d\n", image->w, image->h, pixel_amount);


	t_image_create_info	info[THREAD_AMOUNT];
	pthread_t			threads[THREAD_AMOUNT];

	int	mcu_on_w = image->w / 8;
	int	per_thread_amount_on_h = (image->h / 8) / THREAD_AMOUNT;
	int i = -1;
	while (++i < THREAD_AMOUNT)
	{
		info[i].start_h = i * per_thread_amount_on_h;
		info[i].mcu_on_h = per_thread_amount_on_h;
		info[i].mcu_on_w = mcu_on_w;
		info[i].mcus = mcus;
		info[i].image = image;
		if (pthread_create(&threads[i], NULL, threaded_img_creation, &info[i]))
			LG_ERROR("Couldnt create image creation thread.");
	}
	while (--i >= 0)
		pthread_join(threads[i], NULL);


	LG_INFO("Done Here");
}

/*
 * 'mcus' array of 't_mcu';
 * 'image' pointer to 't_bimgf' given by the user; can be malloced or not, dont care;
*/
void	create_image_from_mcus(t_bimgf *image, t_mcu *mcus)
{
	int	pixel_amount;

	pixel_amount = image->w * image->h;
	image->bpp = 4;
	image->line_w = image->w * image->bpp;
	image->pixels = malloc(sizeof(unsigned char) * (image->line_w * image->h));
	printf("w : %d, h : %d, Pixel Amount : %d\n", image->w, image->h, pixel_amount);


	int	mcu_on_w = image->w / 8;
	int	mcu_on_h = image->h / 8;

	for (int i = 0; i < mcu_on_h; i++)
	{
		for (int j = 0; j < mcu_on_w; j++)
		{
			for (int y = 0; y < 8; y++)
			{
				int	y_pix = (i * 8 * image->line_w) + (y * image->line_w);
				for (int x = 0; x < 8; x++)
				{
					int x_pix = (j * 8 * 4) + (x * 4);
					int coord = y * 8 + x;
					int mcu_coord = (i * mcu_on_w) + j;
					int pix = y_pix + x_pix;
					image->pixels[pix + 0] = 255;
					image->pixels[pix + 1] = mcus[mcu_coord].rgb[2][coord];
					image->pixels[pix + 2] = mcus[mcu_coord].rgb[1][coord];
					image->pixels[pix + 3] = mcus[mcu_coord].rgb[0][coord];
				}
			}
		}
	}
	LG_INFO("Done Here");
}


//////////////////////////////////////////
// NEW STUFF STARTS HERE
//////////////////////////////////////////

typedef struct	s_sos_thread
{
	unsigned char	*file_content;
	int32_t			index;
	t_sos			*sos;
	t_imgdata		*imgdata;
}	t_sos_thread;

void	read_imgdata_v2(uint8_t *content, t_imgdata *data, int32_t byte)
{
	uint8_t		bits[8];
	uint16_t	skipped = 0;
	int			data_allocated = 0;

	data->data = NULL;
	data->amount = 0;
	data->bytes_read = 0;
	data->unprocessed_data = NULL;

	uint8_t	curr_byte;
	uint8_t	last_byte;

	last_byte = content[byte];
	while (1)
	{
		curr_byte = content[++byte];

		if (last_byte == 0xff)
		{
			if (curr_byte == BIMGF_JPG_EOI) // EOI
				break ;
			else if (curr_byte == 0x00) // skip it;
			{
				curr_byte = content[++byte];
				skipped++;
			}
			else
				LG_ERROR("Marker 0x%02X found in the middle of image data.", curr_byte);
		}
		if (data_allocated == 0)
		{
			data_allocated = 1024 * 1024 * 3;
			data->unprocessed_data = malloc(data_allocated);
		}
		else if ((data->amount + 1) * 8 >= data_allocated) // We are aiming to never come in here!
		{
			data_allocated += 1024 * 1024 * 3;
			data->unprocessed_data = realloc(data->unprocessed_data, data_allocated);
			LG_WARN("Allocating more memory! (this should not happen)");
		}
		data->unprocessed_data[data->amount] = last_byte;
		data->amount++;
		last_byte = curr_byte;
	}
}

void	*sos_thread_func(void *arg)
{
	t_sos_thread	*info;
	uint8_t			*content;
	int32_t			byte;
	t_sos			*sos;
	
	info = arg;
	sos = info->sos;
	content = info->file_content + info->index;
	byte = -1;
	sos->length = content[++byte] << 8 | content[++byte];
	sos->count = content[++byte];
	int i = 0;
	int	p = sos->count * 2;
	sos->data = malloc(p);
	while (i < p) 
	{
		sos->data[i + 0] = content[++byte];
		sos->data[i + 1] = content[++byte];
		i += 2;
	}
	sos->skip_bytes[0] = content[++byte];
	sos->skip_bytes[1] = content[++byte];
	sos->skip_bytes[2] = content[++byte];

	read_imgdata_v2(content, info->imgdata, ++byte);
}

typedef struct	s_dht_thread
{
	uint8_t		*file_content;
	int32_t		*index;
	t_dht		*dht;
	int8_t		amount;
}	t_dht_thread;

void	*dht_thread_func(void *arg)
{
	t_dht_thread	*info;
	uint8_t			*content;
	int32_t			byte;
	t_dht			*dht;
	
	info = arg;
	
	int i = -1;
	while (++i < info->amount)
	{
		dht = &info->dht[i];
		content = info->file_content + info->index[i];
		byte = -1;

		dht->length = content[++byte] << 8 | content[++byte];
		while (byte + 1 < dht->length)
		{
			dht->info = content[++byte];
			dht->total_symbol_count = 0;
			for (int i = 0; i < 16; i++)
			{
				dht->symbol_count[i] = content[++byte];
				dht->total_symbol_count += dht->symbol_count[i];
			}

			dht->symbols = malloc(dht->total_symbol_count);
			for (int i = 0; i < dht->total_symbol_count; i++)
				dht->symbols[i] = content[++byte];

			dht->tree = build_hm_tree(dht->symbol_count, dht->symbols);
		}
		dht->bytes_read = byte + 1;
	}
}

typedef struct	s_dqt_thread
{
	uint8_t		*file_content;
	int32_t		*index;
	t_dqt		*dqt;
	int8_t		amount;
}	t_dqt_thread;

void	*dqt_thread_func(void *arg)
{
	t_dqt_thread	*info;
	uint8_t			*content;
	int32_t			byte;
	t_dqt			*dqt;
	
	info = arg;
	
	int i = -1;
	while (++i < info->amount)
	{
		dqt = &info->dqt[i];
		content = info->file_content + info->index[i];
		byte = -1;

		dqt->length = content[++byte] << 8 | content[++byte];
		dqt->info = content[++byte];
		if (dqt->info >> 4 == 0) // 8 bit
		{
			for (int a = 0; a < 64; a++)
				dqt->data[a] = 0x00 << 8 | content[++byte];
		}
		else if (dqt->info >> 4 == 1) // 16 bit
		{
			LG_WARN("This is untested, might not work correctly.");
			for (int b = 0; b < 64; b++) // untested;
				dqt->data[b] = content[++byte] << 8 | content[++byte];
		}
	}
}

typedef struct	s_sof_thread
{
	uint8_t		*file_content;
	int32_t		index;
	t_sof		*sof;
}	t_sof_thread;

void	*sof_thread_func(void *arg)
{
	t_sof_thread	*info;
	uint8_t			*content;
	int32_t			byte;
	t_sof			*sof;
	
	info = arg;
	sof = info->sof;
	content = info->file_content + info->index;
	byte = -1;

	sof->length = content[++byte] << 8 | content[++byte];

	sof->precision = content[++byte];

	sof->height = content[++byte] << 8 | content[++byte];
	sof->width = content[++byte] << 8 | content[++byte];

	sof->comp_amount = content[++byte];
	sof->components = malloc(sof->comp_amount * 3);
	for (int i = 0; i < sof->comp_amount; i++)
	{
		sof->components[i * 3 + 0] = content[++byte];
		sof->components[i * 3 + 1] = content[++byte];
		sof->components[i * 3 + 2] = content[++byte];
	}

	sof->bytes_read = byte;
}

#include "sys/stat.h"
char	*bimgf_file_content(char *file)
{
	char	*final;
	FILE	*fp;
	int		fd;
	struct stat	file_stat;
	int		file_len;

	if (!file)
		return (NULL);
	fp = fopen(file, "rb");
	if (!fp)
		return (NULL);
	fd = fileno(fp);
	fstat(fd, &file_stat);
	file_len = file_stat.st_size;
	final = malloc(file_len + 1);
	fread(final, 1, file_len, fp);
	final[file_len] = '\0';
	fclose(fp);
	return (final);
}

int	bimgf_load_jpg(t_bimgf *image, const char *file_path)
{
	unsigned char	*file_content;
	int				curr_byte = -1;
	int				dht_index[4] = {-1};
	int				dqt_index[2] = {-1};
	int				sof_index = -1;
	int				sos_index = -1;
	int				eoi_index = -1;

	int				dht_amount = -1;
	int				dqt_amount = -1;

	// Read the stuff and get the indices;
	file_content = bimgf_file_content((char *)file_path);
	if (!file_content)
	{
		LG_WARN("Couldn\'t read file %s.", file_path);
		return (0);
	}
	LG_INFO("File content got.");
	while (1)
	{
		if (file_content[++curr_byte] == 0xff)
		{
			curr_byte++;
			if (file_content[curr_byte] == BIMGF_JPG_DHT)
				dht_index[++dht_amount] = ++curr_byte;
			else if (file_content[curr_byte] == BIMGF_JPG_DQT)
				dqt_index[++dqt_amount] = ++curr_byte;
			else if (file_content[curr_byte] == BIMGF_JPG_SOF0)
				sof_index = ++curr_byte;
			else if (file_content[curr_byte] == BIMGF_JPG_SOS)
				sos_index = ++curr_byte;
			else if (file_content[curr_byte] == BIMGF_JPG_EOI)
			{
				eoi_index = curr_byte;
				break ;
			}
			else if (file_content[curr_byte] == BIMGF_JPG_SOF2)
				LG_ERROR("SOF2 not supported yet.");
			else if (file_content[curr_byte])
				LG_WARN("Unsupported marker found. 0x%02x%02x", file_content[curr_byte - 1], file_content[curr_byte]);
		}
	}

	if (BEDUGGER)
	{
		LG_INFO("At %d:th byte", curr_byte);
		int i = -1;
		while (++i < dht_amount)
			LG_INFO("DHT[%d] index %d", i, dht_index[i]);
		i = -1;
		while (++i < dqt_amount)
			LG_INFO("DQT[%d] index %d", i, dqt_index[i]);
		LG_INFO("SOF index %d", sof_index);
		LG_INFO("SOS index %d", sos_index);
		LG_INFO("EOI index %d", eoi_index);
	}


	// Thread the stuffs;
	pthread_t		sos_thread;
	t_sos			sos;
	t_imgdata		imgdata;
	t_sos_thread	sos_thread_info = {file_content, sos_index, &sos, &imgdata};
	sos_thread_func(&sos_thread_info);
	/*
	if (pthread_create(&sos_thread, NULL, sos_thread_func, &sos_thread_info))
		LG_ERROR("Couldnt create sos thread.");
		*/
	if (BEDUGGER)
		LG_INFO("Done with sos");

	pthread_t		dht_thread;
	t_dht			dht[4];
	t_dht_thread	dht_thread_info = {file_content, dht_index, dht, dht_amount + 1};
	dht_thread_func(&dht_thread_info);
	/*
	if (pthread_create(&dht_thread, NULL, dht_thread_func, &dht_thread_info))
		LG_ERROR("Couldnt create dht thread.");
		*/
	if (BEDUGGER)
	{
		printf("DHT amount : %d\n", dht_amount);
		for (int i = 0; i < dht_amount; i++)
			show_dht(&dht[i], i % 2 == 0);
	}

	pthread_t		dqt_thread;
	t_dqt			dqt[2];
	t_dqt_thread	dqt_thread_info = {file_content, dqt_index, dqt, dqt_amount + 1};
	dqt_thread_func(&dqt_thread_info);
	/*
	if (pthread_create(&dqt_thread, NULL, dqt_thread_func, &dqt_thread_info))
		LG_ERROR("Couldnt create dqt thread.");
		*/
	if (BEDUGGER)
		LG_INFO("Done with dqt");

	// Lets have sof on the main thread so that it also does something;
	t_sof			sof;
	t_sof_thread	sof_thread_info = {file_content, sof_index, &sof};
	sof_thread_func(&sof_thread_info);

	if (BEDUGGER)
	{
		show_sof(&sof);
	}
/*
	pthread_join(sos_thread, NULL);
	pthread_join(dht_thread, NULL);
	pthread_join(dqt_thread, NULL);
	*/

	if (BEDUGGER)
		LG_INFO("Starting mcu creation.");

	int mcu_amount = sof.width * sof.height / 64;
	t_mcu *mcu = malloc(sizeof(t_mcu) * mcu_amount);

	//create_mcus(mcu, &imgdata, dht, mcu_amount);
	create_mcus_v2(mcu, &imgdata, dht, mcu_amount, &sof);

/*
	for(int i = 0; i < mcu_amount; i++)
		new_mcu(&mcu[i], dqt);
		*/
	pthread_t threads[THREAD_AMOUNT];
	t_temp_struct	temps[THREAD_AMOUNT];
	int per_thread_mcu = mcu_amount / THREAD_AMOUNT;
	int i = -1;
	while (++i < THREAD_AMOUNT)
	{
		temps[i].mcu = mcu + (i * per_thread_mcu);
		temps[i].per_thread_mcu = per_thread_mcu;
		temps[i].dqt = dqt;
		if (pthread_create(&threads[i], NULL, threaded_mcu_math, &temps[i]))
			LG_ERROR("Couldnt create mcu thread.");
	}
	while (--i >= 0)
		pthread_join(threads[i], NULL);

	image->w = sof.width;
	image->h = sof.height;
	create_image_from_mcus_v2(image, mcu);

	return (1);
}