#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//#include <serializing.h>

#include "core.h"

void *new_storage(void)
{
	return malloc(sizeof(struct storage));
}
void init_storage(struct storage *storage)
{
	avl_init(&(storage->tortiks_name_tree), name_compare, 
			offsetof(struct tortik_node, name_node));
	avl_init(&(storage->ingridients_name_tree), name_compare, 
			offsetof(struct ingridient_node, name_node));
};

int name_compare(const void *l, const void *r)
{
	const struct tortik *left = l, *right = r;

	return strcmp(left->name, right->name);  
}

void process_tortik(struct db *db, struct tortik *tortik)
{
	struct recipe_entry *entries = tortik->recipe.entries;
	int size = tortik->recipe.entries_number;
	
	tortik->weight = 0;
	tortik->calories = 0;
	tortik->taste = 0;

	for(int i = 0; i < size; i++) {
		tortik->weight += entries[i].weight;
		tortik->taste |= entries[i].ingridient->taste;
		tortik->calories += entries[i].ingridient->calories * entries[i].weight;
	}

	if(tortik->weight)
		tortik->calories /= tortik->weight;
}

struct tortik *insert_tortik_node(struct db *db, struct tortik_node *node)
{
	process_tortik(db, &(node->tortik));
	struct avl_tree *tree = &(((struct storage *) db->storage)
		->tortiks_name_tree);
	return avl_insert(tree, node);
};
struct ingridient *insert_ingridient_node(struct db *db, struct ingridient_node *node)
{

	struct avl_tree *tree = &(((struct storage *) (db->storage))
			->ingridients_name_tree);
	return avl_insert(tree, node);
}

int load_ingridients(struct db *db, char *filename)
{
	return 0;
};
int load_tortiks(struct db *db, char *filename)
{
	return 0;
};

int save_ingridients(struct db *db, char *filename)
{
	return 0;
};
int save_tortiks(struct db *db, char *filename)
{
	return 0;
};

void clear_storage(struct storage *storage) {

}
