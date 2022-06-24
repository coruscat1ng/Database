#ifndef DATABASE_H
#define DATABASE_H

#include "data.h"

void i_print(void *data);
void t_print(void *data);
void print_taste(int taste);

struct db {
	int tortiks_number;
	int ingridients_number;

	void *storage;
};

void init_db(struct db *db);

int open_db(struct db *db, const char *name);
int save_db(struct db *db, const char *name);

void close_db(struct db *db);

struct tortik *name_find_tortik(struct db *db, char *name);
struct ingridient *name_find_ingridient(struct db *db, char *name);

struct tortik *insert_tortik(struct db *db, struct tortik *tortik);
struct ingridient *insert_ingridient(struct db *db, struct ingridient *ingridient);

struct tortik *remove_tortik(struct db *db, char *name);
struct ingridient *remove_ingridient(struct db *db, char *name);

void name_range_tortiks(struct db *db, 
	const char *l, 
	const char *u, 
	void (*callback)(void *));
void name_range_ingridients(struct db *db, 
	const char *l, 
	const char *u, 
	void (*callback)(void *));
#endif
