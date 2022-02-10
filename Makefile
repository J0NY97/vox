NAME = scop
SRCS = main.c \
	bimgf/*.c \
	bmath/*.c \
	bobj/*.c \
	glad/*.c \

CDIR = srcs 
ODIR = objs
OBJS = $(addprefix $(ODIR)/, $(SRCS:.c=.o))
JOHNNY = $(OBJS:.o=.d)

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

$(NAME): $(OBJS)
	@gcc -o $(NAME) $(OBJS) $(INCS) $(LIB_DIRS) $(LIBS) $(FLAGS)
	@echo "$(NAME) was successfully created."

$(ODIR)/%.o: $(CDIR)/%.c
	@gcc -c $< -o $@ -MMD -O3 \
		$(INCS) $(LIB_DIRS) $(LIBS) $(FLAGS)
	@printf $(YELLOW)"Compiling $<\n"$(RESET)

clean:
	@rm -rf $(ODIR)
	@echo "$(NAME) was cleaned."

fclean: clean
	@rm -f $(NAME)
	@echo "$(NAME) was fcleaned."

re: fclean $(LIBS) all

.PHONY: clean, all, re, fclean
