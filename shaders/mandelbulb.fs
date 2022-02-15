#version 330 core

#define MAX_ITERATIONS 95

in vec4 gl_FragCoord;

out vec4 FragColor;

int get_iterations()
{
	float real = (gl_FragCoord.x / 1280.0 - 0.6) * 4.0;
	float imag = (gl_FragCoord.y / 1280.0 - 0.3) * 4.0;

	int iterations = 0;
	float const_real = real;
	float const_imag = imag;

	while (iterations < MAX_ITERATIONS)
	{
		float tmp_real = real;
		real = (real * real - imag * imag) + const_real;
		imag = (2.0 * tmp_real * imag) + const_imag;

		float dist = real * real + imag * imag;

		if (dist > 4.0)
			break;
		++iterations;
	}
	return iterations;
}

vec4 return_color()
{
	int iter = get_iterations();
	if (iter == MAX_ITERATIONS)
		return vec4(0.0f, 0.0f, 0.0f, 1.0f);

	float iterations = float(iter) / MAX_ITERATIONS;
	return vec4(0.0f, iterations, 0.0f, 1.0f);
}

void main()
{
	FragColor = return_color();
}
