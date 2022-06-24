#ifndef DB_SERIAL_H
#define DB_SERIAL_H
#include "data.h"

int tortik_size_from_data(const struct tortik *tortik);
int tortik_size_from_header(const char *header, int readsize);

int ingridient_size_from_data(const struct ingridient *ingridient);
int ingridient_size_from_header(const char *header, int readsize);

int read_tortik(struct tortik *tortik, const char *buffer);
int write_tortik(const struct tortik *tortik, char *buffer);

int read_ingridient(struct ingridient *ingridient, const char *buffer);
int write_ingridient(const struct ingridient *ingridient, char *buffer);

#endif
