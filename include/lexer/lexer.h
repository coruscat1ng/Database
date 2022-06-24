#ifndef PARSER_H
#define PARSER_H

#include <list.h>

enum lexer_token_type {
	lexer_token_number_type,
	lexer_token_name_type,
	lexer_token_operator_type,
	lexer_token_separator_type,
	lexer_token_special_type
};

struct lexer_token {
	int type;
	
	const char *data;
	int len;
};

struct lexer_node {
	struct lexer_token token;
	struct binary_node node;
};

struct lexer_info {
	const char *operators;
	int operators_n;

	const char *separators;
	int separators_n;

	const char *specials;
	int specials_n;
};

void lexer_print_token(struct lexer_token *token);

struct list *lexer_scan(const struct lexer_info *lexer_info, const char *buffer, int len);

#endif
