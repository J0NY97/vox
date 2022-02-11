#include "huffman.h"

// gcc huffman.c -Ilibs/libft -Llibs/libft -lft -Ilibs/liblg -Llibs/liblg -llg -Wall

t_hm_node	*new_hm_node(int symbol, int frequency, int depth)
{
	t_hm_node	*node;

	node = ft_memalloc(sizeof(t_hm_node));
	node->parent = NULL;
	node->left_child = NULL;
	node->right_child = NULL;
	node->symbol = symbol;
	node->frequency = frequency;
	node->depth = depth;
	node->code = NULL;
	return (node);
}

void	free_node(t_hm_node *node)
{
	node->parent = NULL;
	node->left_child = NULL;
	node->right_child = NULL;
	free(node);
}

int	get_hm_frequency(char *str, char *chars, int *freqs)
{
	int i = -1;
	int already_done;
	int	count;
	int	unique_amount = -1;
	while (str[++i])
	{
		already_done = 0;
		// check if 'str[i]' in 'chars'; (which means we already have counted the frequency);
		for (int j = 0; chars[j]; j++)
		{
			if (chars[j] == str[i])
			{
				already_done = 1;
				break ;
			}
		}
		// if !already_done, go through the str and count how many of that char we have;
		if (!already_done)
		{
			count = 0;
			for (int k = 0; str[k]; k++)
				if (str[k] == str[i])
					count++;
			++unique_amount;
			chars[unique_amount] = str[i];
			freqs[unique_amount] = count;
		}
	}
	return (unique_amount);	
}

void	sort_hm_frequency(char *chars, int *freqs)
{
	char	temp;

	for (int i = 0; chars[i] && freqs[i]; i++)
	{
		if (freqs[i + 1])
		{
			if (freqs[i] > freqs[i + 1])
			{
				temp = chars[i];
				chars[i] = chars[i + 1];
				chars[i + 1] = temp;

				temp = freqs[i];
				freqs[i] = freqs[i + 1];
				freqs[i + 1] = temp;

				i = 0;
			}
		}
	}
}

void	swap_hm_nodes(t_hm_node **a, t_hm_node **b)
{
	t_hm_node	*temp;

	temp = *a;
	*a = *b;
	*b = temp;
}

// Returns root node;
t_hm_node	*create_hm_tree(char *chars, int *freqs)
{
	t_hm_node	*temp;
	t_hm_node	*root;
	t_hm_node	*node;

	root = new_hm_node(0, 0, 0);
	for (int i = 0; chars[i] && freqs[i]; i++)
	{
		if (root->right_child && root->left_child)
		{
			temp = new_hm_node(0, 0, root->depth - 1);
			temp->left_child = root;
			root->parent = temp;
			swap_hm_nodes(&temp, &root);
		}
		node = new_hm_node(chars[i], freqs[i], root->depth + 1);
		node->parent = root;
		if (root->left_child == NULL)
			root->left_child = node;
		else if (root->right_child == NULL)
		{
			root->right_child = node;
			if (root->left_child->frequency > node->frequency)
				swap_hm_nodes(&root->left_child, &root->right_child);
		}
		else
			printf("[%s] Error: New root node was never created!\n", __FUNCTION__);
		if (root->left_child && root->right_child)
			root->frequency = root->left_child->frequency + root->right_child->frequency;
	}
	return (root);
}

void	print_hm_tree(t_hm_node *root, int iter)
{
	for (int i = 0; i < iter; i++)
		printf("\t");
	if (!root->left_child && !root->right_child)
		printf("Node> %d : %d : <%s>\n", root->symbol, root->frequency, root->code);
	else
		printf("Root> %d\n", root->frequency);
	if (root->left_child)
	{
		for (int i = 0; i < iter; i++)
			printf("\t");
		printf("Root Left>\n");
		print_hm_tree(root->left_child, iter + 1);
	}
	if (root->right_child)
	{
		for (int i = 0; i < iter; i++)
			printf("\t");
		printf("Root Right>\n");
		print_hm_tree(root->right_child, iter + 1);
	}
}

