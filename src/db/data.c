#include <data.h>
#include <stdlib.h>
#include <string.h>

void delete_ingridient(struct ingridient *ingridient)
{
	free(ingridient->name);
}

void delete_tortik(struct tortik *tortik)
{
	free(tortik->recipe.entries);
	free(tortik->name);
}

void cpy_ingridient(struct ingridient *dest, struct ingridient *src)
{
	int len = strlen(src->name) + 1;

	dest->name = malloc(len);
	memcpy(dest->name, src->name, len);
	
	dest->calories = src->calories;
	dest->taste = src->taste;
}
