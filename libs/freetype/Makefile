# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: jsalmi <jsalmi@student.hive.fi>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/05/17 13:09:50 by jsalmi            #+#    #+#              #
#    Updated: 2022/05/17 13:23:12 by jsalmi           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

RAW =	\
	base/ftsystem.c \
	base/ftinit.c \
	base/ftdebug.c \
	base/ftbase.c \
	base/ftbbox.c \
	base/ftglyph.c \
	\
	sfnt/sfnt.c \
	psnames/psnames.c \
	truetype/truetype.c \
	raster/raster.c \

NAME = libfreetype.a
CDIR = src
IDIR = -Iinclude
ODIR = obj
SRCS = $(addprefix $(CDIR)/, $(RAW))
OBJS = $(addprefix $(ODIR)/, $(RAW:.c=.o))
CFLAGS = -Wall -Wextra -MMD -O3 #-Werror

all: $(NAME)
	@printf $(GREEN)"\e[J[INFO] $(NAME) ready!\n"$(RESET)

$(ODIR):
	@mkdir -p $(ODIR)/base/
	@mkdir -p $(ODIR)/sfnt/
	@mkdir -p $(ODIR)/psnames/
	@mkdir -p $(ODIR)/truetype/
	@mkdir -p $(ODIR)/raster/

$(ODIR)/%.o: $(CDIR)/%.c
	@printf $(YELLOW)"\e[JCompiling $<\n\e[F"$(RESET)
	@gcc -DFT2_BUILD_LIBRARY -c $< -o $@ $(CFLAGS) $(IDIR)

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
