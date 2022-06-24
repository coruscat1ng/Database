#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <avl_tree.h>
#include <dbgen.h>

#include "core.h"
static const struct ingridient ingridients[] = {
	{.name = "chocolate", .taste = sweet, .calories = 123},
	{.name = "ice", .taste = 0, .calories = 0},
	{.name = "milk", .taste = sweet, .calories = 23},
	{.name = "pepper", .taste = hot, .calories = 11},
	{.name = "sugar", .taste = sweet, .calories = 72},
	{.name = "butter", .taste = bitter, .calories = 65},
	{.name = "lemon", .taste = sour, .calories = 26},
	{.name = "salt", .taste = salty, .calories = 3},
	{.name = "cofee", .taste = bitter, .calories = 10},
};
static const size_t ingridients_size = sizeof(ingridients) / sizeof(struct ingridient);
static const char *properties[] = {
	"Choco", "Coco", "Moko", "Sweet", "Sour", "Crazy", 
	"Soft", "Red", "Bloody",  "Tasty", "Not tasty", "Cold", 
	"Cream", "Cheese", "Meat", "Fat", "Crumpy", "Hot"
};
static const char *names[] = {
	"Barhat", "Pragha", "Marry", "Cake", "Tortik", "Tortella", 
	"Tort", "Cat", "Moon", "Sun", "Sept", "Star",  "Light",
	
	"Choco", "Coco", "Moko", "Sweet", "Sour", "Crazy", 
	"Soft", "Red", "Bloody",  "Tasty", "Not tasty", "Cold", 
	"Cream", "Cheese", "Meat", "Fat", "Crumpy", "Hot"
};
static const size_t name_size = sizeof(names) / sizeof(char *);
static const size_t property_size = sizeof(properties) / sizeof(char *);

void random_fill(size_t *array, size_t length, size_t max_value);

void generate_recipe(struct db *db, struct recipe *recipe);

struct ingridient *random_ingridient(struct db *db)
{
	struct avl_tree *tree = &(((struct storage *) db->storage)
		       ->ingridients_name_tree);
	return avl_pathfind(tree, rand(), rand());	
}

char *new_tortik_name(void) {
	static size_t n = 0, p = 0;
	static char sufix = '0';

	size_t n_len = strlen(names[n]);
	size_t p_len = strlen(properties[p]);
			
	char *name = malloc(1 + p_len + 1 +n_len + 1);

	name[p_len + 1 + n_len] = sufix;
	name[p_len + 1 + n_len + 1] = '\0';
	name[p_len] = ' ';

	memcpy(name, properties[p], p_len);
	memcpy(name + p_len + 1, names[n], n_len);

	if(++p == property_size) {
		p = 0;
		if(++n == name_size) {
			n = 0;
			sufix++;
		}	
	}

	return name;
};
char *new_ingridient_name()
{
	return 0;	
};

void generate_db(struct db *db, int size)
{
	struct ingridient_node *nodes[ingridients_size];

	for(int i = 0; i < ingridients_size; i++) {
		nodes[i] = malloc(sizeof(struct ingridient_node));
		cpy_ingridient(&nodes[i]->ingridient, ingridients +i);
		insert_ingridient_node(db, nodes[i]);
	}
	#define MAX 6
	#define MIN 3

	size_t indicies[MAX];

	for(int i = 0; i < size; i++) {
		int len = MIN + (rand() % (MAX - MIN));
		random_fill(indicies, len, ingridients_size - 1);

		struct tortik_node *node = malloc(sizeof(struct tortik_node));
		node->tortik.name = new_tortik_name();
		node->tortik.recipe.entries_number = len;
		node->tortik.recipe.entries = malloc(sizeof(struct recipe_entry) * len);
		
	
		for(int j = 0; j < len; j++) {
			node->tortik.recipe.entries[j].weight = 10 + rand() % 390;
			node->tortik.recipe.entries[j].ingridient = 
				&(nodes[indicies[j]]->ingridient);
		}

		insert_tortik_node(db, node);
	}
}

void generate_ingridient(struct db *db, struct ingridient *ingridient)
{
	static size_t i = 0;
	cpy_ingridient(ingridient, ingridient + i);
	i == (ingridients_size - 1) ? i = 0 : i++;
}


void random_fill(size_t *array, size_t length, size_t max_value)
{
	array[0] = rand() % (max_value + 1);
	
	for(size_t i = 1; i < length; i++) {
		array[i] = rand() % max_value;

		size_t offset, elements_less = 0;
		
		do {
			offset = elements_less;
			elements_less = 0;

			size_t temp = array[i] + offset;

			for(size_t j = 0; j < i; j++) {
				if(array[j] <= temp)
					elements_less++;
			}

		} while(elements_less != offset);

		array[i] += offset;

		max_value--;
	}

}
