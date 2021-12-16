#ifndef UTILITY_H
#define UTILITY_H

#include <stddef.h>

void load_data_from_file(const char* filename, char** output, size_t* len);
void write_array_to_file(const char* filename, const char* data, size_t len);
void load_data_and_write_array_to_file(const char* in_file, const char* out_file);

#endif //UTILITY_H
