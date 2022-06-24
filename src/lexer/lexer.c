#include <lexer.h>
#include <stdlib.h>
#include <stdio.h>

static const char skip_characters[] = " ";

static inline int is_digit(char c)
{
	return c <= '9' && c >= '0';
}

static inline int is_letter(char c) 
{
	return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

int contains(char c, const char *buffer, int len)
{
	for(int i = 0; i < len; i++) {
		if(buffer[i] == c)
			return 1;
	}

	return 0;
}

const char *lexer_token_start(const char *buffer, int len)
{	
	do {
		if(!contains(buffer[0], skip_characters, sizeof(skip_characters))) 
			return buffer;
		len--;
		buffer++;

	} while(len > 0);

	return 0;
}

static inline struct lexer_node *lexer_new_node(void)
{
	return malloc(sizeof(struct lexer_node));
}

int lexer_proccess_info(const struct lexer_info *lexer_info, char c)
{

	if(lexer_info->operators &&
		contains(c, lexer_info->operators, lexer_info->operators_n))
       	{
		return lexer_token_operator_type;
	}

	if(lexer_info->separators &&
		contains(c, lexer_info->separators, lexer_info->separators_n))
       	{
		return lexer_token_separator_type;
	}

	if(lexer_info->specials && 
		contains(c, lexer_info->specials, lexer_info->specials_n)) 
	{
		return lexer_token_special_type;
	}

	return -1;
}

void lexer_init_token(const struct lexer_info *lexer_info, struct lexer_token *token)
{
	char c = token->data[0];
	int res = lexer_proccess_info(lexer_info, c);

	if(res >= 0) {
		token->type = res;
		token->len = 1;
		return;
	}

	if(is_digit(c)) {
		token->type = lexer_token_number_type;
		int i;

		for(i = 1; (i < token->len && is_digit(token->data[i])); i++) {
		}

		token->len = i;

		return;
	}

	
	int i = 0;

	do {
		i++;
		c = token->data[i];

		res = lexer_proccess_info(lexer_info, token->data[i]);

	} while(i < token->len && (is_letter(c) || is_digit(c)) && res < 0);

	token->len = i;
	token->type = lexer_token_name_type;
}

struct list *lexer_scan(const struct lexer_info *lexer_info, const char *buffer, int len)
{
	const char *data = lexer_token_start(buffer, len);
	
	if(!data)
		return 0;

	struct list *list = list_new(offsetof(struct lexer_node, node));

	do {

		len -= (data - buffer);

		struct lexer_node *lexer_node = lexer_new_node();

		lexer_node->token.data = data;
		lexer_node->token.len = len;

		lexer_init_token(lexer_info, &lexer_node->token);

		len -= lexer_node->token.len;
		buffer = data + lexer_node->token.len;

		list_append(list, list_right, &lexer_node->token);

		if(len <= 0)
			break;

		data = lexer_token_start(buffer, len);

	} while(data);

	return list;
}


void lexer_print_token(struct lexer_token *token)
{
	static const char *token_type_names[] = {
		[lexer_token_number_type] = "number",
		[lexer_token_name_type] = "name",
		[lexer_token_operator_type] = "operator",
		[lexer_token_separator_type] = "separator",
		[lexer_token_special_type] = "special"
	};

	printf("type: %s, data: %.*s", token_type_names[token->type], token->len, token->data);
}