void	gen_hm_code(t_hm_code *code, t_hm_node *node, char c, int *n)
{
	if (c == ' ')
		node->code = ft_strdup("");
	else
	{
		node->code = ft_strdup(node->parent->code);
		ft_straddchar(&node->code, c);
	}
	if (!node->left_child && !node->right_child)
	{
		code[*n].code = ft_strdup(node->code);
		code[*n].symbol = node->symbol;
		*n += 1;
	}
	if (node->left_child)
		gen_hm_code(code, node->left_child, '0', n);
	if (node->right_child)
		gen_hm_code(code, node->right_child, '1', n);
}

void	print_hm_codes(t_hm_code *codes, int amount)
{
	for (int i = 0; i < amount; i++)
		printf("%c : %s\n", codes[i].symbol, codes[i].code);
}

/*
 * 'code' should already be allocated in either heap or stack;
*/
void	get_hm_code(char *code, t_hm_code *codes, char *str)
{
	int	total_len = 0;
	int	found;

	for (int i = 0; str[i]; i++)
	{
		found = 0;
		for (int j = 0; !found; j++)
		{
			if (codes[j].symbol == str[i])
			{
				ft_strcpy(code + total_len, codes[j].code);
				total_len += ft_strlen(codes[j].code) + 1;
				code[total_len - 1] = ' ';
				found = 1;
			}
		}
	}
}

// Returns amount of unique codes;
int	encode_hm_code(t_hm_node **tree, t_hm_code **codes, char *code, char *str)
{
	char		chars[128];
	int			freqs[128];
	t_hm_node	*root;
	t_hm_code	*code_arr;

	memset(chars, 0, 128);
	memset(freqs, 0, 128);

	int count = get_hm_frequency(str, chars, freqs);

	sort_hm_frequency(chars, freqs);

	root = create_hm_tree(chars, freqs);

	int code_amount = 0;
	code_arr = malloc(sizeof(t_hm_code) * count);
	gen_hm_code(code_arr, root, ' ', &code_amount);

	get_hm_code(code, code_arr, str);

	printf("%s\n", str);

	// Print chars and freqs;
	for (int p = 0; chars[p]; p++)
	{
		printf("%c : %d\n", chars[p], freqs[p]);
	}

	*tree = root;
	*codes = code_arr;
	return (count);
}

/*
 * 'str' is where we save the final str that has been decoded, should be allocated.
*/
void	decode_hm_code(t_hm_code *codes, char *code, char *str)
{
	char	**code_arr;
	int		found;

	code_arr = ft_strsplit(code, ' ');
	for (int i = 0; code_arr[i]; i++)
	{
		printf("Checking code : %s == \n", code_arr[i]);
		found = 0;
		for (int j = 0; !found; j++)
		{
			printf("\t%s\n", codes[j].code);
			if (ft_strequ(codes[j].code, code_arr[i]))
			{
				str[i] = codes[j].symbol;
				found = 1;
			}
		}
	}		
	ft_arraydel(code_arr);
}

/*
 * HOW:
 *	Get down to a node where the depth is 'to_add->depth - 1',
 *	and either left_child or right_child is NULL;
 *	Then add the 'to_add' in the NULL _child (prefering left);
 *	If the 'tree' isn't deep enough, we will create nodes till it is;
 * Returns 1 if found else 0;
*/
int	hm_node_add(t_hm_node *tree, t_hm_node *to_add)
{
	int	result = 0;
	// WE NEED TO GO DEEPER!
	if (tree->symbol != -1) // We are on a leaf, go back;
		return (0);
	if (tree->depth + 1 != to_add->depth)
	{
		if (tree->symbol == -1) // Are we not a leaf;
		{
			if (!tree->left_child)
			{
				tree->left_child = new_hm_node(-1, -1, tree->depth + 1);
				tree->left_child->parent = tree;
				tree->left_child->code = ft_strdup(tree->code);
				ft_straddchar(&tree->left_child->code, '0');
				result = hm_node_add(tree->left_child, to_add);
			}
			else
				result = hm_node_add(tree->left_child, to_add);
			if (result)
				return (1);
			if (!tree->right_child)
			{
				tree->right_child = new_hm_node(-1, -1, tree->depth + 1);
				tree->right_child->parent = tree;
				tree->right_child->code = ft_strdup(tree->code);
				ft_straddchar(&tree->right_child->code, '1');
				result = hm_node_add(tree->right_child, to_add);
			}
			else
				result = hm_node_add(tree->right_child, to_add);
			return (result);
		}
		else // We have taken the wrong path;
			return (0);
	}
	else // We are one depth from the desired depth;
	{
		if (!tree->left_child || !tree->right_child)
		{
			to_add->parent = tree;
			to_add->code = ft_strdup(tree->code);
			if (!tree->left_child)
			{
				tree->left_child = to_add;
				ft_straddchar(&tree->left_child->code, '0');
			}
			else if (!tree->right_child)
			{
				tree->right_child = to_add;
				ft_straddchar(&tree->right_child->code, '1');
			}
			return (1);
		}
		else // Wrong path; But if this path was deeper than the wanted depth we dont bother to traverse it;
			return (0);
	}
	return (0);
}

