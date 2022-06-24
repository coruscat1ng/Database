#include <db.h>
#include <dbgen.h>
#include <stdio.h>
int main()
{
	struct db db;

	printf("init\n");
	init_db(&db);

	printf("generate\n");
	generate_db(&db, 10000);

	name_range_tortiks(&db, "Astra", "Getta", t_print);
	name_range_ingridients(&db, "cofee", "milk", i_print);
	
	return 0;
}
