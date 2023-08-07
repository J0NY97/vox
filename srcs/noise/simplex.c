/*
 *  Author: Jony Salmi
 *  Create Time: 30.09.2022 20:45:03
 *  Modified by: Jony Salmi
 *  Modified time: 03.08.2023 13:13:38
 */

/*
 * Everything yoinked from : 
 * https://weber.itn.liu.se/~stegu/simplexnoise/simplexnoise.pdf
 * https://weber.itn.liu.se/~stegu/simplexnoise/SimplexNoise.java
*/

static int grad3[24][3] = {
	{1,1,0},{-1,1,0},{1,-1,0},{-1,-1,0},
	{1,0,1},{-1,0,1},{1,0,-1},{-1,0,-1},
	{0,1,1},{0,-1,1},{0,1,-1},{0,-1,-1}};

static short perm[] = {
	151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,
	142,8,99,37,240,21,10,23,190,6,148,247,120,234,75,0,26,197,62,94,252,219,
	203,117,35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168,68,175,
	74,165,71,134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,230,
	220,105,92,41,55,46,245,40,244,102,143,54,65,25,63,161,1,216,80,73,209,76,
	132,187,208,89,18,169,200,196,135,130,116,188,159,86,164,100,109,198,173,
	186,3,64,52,217,226,250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,
	59,227,47,16,58,17,182,189,28,42,223,183,170,213,119,248,152,2,44,154,163,
	70,221,153,101,155,167,43,172,9,129,22,39,253,19,98,108,110,79,113,224,232,
	178,185,112,104,218,246,97,228,251,34,242,193,238,210,144,12,191,179,162,
	241,81,51,145,235,249,14,239,107,49,192,214,31,181,199,106,157,184,84,204,
	176,115,121,50,45,127,4,150,254,138,236,205,93,222,114,67,29,24,72,243,141,
	128,195,78,66,215,61,156,180,
	151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,
	142,8,99,37,240,21,10,23,190,6,148,247,120,234,75,0,26,197,62,94,252,219,
	203,117,35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168,68,175,
	74,165,71,134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,230,
	220,105,92,41,55,46,245,40,244,102,143,54,65,25,63,161,1,216,80,73,209,76,
	132,187,208,89,18,169,200,196,135,130,116,188,159,86,164,100,109,198,173,
	186,3,64,52,217,226,250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,
	59,227,47,16,58,17,182,189,28,42,223,183,170,213,119,248,152,2,44,154,163,
	70,221,153,101,155,167,43,172,9,129,22,39,253,19,98,108,110,79,113,224,232,
	178,185,112,104,218,246,97,228,251,34,242,193,238,210,144,12,191,179,162,
	241,81,51,145,235,249,14,239,107,49,192,214,31,181,199,106,157,184,84,204,
	176,115,121,50,45,127,4,150,254,138,236,205,93,222,114,67,29,24,72,243,141,
	128,195,78,66,215,61,156,180};

