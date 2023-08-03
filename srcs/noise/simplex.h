/*
 *  Author: Jony Salmi
 *  Create Time: 30.09.2022 21:11:13
 *  Modified by: Jony Salmi
 *  Modified time: 30.09.2022 21:41:28
 */

#ifndef SIMPLEX_H
#define SIMPLEX_H

// NOISE 2D
float	simplex_noise_2d(float xin, float yin);
float	simplex_noise_2d_octave(float xin, float zhao, float amplitude, float frequency, int octaves, float persistence, float lacunarity);

// NOISE 3D
float	simplex_noise_3d(float xin, float yin, float zin);
float	simplex_noise_3d_octave(float x, float y, float z, float amplitude, float frequency, int octaves, float persistence, float lacunarity);

#endif
