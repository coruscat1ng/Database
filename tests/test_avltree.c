#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <avl_tree.h>

int compare(const void *left, const void *right);
void print(void *tortik);
void print2(void *tortik);

int check_balances(struct avl_node *node);
void test(struct avl_tree *tree, void (*callback)(struct avl_tree *, void *));

void test_insert(struct avl_tree *tree, void *tasty);
void test_remove(struct avl_tree *tree, void *tasty);
void test_destroy(void *tasty);
		
char *names[] = {
	"Croco", "Panda",
	"Choco", "Dogo",
	"Moca", "Tacocat",
	"Evrika", "Apple",
	"Tasty", "Muffin",
	"Some", "Beuty",
	"Chilly", "Red",
	"Leaves", "With Sugar",
	"No", "Milk",
	"Ice", "Cold",
	"Way more", "Smooth",
	"And also", "Now",
	"Another", "Dimension"

};

struct Tortik {
	char *name;
	int calories;
	struct avl_node name_tree;
};

int success = 0;

int main()
{
	struct Tortik *tasty, *temp; 
	struct avl_tree Tortiks_tree;

	avl_init(&Tortiks_tree, 
		&compare, 
		offsetof(struct Tortik, name_tree));
	
	test(&Tortiks_tree, test_insert);
	test(&Tortiks_tree, test_remove);
	test(&Tortiks_tree, test_insert);

	printf("\nTree height: %d\n\n", check_balances(Tortiks_tree.root));	

	char u[] = "Red Leaves";
	char l[] = "Evrika Cold"; 
	
	printf("Testing Range: [%s -- %s]\n\n", l, u);
	
	struct Tortik upper, lower;
	
	upper.name = u;
	lower.name = l;
	
	avl_range(&Tortiks_tree, &lower, &upper, print2);
	
	printf("\nTesting left Ray: (inf -- %s]\n\n", u);
	
	avl_left_ray(&Tortiks_tree, &upper, print2);
	
	
	printf("\nTesting right Ray: [%s -- inf)\n\n", u);
	
	avl_right_ray(&Tortiks_tree, &upper, print2);
	
	avl_preorder(&Tortiks_tree, print2);
	
	avl_destroy(&Tortiks_tree, &test_destroy);

	return 0;
	
}

void test_insert(struct avl_tree *tree, void *data)
{

	static int success = 0;
	struct Tortik *tasty = data;
	
	if(!avl_insert(tree, tasty)) {
		success++;	
		printf("inserts's number %d, name : %s\n", success, tasty->name);
		check_balances(tree->root);	
	}

}
void test_remove(struct avl_tree *tree, void *data)
{
	static int success = 0;
	static struct Tortik *temp;
	struct Tortik *tasty = data;
	
	if(temp = avl_remove(tree, tasty)) {
		success++;	
		printf("removes's number %d, name : %s\n", success, tasty->name);
		check_balances(tree->root);	
	
		free(tasty->name);
		free(temp->name);
		free(temp);
	}

}
void test_destroy(void *data)
{
	static int success = 0;
	struct Tortik *tasty = data;
	success++;	
	printf("destroys's number %d, name : %s\n", success, tasty->name);
	free(tasty->name);
	free(tasty);
}

void test(struct avl_tree *tree, void (*callback)(struct avl_tree *, void *))
{
	const size_t size = sizeof(names)/sizeof(char *);
	size_t len1, len2;
	static struct Tortik *tasty;
	
	for(int j = size - 1; j >=0; j--) {
	for(int i = size - 1; i >= 0; i--) {

		len1 = strlen(names[i % size]);
		len2 = strlen(names[j % size]);
		
		tasty = malloc(sizeof(struct Tortik));
		tasty->name = malloc(len1 + len2 + 2);

		if(tasty->name == 0) {
			perror("Allocation failed");
			return;
		}

		memcpy(tasty->name, names[i % size], len1);
		memcpy(tasty->name + len1 + 1, names[j % size], len2);
		tasty->name[len1] = ' ';
		tasty->name[len1+len2+1]='\0';
		tasty->calories = rand() % 666;

		callback(tree, tasty);
		
	}
	}
	
	printf("\nNodes Number: %d\n", tree->nodes_number);
}



int check_balances(struct avl_node *node)
{

	if(!node)
		return 0;
	
	int r, l;
		
	r = check_balances((struct avl_node *) node->node.children[binary_right]);
	
	l = check_balances((struct avl_node *) node->node.children[binary_left]);
	
	struct Tortik *temp = (((char *)node) - offsetof(struct Tortik, name_tree));	
		
	if(node->balance != (l - r)) {
		printf("disbalanced: %s \t\t %p b: %d true b: %d\n", 
		temp->name, node, node->balance, (l - r));
		
		success = -1;
	}
	
	return (r > l ? r : l) + 1;	 
}

void print(void *tortik)
{
	if(abs(((struct Tortik *)tortik)->name_tree.balance) > 1) {
		printf("%20s Adress: %p \t Balance: %d\n",
		((struct Tortik *)tortik)->name, 
		tortik, 
		((struct Tortik *)tortik)->name_tree.balance);
	}
}
void print2(void *tortik) 
{
	
	printf("%-20s \t Adress: %p \t Balance: %2d\n",
		((struct Tortik *)tortik)->name, 
		tortik, 
		((struct Tortik *)tortik)->name_tree.balance);
}


int compare(const void *left, const void *right)
{
	return strcmp(((struct Tortik *) left)->name, ((struct Tortik *) right)->name);
}
