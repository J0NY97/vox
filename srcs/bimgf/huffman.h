#ifndef HUFFMAN_H
# define HUFFMAN_H
# include "libft.h"
# include "liblg.h"
# include "stdio.h"
# include "stdint.h"
# include "bimgf.h"

typedef struct s_hm_node
{
	struct s_hm_node	*parent;
	struct s_hm_node	*left_child;
	struct s_hm_node	*right_child;
	int16_t				symbol;
	int					frequency;
	int					depth;
	char				*code;
}	t_hm_node;

typedef struct s_hm_code
{
	char	symbol;
	char	*code;
}	t_hm_code;

t_hm_node	*build_hm_tree(uint8_t *symbol_count, uint8_t *symbols);
void		get_hm_codes(t_hm_node *node, char	**code_arr, int *count);
int			get_hm_symbol_index_with_code(char *code, char **codes);
int			get_hm_symbol_with_code(uint8_t *symbol, char *code, int *len, t_hm_node *tree);
int			get_hm_symbol_with_code_v2(uint8_t *symbol, uint16_t code, int *len, t_hm_node *tree);
void		print_hm_tree(t_hm_node *root, int iter);

#endif