static short perm_mod_12[512] = {
	151 % 12,160 % 12,137 % 12,91 % 12,90 % 12,15 % 12,131 % 12,13 % 12,201 % 12,95 % 12,96 % 12,53 % 12,194 % 12,233 % 12,7 % 12,225 % 12,140 % 12,36 % 12,103 % 12,30 % 12,69 % 12,
	142 % 12,8 % 12,99 % 12,37 % 12,240 % 12,21 % 12,10 % 12,23 % 12,190 % 12,6 % 12,148 % 12,247 % 12,120 % 12,234 % 12,75 % 12,0 % 12,26 % 12,197 % 12,62 % 12,94 % 12,252 % 12,219 % 12,
	203 % 12,117 % 12,35 % 12,11 % 12,32 % 12,57 % 12,177 % 12,33 % 12,88 % 12,237 % 12,149 % 12,56 % 12,87 % 12,174 % 12,20 % 12,125 % 12,136 % 12,171 % 12,168 % 12,68 % 12,175 % 12,
	74 % 12,165 % 12,71 % 12,134 % 12,139 % 12,48 % 12,27 % 12,166 % 12,77 % 12,146 % 12,158 % 12,231 % 12,83 % 12,111 % 12,229 % 12,122 % 12,60 % 12,211 % 12,133 % 12,230 % 12,
	220 % 12,105 % 12,92 % 12,41 % 12,55 % 12,46 % 12,245 % 12,40 % 12,244 % 12,102 % 12,143 % 12,54 % 12,65 % 12,25 % 12,63 % 12,161 % 12,1 % 12,216 % 12,80 % 12,73 % 12,209 % 12,76 % 12,
	132 % 12,187 % 12,208 % 12,89 % 12,18 % 12,169 % 12,200 % 12,196 % 12,135 % 12,130 % 12,116 % 12,188 % 12,159 % 12,86 % 12,164 % 12,100 % 12,109 % 12,198 % 12,173 % 12,
	186 % 12,3 % 12,64 % 12,52 % 12,217 % 12,226 % 12,250 % 12,124 % 12,123 % 12,5 % 12,202 % 12,38 % 12,147 % 12,118 % 12,126 % 12,255 % 12,82 % 12,85 % 12,212 % 12,207 % 12,206 % 12,
	59 % 12,227 % 12,47 % 12,16 % 12,58 % 12,17 % 12,182 % 12,189 % 12,28 % 12,42 % 12,223 % 12,183 % 12,170 % 12,213 % 12,119 % 12,248 % 12,152 % 12,2 % 12,44 % 12,154 % 12,163 % 12,
	70 % 12,221 % 12,153 % 12,101 % 12,155 % 12,167 % 12,43 % 12,172 % 12,9 % 12,129 % 12,22 % 12,39 % 12,253 % 12,19 % 12,98 % 12,108 % 12,110 % 12,79 % 12,113 % 12,224 % 12,232 % 12,
	178 % 12,185 % 12,112 % 12,104 % 12,218 % 12,246 % 12,97 % 12,228 % 12,251 % 12,34 % 12,242 % 12,193 % 12,238 % 12,210 % 12,144 % 12,12 % 12,191 % 12,179 % 12,162 % 12,
	241 % 12,81 % 12,51 % 12,145 % 12,235 % 12,249 % 12,14 % 12,239 % 12,107 % 12,49 % 12,192 % 12,214 % 12,31 % 12,181 % 12,199 % 12,106 % 12,157 % 12,184 % 12,84 % 12,204 % 12,
	176 % 12,115 % 12,121 % 12,50 % 12,45 % 12,127 % 12,4 % 12,150 % 12,254 % 12,138 % 12,236 % 12,205 % 12,93 % 12,222 % 12,114 % 12,67 % 12,29 % 12,24 % 12,72 % 12,243 % 12,141 % 12,
	128 % 12,195 % 12,78 % 12,66 % 12,215 % 12,61 % 12,156 % 12,180 % 12,

	151 % 12,160 % 12,137 % 12,91 % 12,90 % 12,15 % 12,131 % 12,13 % 12,201 % 12,95 % 12,96 % 12,53 % 12,194 % 12,233 % 12,7 % 12,225 % 12,140 % 12,36 % 12,103 % 12,30 % 12,69 % 12,
	142 % 12,8 % 12,99 % 12,37 % 12,240 % 12,21 % 12,10 % 12,23 % 12,190 % 12,6 % 12,148 % 12,247 % 12,120 % 12,234 % 12,75 % 12,0 % 12,26 % 12,197 % 12,62 % 12,94 % 12,252 % 12,219 % 12,
	203 % 12,117 % 12,35 % 12,11 % 12,32 % 12,57 % 12,177 % 12,33 % 12,88 % 12,237 % 12,149 % 12,56 % 12,87 % 12,174 % 12,20 % 12,125 % 12,136 % 12,171 % 12,168 % 12,68 % 12,175 % 12,
	74 % 12,165 % 12,71 % 12,134 % 12,139 % 12,48 % 12,27 % 12,166 % 12,77 % 12,146 % 12,158 % 12,231 % 12,83 % 12,111 % 12,229 % 12,122 % 12,60 % 12,211 % 12,133 % 12,230 % 12,
	220 % 12,105 % 12,92 % 12,41 % 12,55 % 12,46 % 12,245 % 12,40 % 12,244 % 12,102 % 12,143 % 12,54 % 12,65 % 12,25 % 12,63 % 12,161 % 12,1 % 12,216 % 12,80 % 12,73 % 12,209 % 12,76 % 12,
	132 % 12,187 % 12,208 % 12,89 % 12,18 % 12,169 % 12,200 % 12,196 % 12,135 % 12,130 % 12,116 % 12,188 % 12,159 % 12,86 % 12,164 % 12,100 % 12,109 % 12,198 % 12,173 % 12,
	186 % 12,3 % 12,64 % 12,52 % 12,217 % 12,226 % 12,250 % 12,124 % 12,123 % 12,5 % 12,202 % 12,38 % 12,147 % 12,118 % 12,126 % 12,255 % 12,82 % 12,85 % 12,212 % 12,207 % 12,206 % 12,
	59 % 12,227 % 12,47 % 12,16 % 12,58 % 12,17 % 12,182 % 12,189 % 12,28 % 12,42 % 12,223 % 12,183 % 12,170 % 12,213 % 12,119 % 12,248 % 12,152 % 12,2 % 12,44 % 12,154 % 12,163 % 12,
	70 % 12,221 % 12,153 % 12,101 % 12,155 % 12,167 % 12,43 % 12,172 % 12,9 % 12,129 % 12,22 % 12,39 % 12,253 % 12,19 % 12,98 % 12,108 % 12,110 % 12,79 % 12,113 % 12,224 % 12,232 % 12,
	178 % 12,185 % 12,112 % 12,104 % 12,218 % 12,246 % 12,97 % 12,228 % 12,251 % 12,34 % 12,242 % 12,193 % 12,238 % 12,210 % 12,144 % 12,12 % 12,191 % 12,179 % 12,162 % 12,
	241 % 12,81 % 12,51 % 12,145 % 12,235 % 12,249 % 12,14 % 12,239 % 12,107 % 12,49 % 12,192 % 12,214 % 12,31 % 12,181 % 12,199 % 12,106 % 12,157 % 12,184 % 12,84 % 12,204 % 12,
	176 % 12,115 % 12,121 % 12,50 % 12,45 % 12,127 % 12,4 % 12,150 % 12,254 % 12,138 % 12,236 % 12,205 % 12,93 % 12,222 % 12,114 % 12,67 % 12,29 % 12,24 % 12,72 % 12,243 % 12,141 % 12,
	128 % 12,195 % 12,78 % 12,66 % 12,215 % 12,61 % 12,156 % 12,180 % 12,
};

