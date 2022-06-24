#ifndef DB_GENERATION_H
#define DB_GENERATION_H
#include "db.h"

void generate_db(struct db *db, int size);

char *new_tortik_name(void);
char *new_ingridient_name(void);

void generate_recipe(struct db *db, struct recipe *recipe);
void generate_ingridient(struct db *db, struct ingridient *ingridient);

struct ingridient *random_ingridient(struct db *db);

#endif
