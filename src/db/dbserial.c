#include <dbserial.h>
#include <string.h>
#include <stdlib.h>
#include <serializing.h>

static const int t_base = 3, i_base = 3;

void set_tortik_sizes(struct serial_package *package,  const struct tortik *tortik)
{

	int r_base = tortik->recipe.entries_number;
	package->var_arrays[r_base].data_size = strlen(tortik->name) + 1;
	for(int i = 0; i < r_base; i++) {
		package->var_arrays[i].data_size = 
			strlen(tortik->recipe.entries[i].ingridient->name) + 1;
	}
}

void set_ingridient_sizes(struct serial_package *package, struct ingridient *ingridient)
{
	package->var_arrays[0].data_size = strlen(ingridient->name) + 1;
}

void ingridient_package(struct serial_package *package, struct ingridient *ingridient)
{
	package->var_arrays_number = 1;
	package->const_arrays_number = 0;
	package->base_entries_number = 2;
	package->header_entries_number = 0;
	
	package->base_entries = malloc(package->base_entries_number
		       	* sizeof(struct serial_base));
	package->base_entries[0].data = &(ingridient->calories);
	package->base_entries[0].type = serial_int;
	
	package->base_entries[1].data = &(ingridient->taste);
	package->base_entries[1].type = serial_int;
	
	package->var_arrays = malloc(package->var_arrays_number
		       	* sizeof(struct serial_array));
	
	package->var_arrays[0].type = serial_char;
	package->var_arrays[0].data = ingridient->name;
}

void header_tortik_package(struct serial_package *package, int *r_base)
{

	package->var_arrays_number = 0;
	package->const_arrays_number = 0;
	package->base_entries_number = 0;
	package->header_entries_number = 1;	
	
	package->header_entries = malloc(sizeof(struct serial_base) 
		* package->header_entries_number);  

	package->header_entries[0].data = r_base;
	package->header_entries[0].type = serial_int;
}

void bind_tortik_package(struct serial_package *package, struct tortik *tortik)
{
	int r_base = tortik->recipe.entries_number;
	
	package->base_entries[0].data = &(tortik->calories);
	package->base_entries[1].data = &(tortik->weight);
	package->base_entries[2].data = &(tortik->taste);
	
	package->var_arrays[r_base].data = tortik->name;
	
	for(int i = 0; i < r_base; i++) {
		int j = t_base + i_base * i;
		
		package->base_entries[j].data = &(tortik->recipe.entries[i].weight);
		package->base_entries[j + 1].data = &(tortik->recipe.entries[i]
			.ingridient->calories);
		package->base_entries[j + 2].data = &(tortik->recipe.entries[i]
			.ingridient->taste);
			
		package->var_arrays[i].data = tortik->recipe.entries[i]
			.ingridient->name;
	}		
}

void extend_tortik_package(struct serial_package *package, int r_base)
{
	package->base_entries_number = t_base + i_base * r_base;
	package->var_arrays_number = 1 + r_base;
	
	package->base_entries = malloc(package->base_entries_number
		       	* sizeof(struct serial_base));
		       		       	
	package->var_arrays = malloc(package->var_arrays_number
		       	* sizeof(struct serial_array));
	
	package->base_entries[0].type = serial_int;
	package->base_entries[1].type = serial_int;
	package->base_entries[2].type = serial_int;
	
	package->var_arrays[r_base].type = serial_char;

	for(int i = 0; i < r_base; i++) {
		int j = t_base + i_base * i;
		package->base_entries[j].type = serial_int;
		package->base_entries[j + 1].type = serial_int;
		package->base_entries[j + 2].type = serial_int;
		
		package->var_arrays[i].type = serial_char;
	}
};

int tortik_size_from_data(const struct tortik *tortik)
{
	struct serial_package package;
		
	header_tortik_package(&package, &(tortik->recipe.entries_number));
	extend_tortik_package(&package, tortik->recipe.entries_number);
	bind_tortik_package(&package, tortik);
	set_tortik_sizes(&package, tortik);
	
	int size = serial_package_size(&package);
	
	free_serial_package(&package);
	
	return size;
	
}

int tortik_size_from_header(const char *header, int readsize)
{
	struct serial_package package;
	int r_base;
		
	header_tortik_package(&package, &r_base);
	
	int size = serial_header_size(&package);
	if(size > readsize)
		return size;
		
	deserialize_header(&package, header);
	extend_tortik_package(&package, r_base);
	
	size = serial_header_size(&package);
	if(size > readsize)
		return size;
		
	deserialize_header(&package, header);
	
	size = serial_package_size(&package);
	
	free_serial_package(&package);
	
	return size;

}

int ingridient_size_from_data(const struct ingridient *ingridient)
{
	struct serial_package package;
		
	ingridient_package(&package, ingridient);
	set_ingridient_sizes(&package, ingridient);
	
	int size = serial_package_size(&package);
	
	free_serial_package(&package);
	
	return size;
	
}
int ingridient_size_from_header(const char *header, int readsize)
{
	struct serial_package package;
	struct ingridient ingridient; 
		
	ingridient_package(&package, &ingridient);
	
	int size = serial_header_size(&package);
	if(size > readsize)
		return size;
		
	deserialize_header(&package, header);

	size = serial_package_size(&package);
	
	free_serial_package(&package);
	
	return size;

}

int read_tortik(struct tortik *tortik, const char *buffer)
{
	struct serial_package package;	
	int r_base;
		
	header_tortik_package(&package, &r_base);
	deserialize_header(&package, buffer);
	extend_tortik_package(&package, r_base);
	deserialize_header(&package, buffer);

	tortik->recipe.entries_number = r_base;
	
	tortik->recipe.entries = malloc(r_base * sizeof(struct recipe_entry));
	tortik->name = malloc(package.var_arrays[r_base].data_size);
		
	for(int i = 0; i < r_base; i++) {
		tortik->recipe.entries[i].ingridient = malloc(sizeof(struct ingridient));
		tortik->recipe.entries[i].ingridient->name =
			malloc(package.var_arrays[i].data_size);
	}
	
	bind_tortik_package(&package, tortik);

	deserialize_package(&package, buffer);

	int size = serial_package_size(&package);
	
	free_serial_package(&package);
	
	return size;
};

int write_tortik(const struct tortik *tortik, char *buffer)
{
	struct serial_package package;

	header_tortik_package(&package, &(tortik->recipe.entries_number));
	extend_tortik_package(&package, tortik->recipe.entries_number);
	bind_tortik_package(&package, tortik);
	set_tortik_sizes(&package, tortik);
	
	int size = serial_package_size(&package);

	serialize_package(&package, buffer);
	free_serial_package(&package);

	return size;

};

int read_ingridient(struct ingridient *ingridient, const char *buffer)
{
	struct serial_package package;
	
	ingridient_package(&package, ingridient);
	deserialize_header(&package, buffer);
	ingridient->name = malloc(package.var_arrays[0].data_size);
	package.var_arrays[0].data = ingridient->name;
	
	int size = serial_header_size(&package);

	deserialize_data(&package, buffer + size);

	size = serial_package_size(&package);
	
	free_serial_package(&package);
	
	return size;
};
int write_ingridient(const struct ingridient *ingridient, char *buffer)
{
	struct serial_package package;

	ingridient_package(&package, ingridient);
	set_ingridient_sizes(&package, ingridient);
	
	int size = serial_package_size(&package);

	serialize_package(&package, buffer);
	free_serial_package(&package);

	return size;

};
