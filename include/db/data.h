#ifndef DATABASE_TYPES_H
#define DATABASE_TYPES_H
enum tastes {
	sour = 1, 
	hot = 1 << 1, 
	sweet = 1 << 2, 
	bitter = 1 << 3, 
	salty = 1 << 4
};

struct ingridient {
	char *name;

	int calories;
	int taste;
};

struct recipe_entry {
	int weight;
	struct ingridient *ingridient;
};

struct recipe {
	int entries_number;
	struct recipe_entry *entries;
};

struct tortik {
	char *name;
	struct recipe recipe;
	
	int calories;
	int taste;
	int weight;
};

void delete_ingridient(struct ingridient *ingridient);
void delete_tortik(struct tortik *tortik);

void cpy_ingridient(struct ingridient *dest, struct ingridient *src);


#endif
