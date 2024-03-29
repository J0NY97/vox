rwildcard = $(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))
SHELL_NAME = $(shell uname -s)

NAME = a
CFILES = \
	main.c \
	world_generation.c \
	scene.c \
	entity.c \
	entity_manager.c \
	model.c \
	shader.c \
	texture.c \
	camera.c \
	fractal.c \
	player.c \
	fps.c \
	key.c \
	collision.c \
	aabb.c \
	triangle_collision.c \
	plane_collision.c \
	line_collision.c \
	crosshair.c \
	skybox.c \
	instance.c \
	frustum.c \
	thread.c \
	help.c \
	hashtable.c \
	item.c \
	tnt.c \
	bitmap.c \
	save.c \
	ai_entity.c \
	bimgf/bimgf_bmp.c \
	bimgf/bimgf_help.c \
	bimgf/bimgf_jpg.c \
	bimgf/bimgf.c \
	bimgf/huffman.c \
	bmath/bmathf.c \
	bmath/bmathi.c \
	bobj/bobj_loader.c \
	bobj/bobj_v2/bobj.c \
	bobj/bobj_print.c \
	glad/glad.c \
	noise/simplex.c \
	noise/perlin.c \
	noise/noise.c \
	noise/fastnoise.c \
	ui.c \
	ui_manager.c \
#	font_manager.c \

CDIR = srcs
ODIR = obj
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
	-I$(CDIR)/bobj/bobj_v2 \
	-I$(CDIR)/bmath \
	-I$(CDIR)/bimgf \
	-I$(CDIR)/noise \
	-I$(CDIR)/glad \
	-I$(CDIR)/KHR \
	#-I$(LIB_DIR)/freetype/include \

LIB_DIRS = \
	-L$(LIB_DIR)/libft \
	-L$(LIB_DIR)/libpf \
	-L$(LIB_DIR)/liblg \
	-L$(LIB_DIR)/GLFW \
	-L$(LIB_DIR)/freetype \


LIBS = -lpf -lft -llg -lglfw3 -lpthread -lfreetype
ifeq ($(SHELL_NAME), Darwin)
LIBS += -framework OpenGL -framework Cocoa -framework IOKit
else
LIBS += -lOpenGl32 -lgdi32
endif

FLAGS = -Wall -Wextra -Wno-unused-variable -MMD -g #-O3

all: $(ODIR) $(NAME)

-include $(JOHNNY)

$(ODIR):
	@mkdir -p $@
	@mkdir -p $@/bimgf
	@mkdir -p $@/bmath
	@mkdir -p $@/bobj
	@mkdir -p $@/bobj/bobj_v2
	@mkdir -p $@/glad
	@mkdir -p $@/noise

$(ODIR)/%.o: $(CDIR)/%.c
	@gcc -c $< -o $@ $(INCS) $(FLAGS)
	@printf $(YELLOW)"Compiling $<\n"$(RESET)

$(NAME): $(OBJ)
	@gcc -o $(NAME) $(OBJ) $(INCS) $(LIB_DIRS) $(LIBS)
	@echo "$(NAME) was successfully created."

clean:
	@rm -rf $(ODIR)
	@echo "$(NAME) was cleaned."

fclean: clean
	@rm -f $(NAME)
	@echo "$(NAME) was fcleaned."

re: fclean $(LIBS) all

.PHONY: clean, all, re, fclean
