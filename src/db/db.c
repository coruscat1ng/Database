#include <string.h>
#include <stdio.h>
#include <db.h>
#include <stdlib.h>

#include "core.h"

#define EXT ".db"

static const int ext_len = strlen(EXT);
static const int pre_len = 1;

void print_taste(int taste)
{
	if(!taste) {
		printf("no taste");
		return;
	}

	static const char delimeter[] = ", ";
	const int len = sizeof(delimeter) - 1;

	if(taste & sour) printf("sour%s", delimeter);
	if(taste & bitter) printf("bitter%s", delimeter);
	if(taste & salty) printf("salty%s", delimeter);
	if(taste & hot) printf("hot%s", delimeter);
	if(taste & sweet) printf("sweet%s", delimeter);

	for(int i = 0; i < len; i++)
		putchar('\b');
}

void _i_print(void *ingridient) {
	struct ingridient *i = ingridient;
	printf("%s ", i->name);
	putchar('('); print_taste(i->taste); putchar(')');

	printf(" cal: %d", i->calories);
}

void i_print(void *ingridient) {
	_i_print(ingridient);
	putchar('\n');
}



void t_print(void *tortik) {
	struct tortik *t = tortik;
	printf("%s ", t->name);
	putchar('('); print_taste(t->taste); putchar(')');
	printf(" cal: %d, weight %d\n", t->calories, t->weight);

	for(int i = 0; i < t->recipe.entries_number; i++) {
		printf("* ");
		_i_print(t->recipe.entries[i].ingridient);
		printf(", weight: %3d\n", t->recipe.entries[i].weight);
	}
}


void init_db(struct db *db)
{
	
	db->storage = new_storage();

	init_storage(db->storage);

	db->tortiks_number = 0;
	db->ingridients_number = 0;
}
int open_db(struct db* db, const char *name) 
{
	size_t name_len = strlen(name);

	char *filename = malloc(pre_len + name_len + ext_len + 1);
	
	memcpy(filename + pre_len, name, name_len);
	memcpy(filename + pre_len + name_len, EXT, ext_len);
	
	filename[pre_len + ext_len + name_len] = '\0';

	if(load_ingridients(db, filename) < 0) {
		perror("no such ingridients file");
		return -1;
	}

	if(load_tortiks(db, filename)) {
		perror("no such tortiks file");
		return -1;
	}
	
	free(filename);

	return 0;
}

int save_db(struct db *db, const char *name) {

	size_t name_len = strlen(name);

	char *filename = malloc(pre_len + name_len + ext_len + 1);
	
	memcpy(filename + pre_len, name, name_len);
	memcpy(filename + pre_len + name_len, EXT, ext_len);
	
	filename[pre_len + ext_len + name_len] = '\0';

	if(save_ingridients(db, filename) < 0) {
		perror("couldn't save ingridients");
		return -1;
	}

	if(save_tortiks(db, filename)) {
		perror("couldn't save tortiks");
		return -1;
	}
	
	free(filename);

	return 0;
};

void close_db(struct db *db) {
	clear_storage(db->storage);
};

void name_range_tortiks(struct db *db, 
	const char *l, 
	const char *u, 
	void (*callback)(void *))
{
	short ul = !l + ((!u) << 1); 

	struct avl_tree *tree = &(((struct storage *) (db->storage))
			->tortiks_name_tree);
           
	switch(ul) { 
	case 0: 
		avl_range(tree, &l , &u, callback); 
		break; 
	case 1: 
		avl_left_ray(tree, &u, callback); 
		break; 
	case 2: 
		avl_right_ray(tree, &l, callback); 
		break; 
	default: 
		avl_inorder(tree, callback); 

	} 
}; 


void name_range_ingridients(struct db *db, 
	const char *l, 
	const char *u, 
	void (*callback)(void *))
{
	short ul = !l + (!u << 1); 

	struct avl_tree *tree = &(((struct storage *) (db->storage))
			->ingridients_name_tree);
           
	switch(ul) { 
	case 0: 
		avl_range(tree, &l , &u, callback); 
		break; 
	case 1: 
		avl_left_ray(tree, &u, callback); 
		break; 
	case 2: 
		avl_right_ray(tree, &l, callback); 
		break; 
	default: 
		avl_inorder(tree, callback); 

	} 
};

struct tortik *name_find_tortik(struct db *db, char *name)
{
	struct avl_tree *tree = &(((struct storage *) db->storage)
			->tortiks_name_tree);
         
	return avl_find(tree, &name);
};
struct ingridient *name_find_ingridient(struct db *db, char *name)
{
	struct avl_tree *tree = &(((struct storage *) db->storage)
			->ingridients_name_tree);
         
	return avl_find(tree, &name);

}

struct tortik *insert_tortik(struct db *db, struct tortik *tortik)
{
	struct tortik_node *node = malloc(sizeof(struct tortik_node));
	
	int len = strlen(tortik->name);
	node->tortik.name = malloc(len + 1);
	memcpy(node->tortik.name, tortik->name, len + 1);
	memcpy(&(node->tortik.recipe), &(tortik->recipe), sizeof(struct recipe));
	
	return insert_tortik_node(db, node);
};

struct ingridient *insert_ingridient(struct db *db, struct ingridient *ingridient)
{
	struct ingridient_node *node = malloc(sizeof(struct ingridient_node));

	int len = strlen(ingridient->name);
	node->ingridient.name = malloc(len + 1);
	memcpy(node->ingridient.name, ingridient->name, len + 1);

	node->ingridient.taste = ingridient->taste;
	node->ingridient.calories = ingridient->calories;
	
	return insert_ingridient_node(db, node);
};

struct tortik *remove_tortik(struct db *db, char *name)
{
	struct avl_tree *tree = &(((struct storage *) db->storage)
			->tortiks_name_tree);
	return avl_remove(tree, &name);
};
struct ingridient *remove_ingridient(struct db *db, char *name)
{
	struct avl_tree *tree = &(((struct storage *) db->storage)
			->ingridients_name_tree);
	return avl_remove(tree, &name);
};

