#include <lexer.h>
#include <stdio.h>

static const char operators[] = "<=-+>|&";
static const int operators_n = sizeof(operators) - 1;

static const char separators[] = ",{}()[]";
static const int separators_n = sizeof(separators) - 1;

static const char specials[] = "\\!_";
static const int specials_n = sizeof(specials) - 1;


static const char buffer[] = "name=a123 \\test {subject + 589, rating < 34}";
static const int len = sizeof(buffer) - 1;

void print_token(struct lexer_token *token)
{
	static int i = 1;
	printf("token #%d ", i);
	lexer_print_token(token);
	printf("\n");

	i++;
}

int main() {
	struct lexer_info lexer_info = {
		.operators = operators, 
		.operators_n = operators_n,
		.specials = specials,
		.specials_n = specials_n,
		.separators = separators,
		.separators_n = separators_n
	};

	struct list *list = lexer_scan(&lexer_info, buffer, len);

	list_traverse(list, list_right, (void (*)(void *)) print_token);	

	return 0;
}
