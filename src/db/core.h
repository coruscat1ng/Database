#ifndef DATABASE_CORE_H
#define DATABASE_CORE_H
#include <avl_tree.h>
#include <db.h>
#include <data.h> 

struct storage {
	struct avl_tree tortiks_name_tree;
	struct avl_tree ingridients_name_tree;
};

struct ingridient_node {
        struct ingridient ingridient;
        struct avl_node name_node;
};
  
struct tortik_node {
        struct tortik tortik;
        struct avl_node name_node;
};

int load_ingridients(struct db *db, char *filename);
int load_tortiks(struct db *db, char *filename);

int save_ingridients(struct db *db, char *filename);
int save_tortiks(struct db *db, char *filename);

void *new_storage(void);
void init_storage(struct storage *storage);
void clear_storage(struct storage *storage);

int name_compare(const void *l, const void *r);

void process_tortik(struct db *db, struct tortik *tortik);

struct tortik *insert_tortik_node(struct db *db, struct tortik_node *node);
struct ingridient *insert_ingridient_node(struct db *db, struct ingridient_node *node);

#endif
