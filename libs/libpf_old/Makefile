# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: nneronin <nneronin@student.hive.fi>        +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2020/09/23 13:48:12 by nneronin          #+#    #+#              #
#    Updated: 2021/12/09 13:40:52 by nneronin         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

RED		:= "\e[0;31m"
GREEN	:= "\e[0;32m"
YELLOW	:= "\e[0;33m"
BLUE	:= "\e[0;34m"
MAGENTA	:= "\e[0;35m"
CYAN	:= "\e[0;36m"
RESET	:= "\e[0m"

RAW =	pf_printf.c\
	   	pf_format.c\
	   	pf_formats1.c\
	   	pf_formats2.c\
	   	pf_padding.c\
	   	pf_flags.c\
	   	pf_buffer.c\
	   	pf_nbr.c\
	   	pf_str.c\
	   	pf_char.c\
	   	pf_nbr_u.c\
	   	pf_nbr_o.c\
	   	pf_nbr_x.c\
		pf_nbr_p.c\
	   	pf_percent.c\
	   	pf_float.c\
	   	pf_base.c\
	   	pf_nbr_inf.c\
	   	pf_reset.c\
		pf_specials.c\
		error_msg.c

NAME = libpf.a
CDIR = srcs
IDIR = -I.
ODIR = objs
SRCS = $(addprefix $(CDIR)/, $(RAW))
OBJS = $(addprefix $(ODIR)/, $(RAW:.c=.o))
CFLAGS = -Wall -Wextra -Werror -MMD -O3

all: $(NAME)
	@printf $(GREEN)"\e[J[INFO] $(NAME) ready!\n"$(RESET)

$(ODIR):
	@mkdir -p $(ODIR)

$(ODIR)/%.o: $(CDIR)/%.c
	@printf $(YELLOW)"\e[JCompiling $<\n\e[F"$(RESET)
	@gcc -c $< -o $@ $(CFLAGS) $(IDIR)

$(NAME): $(ODIR) $(OBJS)
	@ar rc $(NAME) $(OBJS)
	@ranlib $(NAME)

clean: 
	@printf $(CYAN)"[INFO] $(NAME) cleaned!\n"$(RESET)
	@rm -rf $(ODIR)

fclean: clean
	
	@rm $(NAME)

re: fclean all

.PHONY: fclean all clean re $(NAME)
