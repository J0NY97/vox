/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shader.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsalmi <jsalmi@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/18 14:12:22 by jsalmi            #+#    #+#             */
/*   Updated: 2022/06/30 12:26:15 by jsalmi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SHADER_H
# define SHADER_H

void		new_shader(GLuint *shader, const char *vs, const char *fs);
int			create_shader(GLuint *shader, const char *content, GLenum type);
void		check_program_errors(GLuint program);

#endif
