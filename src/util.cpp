#include <cstdio>
#include <cassert>

#include "util.hpp"

int slurp(const char *filename, void *buffer, int buffer_size)
{
    FILE *f = fopen(filename, "rb");

    if (!f)
    {
        printf("slurp(): cannot open file '%s'\n", filename);
        assert(0);
    }

    fseek(f, 0, SEEK_END);
    long end = ftell(f);
    fseek(f, 0, SEEK_SET);
    long size = end - ftell(f);

    if (size > buffer_size)
    {
        printf("slurp(): '%s' cannot fit in provided buffer, aborting read\n", filename);
        fclose(f);
        assert(0);
    }

    int read = fread(buffer, 1, size, f);
    assert(read == size);

    fclose(f);

    return read;
}

int file_size(const char *filename)
{
    FILE *f = fopen(filename, "rb");

    if (!f)
    {
        printf("file_size(): cannot open file '%s'\n", filename);
        assert(0);
    }

    fseek(f, 0, SEEK_END);
    long end = ftell(f);
    fseek(f, 0, SEEK_SET);
    long size = end - ftell(f);

    fclose(f);

    return (int)size;
}