// Skewing and unskewing factors;
static const float sqrt_3 = 1.73205080757f;
static const float F2 = 0.5f * (sqrt_3 - 1.0f);
static const float G2 = (3.0f - sqrt_3) / 6.0f;
static const float F3 = 1.0f / 3.0f;
static const float G3 = 1.0f / 5.0f;

static int fastfloor(float x)
{
	return x > 0 ? (int)x : (int)x - 1;
}

static float dot2d(int g[], float x, float y)
{
	return g[0] * x + g[1] * y;
}

static float dot3d(int g[], float x, float y, float z)
{
	return g[0] * x + g[1] * y + g[2] * z;
}

float simplex_noise_2d(float xin, float yin)
{
	float n0, n1, n2;
	float s = (xin + yin) * F2;
	int i = fastfloor(xin + s);
	int j = fastfloor(yin + s);
	float t = (i + j) * G2;
	float X0 = i - t;
	float Y0 = j - t;
	float x0 = xin - X0;
	float y0 = yin - Y0;
	
	int i1, j1;
	if(x0 > y0)
	{
		i1 = 1;
		j1 = 0;
	}
	else
	{
		i1 = 0;
		j1 = 1;
	}

	float x1 = x0 - i1 + G2;
	float y1 = y0 - j1 + G2;
	float x2 = x0 - 1.0 + 2.0 * G2;
	float y2 = y0 - 1.0 + 2.0 * G2;

	int ii = i & 255;
	int jj = j & 255;
	int gi0 = perm_mod_12[ii + perm[jj]];
	int gi1 = perm_mod_12[ii + i1 + perm[jj + j1]];
	int gi2 = perm_mod_12[ii + 1 + perm[jj + 1]];

	float t0 = 0.5 - x0 * x0 - y0 * y0;
	if(t0 < 0)
		n0 = 0.0;
	else
	{
		t0 *= t0;
		n0 = t0 * t0 * dot2d(grad3[gi0], x0, y0); // (x,y) of grad3 used for 2D gradient
	}

	float t1 = 0.5 - x1 * x1 - y1 * y1;
	if(t1 < 0)
		n1 = 0.0;
	else
	{
		t1 *= t1;
		n1 = t1 * t1 * dot2d(grad3[gi1], x1, y1);
	}

	float t2 = 0.5 - x2 * x2 - y2 * y2;
	if(t2 < 0)
		n2 = 0.0;
	else
	{
		t2 *= t2;
		n2 = t2 * t2 * dot2d(grad3[gi2], x2, y2);
	}

	return 70.0 * (n0 + n1 + n2);
}

