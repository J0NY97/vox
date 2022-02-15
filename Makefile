rwildcard = $(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

NAME = sp #shaderpixel
CFILES = \
	main.c \
	entity.c \
	model.c \
	shader.c \
	texture.c \
	camera.c \
	fractal.c \
	player.c \
	fps.c \
	key.c \
	bimgf/bimgf_bmp.c \
	bimgf/bimgf_help.c \
	bimgf/bimgf_jpg.c \
	bimgf/bimgf.c \
	bimgf/huffman.c \
	bmath/bmath.c \
	bobj/bobj_loader.c \
	glad/glad.c \

CDIR = srcs
ODIR = obj
#SRCS = $(call rwildcard, $(CDIR), *.c)
#OBJ = $(patsubst $(CDIR)/%.c, $(ODIR)/%.o, $(SRCS))
OBJ = $(addprefix $(ODIR)/, $(CFILES:.c=.o))
JOHNNY = $(OBJ:.o=.d)


LIB_DIR = ./libs

INCS = \
	-I$(LIB_DIR)/libft \
	-I$(LIB_DIR)/libpf \
	-I$(LIB_DIR)/liblg \
	-I$(LIB_DIR)/GLFW \
	-I$(CDIR)/ \
	-I$(CDIR)/bobj \
	-I$(CDIR)/bmath \
	-I$(CDIR)/bimgf \
	-I$(CDIR)/glad \
	-I$(CDIR)/KHR \

LIB_DIRS = \
	-L$(LIB_DIR)/libft \
	-L$(LIB_DIR)/libpf \
	-L$(LIB_DIR)/liblg \
	-L$(LIB_DIR)/GLFW \

LIBS = -lpf -lft -llg -lglfw3 -lOpenGl32 -lgdi32 -lpthread

FLAGS = -Wall -Wextra -Wno-unused-variable

all: $(ODIR) $(NAME)

-include $(JOHNNY)

$(ODIR):
	@mkdir -p $@
	@mkdir -p $@/bimgf
	@mkdir -p $@/bmath
	@mkdir -p $@/bobj
	@mkdir -p $@/glad

$(ODIR)/%.o: $(CDIR)/%.c
	@gcc -c $< -o $@ -MMD -O3 $(INCS) $(FLAGS)
	@printf $(YELLOW)"Compiling $<\n"$(RESET)

$(NAME): $(OBJ)
	@gcc -o $(NAME) $(OBJ) $(INCS) $(LIB_DIRS) $(LIBS) $(FLAGS)
	@echo "$(NAME) was successfully created."

clean:
	@rm -rf $(ODIR)
	@echo "$(NAME) was cleaned."

fclean: clean
	@rm -f $(NAME)
	@echo "$(NAME) was fcleaned."

re: fclean $(LIBS) all

.PHONY: clean, all, re, fclean
