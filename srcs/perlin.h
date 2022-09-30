/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   perlin.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsalmi <jsalmi@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/07 13:21:16 by jsalmi            #+#    #+#             */
/*   Updated: 2022/09/30 20:36:24 by jsalmi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PERLIN_H
# define PERLIN_H

float	noise2d_octave(float x, float y, float amplitude, float frequency, int octaves, float persistence, float lacunarity);
float	noise2d(float x, float y);
float	noise3d(float x, float y, float z);
float	noise3d_octave(float x, float y, float z, float amplitude, float frequency, int octaves, float persistence, float lacunarity);

#endif