#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <serializing.h>
static const char *names[] = {
	"Croco", "Panda",
	"Choco", "Dogo",
	"Moca", "Tacocat",
	"Evrika", "Apple",
	"Tasty", "Muffin",
	"Some", "Beuty",
	"Chilly", "Red",
	"Leaves", "With Sugar",
	"No", "Milk",
	"Ice", "Cold",
	"Way more", "Smooth",
	"And also", "Now",
	"Another", "Dimension"

};

struct data {
	char *name;
	long marks[6];
	
	int age;
	int score;
	size_t size;
};


void print(struct data *data);
void generate_data(struct data *data);

void init_package(struct serial_package *package);
void bind_package(struct serial_package *package, struct data *data);
void set_sizes(struct serial_package *package, struct data *data);

int check(struct data *old,  struct data *new);

static const size_t names_number = sizeof(names) / sizeof(char *);

#define N 1000

int main() 
{

	char buffer[556];
	struct data data[N];
	
	struct serial_package *package = malloc(sizeof(struct serial_package));
	
	init_package(package);
	
	struct data new;
	new.name = malloc(66);

	int success = 0;

	for(int i = 0; i < N; i++) {
					
		generate_data(data + i);
		bind_package(package, data + i);
		set_sizes(package, data + i);
		printf("package size: %ld\n", serial_package_size(package));
		
		serialize_package(package, buffer);
		
		bind_package(package, &new);
		deserialize_package(package, buffer);
	
		if(check(data + i, &new) < 0)
			success = -1;
	}

	return success;
}



void init_package(struct serial_package *package)
{

	package->base_entries_number = 3;
	package->base_entries = malloc(3 * sizeof(struct serial_base));
	
	package->base_entries[0].type = serial_int;
	package->base_entries[1].type = serial_int;
	package->base_entries[2].type = serial_size;
	
	package->var_arrays_number = 1;
	package->var_arrays = malloc(sizeof(struct serial_array));
	
	package->var_arrays[0].type = serial_char;
	
	package->const_arrays_number = 1;
	package->const_arrays = malloc(sizeof(struct serial_array));
	
	package->const_arrays[0].type = serial_long;
	package->const_arrays[0].data_size = 6 * sizeof(long);

}
void bind_package(struct serial_package *package, struct data *data)
{
	package->base_entries[0].data = &(data->age);
	package->base_entries[1].data = &(data->score);
	package->base_entries[2].data = &(data->size);
	
	package->const_arrays[0].data = data->marks; 

	package->var_arrays[0].data = data->name;
}

void set_sizes(struct serial_package *package, struct data *data)
{
	package->var_arrays[0].data_size = strlen(data->name) + 1;
}

int check(struct data *old,  struct data *new)
{
	int success = 0;
	if(	strcmp(old->name, new->name) != 0 ||
		old->age != new->age ||
		old->score != new->score ||
		old->size != new->size ||
		old->marks[0] != new->marks[0]) {
		
		printf("Wrong: ");
		print(new);
		
		printf("\nShould be: ");
		
		success = -1;
		
	} else {
		return success;
		printf("Right: ");
		
	}
	
	print(old);
	
	return success;
}

void generate_data(struct data *data) {
	static int i = 0;
	static size_t test = 1;
	
	int size = strlen(names[i]) + 1;
	
	data->name = malloc(size);
	data->size = test++;
	memcpy(data->name, names[i], size);
	data->age = (rand() % 20) + 11;
	
	for(int j = 0; j < 6; j++) {
		data->marks[j] = 2 + (rand() % 4);
	}
	
	data->score = rand() % 101;
	
	i++;
	
	if(i == names_number)
		i = 0;

}

void print(struct data *data)
{

	printf("Name: %s, size: %lu, age: %d, score: %d, mark: %ld\n", 
		data->name, data->size,
		data->age, data->score, data->marks[0]);
}
