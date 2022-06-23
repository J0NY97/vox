# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: jsalmi <jsalmi@student.hive.fi>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/05/17 13:09:50 by jsalmi            #+#    #+#              #
#    Updated: 2022/06/23 16:21:16 by jsalmi           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

SHELL_NAME = $(shell uname -s)
RAW =	\
	base/ftsystem.c \
	base/ftinit.c \
	base/ftdebug.c \
	base/ftbase.c \
	base/ftbbox.c \
	base/ftglyph.c \
	\
	base/ftbitmap.c \
	\
	sfnt/sfnt.c \
	psnames/psnames.c \
	truetype/truetype.c \
	raster/raster.c \

ifeq ($(SHELL_NAME), Darwin)
	RAW += base/ftmac.c
endif

NAME = libfreetype.a
CDIR = src
IDIR = -Iinclude
ODIR = obj
SRCS = $(addprefix $(CDIR)/, $(RAW))
OBJS = $(addprefix $(ODIR)/, $(RAW:.c=.o))
CFLAGS = -Wall -Wextra -MMD -O3 #-Werror
BUILD_FLAGS = -DFT2_BUILD_LIBRARY

ifeq ($(SHELL_NAME), Darwin)
BUILD_FLAGS += -mmacosx-version-min=10.4
endif

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
	@gcc $(BUILD_FLAGS) -c $< -o $@ $(CFLAGS) $(IDIR)

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