/*
 * Builds tree from the info we get from the jpg;
 * Code len of each symbol in symbols and how many of that code len;
 *
 * 'symbol_count' is len of 16;
 * 'symbols' is len of all values in 'symbol_count' counted together;
 */
t_hm_node	*build_hm_tree(uint8_t *symbol_count, uint8_t *symbols)
{
	t_hm_node	*tree = new_hm_node(-1, -1, 0);
	tree->code = ft_strdup("");

	int syms_done = -1; // symbols we have given their corresponding code len from 'symbols';
	for (int i = 0; i < 16; i++) // symbol count size == 16;
	{
		if (symbol_count[i] == 0) // If we have no symbols of that code len;
			continue ;
		for (int j = 0; j < symbol_count[i]; j++)
		{
			++syms_done;
			if (BEDUGGER)
				LG_INFO("Sym %d has %d code len (syms checked %d)", symbols[syms_done], i + 1, syms_done);
			t_hm_node	*new_node = new_hm_node(symbols[syms_done], -1, i + 1); // we dont care about the frequency of the symbol; code len is equal to depth;
			hm_node_add(tree, new_node);

			if (0)
				print_hm_tree(tree, 0); // Print every iteration;
		}
	}
	return (tree);
}

/*
 * !!!!IMPORTANT!!! NOTE:
 *	Dont free the 'char *'s in the 'code_arr', only free the 'code_arr';
 *
 * Generates code arr where every index corresponds with the symbols index;
 * if symbols[0] == 'a'; then code_arr[0] = "00"; which would then ofc be the
 * code gotten from the tree;
 * aka symbols[index]'s code is code_arr[index];
 * 
 * Go through 'node' and save all the codes in the right spot in the 'code_arr';
 * 'code_arr' should be allocated already; only the first dimension... if i have to say;
*/
void	get_hm_codes(t_hm_node *node, char	**code_arr, int *count)
{
	if (!node->left_child && !node->right_child) // meaning its not a branch, but a leaf;
	{
		code_arr[*count] = node->code; // TODO: this might be stupid, and i will forget; check this if freeing causes issues;
		*count += 1;
		code_arr[*count] = NULL;
		LG_DEBUG("[%s] If Segfault / Freeing issues check here\n", __FUNCTION__);
		return ;
	}
	if (node->left_child)
		get_hm_codes(node->left_child, code_arr, count);
	if (node->right_child)
		get_hm_codes(node->right_child, code_arr, count);
}

/*
 * Returns index of symbol that has code 'code';
 * Works since symbols and codes are in the same index, even though they are in
 * 2 different arrays;
 * 
 * Searches 'code' from the array of 'codes';
 */
int	get_hm_symbol_index_with_code(char *code, char **codes)
{
	if (!code || code[0] == '\0')
		return (-1);
	for (int i = 0; codes[i]; i++)
	{
		if (ft_strequ(code, codes[i]))
			return (i);
	}
	return (-1);
}

/*
 * this is basically the same as ft_strnequ, but its faster just because we
 * are not checking if s1 / s2 exists;
 */
int	equalisation(char *s1, char *s2, size_t n)
{
	size_t	i;

	i = 0;
	while (i < n)
	{
		if (s1[i] == s2[i])
		{
			i++;
			continue ;
		}
		return (0);
	}
	return (1);
}