float	simplex_noise_2d_octave(float xin, float zhao, float amplitude, float frequency, int octaves, float persistence, float lacunarity)
{
	float	value;

	value = 0;
	for (int i = 0; i < octaves; i++)
	{
		value += amplitude * simplex_noise_2d(xin * frequency, zhao * frequency);
		amplitude *= persistence;
		frequency *= lacunarity;
	}
	return (value);
}

float	simplex_noise_3d(float xin, float yin, float zin)
{
	float n0, n1, n2, n3;
	float s = (xin+yin+zin)*F3;
	int i = fastfloor(xin+s);
	int j = fastfloor(yin+s);
	int k = fastfloor(zin+s);
	float t = (i+j+k)*G3;
	float X0 = i-t;
	float Y0 = j-t;
	float Z0 = k-t;
	float x0 = xin-X0;
	float y0 = yin-Y0;
	float z0 = zin-Z0;
	int i1, j1, k1;
	int i2, j2, k2;

	if (x0 >= y0)
	{
		if (y0 >= z0)
		{
			i1=1; j1=0; k1=0; i2=1; j2=1; k2=0;
		}
		else if (x0 >= z0)
		{
			i1=1; j1=0; k1=0; i2=1; j2=0; k2=1;
		}
		else
		{
			i1=0; j1=0; k1=1; i2=1; j2=0; k2=1;
		}
	}
	else
	{
		if(y0<z0)
		{
			i1=0; j1=0; k1=1; i2=0; j2=1; k2=1;
		}
		else if (x0<z0)
		{
			i1=0; j1=1; k1=0; i2=0; j2=1; k2=1;
		}
		else
		{
			i1=0; j1=1; k1=0; i2=1; j2=1; k2=0;
		}
	}

	float x1 = x0 - i1 + G3;
	float y1 = y0 - j1 + G3;
	float z1 = z0 - k1 + G3;
	float x2 = x0 - i2 + 2.0 * G3;
	float y2 = y0 - j2 + 2.0 * G3;
	float z2 = z0 - k2 + 2.0 * G3;
	float x3 = x0 - 1.0 + 3.0 * G3;
	float y3 = y0 - 1.0 + 3.0 * G3;
	float z3 = z0 - 1.0 + 3.0 * G3;

	int ii = i & 255;
	int jj = j & 255;
	int kk = k & 255;
	int gi0 = perm_mod_12[ii + perm[jj + perm[kk]]];
	int gi1 = perm_mod_12[ii + i1 + perm[jj + j1 + perm[kk + k1]]];
	int gi2 = perm_mod_12[ii + i2 + perm[jj + j2 + perm[kk + k2]]];
	int gi3 = perm_mod_12[ii + 1 + perm[jj + 1 + perm[kk + 1]]];

	float t0 = 0.5 - x0 * x0 - y0 * y0 - z0 * z0;

	if (t0 < 0)
		n0 = 0.0;
	else
	{
		t0 *= t0;
		n0 = t0 * t0 * dot3d(grad3[gi0], x0, y0, z0);
	}

	float t1 = 0.6 - x1*x1 - y1*y1 - z1*z1;

	if (t1 < 0)
		n1 = 0.0;
	else
	{
		t1 *= t1;
		n1 = t1 * t1 * dot3d(grad3[gi1], x1, y1, z1);
	}

	float t2 = 0.6 - x2*x2 - y2*y2 - z2*z2;

	if (t2 < 0)
		n2 = 0.0;
	else
	{
		t2 *= t2;
		n2 = t2 * t2 * dot3d(grad3[gi2], x2, y2, z2);
	}

	float t3 = 0.6 - x3 * x3 - y3 * y3 - z3 * z3;

	if (t3 < 0)
		n3 = 0.0;
	else
	{
		t3 *= t3;
		n3 = t3 * t3 * dot3d(grad3[gi3], x3, y3, z3);
	}
	return 32.0*(n0 + n1 + n2 + n3);
}

float	simplex_noise_3d_octave(float x, float y, float z, float amplitude, float frequency, int octaves, float persistence, float lacunarity)
{
	float	value;

	value = 0;
	for (int i = 0; i < octaves; i++)
	{
		value += amplitude * simplex_noise_3d(x * frequency, y * frequency, z * frequency);
		amplitude *= persistence;
		frequency *= lacunarity;
	}
	return (value);
}