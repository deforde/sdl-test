#include "utility.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void load_data_from_file(const char* const filename, char** const output, size_t* len)
{
    FILE* file = fopen(filename, "rb");
    if(!file) {
        fprintf(stderr, "Failed to open file: \"%s\"\n", filename);
        return;
    }

    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    *output = (char*)malloc(file_size);
    *len = file_size;
    fread(*output, file_size, 1, file);
    fclose(file);
}

void write_array_to_file(const char* const filename, const char* const data, size_t len)
{
    FILE* file = fopen(filename, "w");
    if(!file) {
        fprintf(stderr, "Failed to open file: \"%s\"\n", filename);
        return;
    }

    fwrite("const char buffer[] = {", 1, 23, file);
    char temp[5];
    for(size_t i = 0; i < len; ++i) {
        int num_bytes_written = sprintf(temp, "%i", data[i]);
        fwrite(temp, 1, num_bytes_written, file);
        if(i != len - 1) {
            fwrite(", ", 1, 2, file);
        }
    }
    fwrite("};", 1, 2, file);

    fclose(file);
}

void load_data_and_write_array_to_file(const char* const in_file, const char* const out_file)
{
    char* buf;
    size_t len = 0;
    load_data_from_file(in_file, &buf, &len);
    write_array_to_file(out_file, buf, len);
    free(buf);
}