/*
 * This version traverses the huffman tree;
 * Returns 0 if not found, otherwise 1;
 * 
 * Takes in a code of any size, traverses the tree correctly from the code,
 * 	if code ends or we encounter a leaf; we compare the leaf to the code until
 * 	we are at the same char as we have checked;
*/
int	get_hm_symbol_with_code(uint8_t *symbol, char *code, int *len, t_hm_node *tree) 
{
	int			i;

	i = -1;
	while (code[++i] && tree && !(!tree->left_child && !tree->right_child))
	{
		if (code[i] == '0')
			tree = tree->left_child;
		else if (code[i] == '1')
			tree = tree->right_child;
	}
	if (tree && !tree->left_child && !tree->right_child
		/*&& equalisation(code, tree->code, i)*/)
	{
		*symbol = tree->symbol & 0x00ff; // get last byte of uint16_t
		*len = i;
		return (1);
	}
	if (!tree)
		LG_ERROR("No tree given.");
	return (0);
}

int	get_hm_symbol_with_code_v2(uint8_t *symbol, uint16_t code, int *len, t_hm_node *tree) 
{
	int			i;

	i = 16;
	while (--i >= 0 && tree && !(!tree->left_child && !tree->right_child))
	{
		if (!((code >> i) & 1))
			tree = tree->left_child;
		else if ((code >> i) & 1)
			tree = tree->right_child;
	}
	if (tree && !tree->left_child && !tree->right_child
		/*&& equalisation(code, tree->code, i)*/)
	{
		*symbol = tree->symbol & 0x00ff; // get last byte of uint16_t
		*len = 15 - i;
		return (1);
	}
	if (!tree)
		LG_WARN("No tree given.");
	return (0);
}

void	asdf(void)
{
	char	symbol_count[16];

	symbol_count[0] = 0; // number of symbols with code len of 1;
	symbol_count[1] = 1; // number of symbols with code len of 2;
	symbol_count[2] = 5; // number of symbols with code len of 3;
	symbol_count[3] = 1; // number of symbols with code len of 4;
	symbol_count[4] = 1; // number of symbols with code len of 5;
	symbol_count[5] = 1; // number of symbols with code len of 6;
	symbol_count[6] = 1; // number of symbols with code len of 7;
	symbol_count[7] = 1; // number of symbols with code len of 8;
	symbol_count[8] = 1; // number of symbols with code len of 9;

	symbol_count[9] = 0; // number of symbols with code len of 10;
	symbol_count[10] = 0; // number of symbols with code len of 11;
	symbol_count[11] = 0; // number of symbols with code len of 12;
	symbol_count[12] = 0; // number of symbols with code len of 13;
	symbol_count[13] = 0; // number of symbols with code len of 14;
	symbol_count[14] = 0; // number of symbols with code len of 15;
	symbol_count[15] = 0; // number of symbols with code len of 16;

	int	total_sym_count = 0;
	for (int i = 0; i < 15; i++)
		total_sym_count += symbol_count[i];

	char	*symbols = malloc(total_sym_count);
	symbols[0] = 'a';
	symbols[1] = 'b';
	symbols[2] = 'c';
	symbols[3] = 'd';
	symbols[4] = 'e';
	symbols[5] = 'f';
	symbols[6] = 'g';
	symbols[7] = 'h';
	symbols[8] = 'i';
	symbols[9] = 'j';
	symbols[10] = 'k';
	symbols[11] = 'l';

	t_hm_node	*tree = build_hm_tree(symbol_count, symbols);

	// Allocation +1 because 0 in the end so that we know it has ended;
	char		**codes = malloc(sizeof(char *) * (total_sym_count + 1));
	int			count = 0;
	get_hm_codes(tree, codes, &count);

	print_hm_tree(tree, 0);
	
	for (int i = 0; i < count; i++)
		LG_DEBUG("%c : %s\n", symbols[i], codes[i]);
}

//int main(void)
int huffman_test_main(void)
{
	lg_setFdLevel(2); // stdout
	lg_setFpLevel(2); // File
	lg_openFile("huffman.log", "a+");

	/*
	t_hm_node	*tree = NULL;
	t_hm_code	*codes = NULL;
	char		code[128];
	char		str[128];

	memset(code, 0, 128);
	memset(str, 0, 128);

	int count = encode_hm_code(&tree, &codes, code, "Test Sentence");

	print_hm_tree(tree, 0);
	print_hm_codes(codes, count);

	printf("Final Code : %s\n", code);
	decode_hm_code(codes, code, str);
	printf("Decoded Str : %s\n", str);
	*/
	asdf();	
	return (0);
}