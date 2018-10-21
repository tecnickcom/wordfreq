// Nicola Asuni

#if __STDC_VERSION__ >= 199901L
#define _XOPEN_SOURCE 600
#else
#define _XOPEN_SOURCE 500
#endif

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <sys/mman.h>
#include "../src/mmap.h"

int test_mmap_file_error(const char* file)
{
    mmfile_t mf = {0};
    mmap_file(file, &mf);
    if (mf.src != MAP_FAILED)
    {
        fprintf(stderr, "An mmap error was expected\n");
        return 1;
    }
    return 0;
}

int test_munmap_file_error()
{
    mmfile_t mf = {0};
    int e = munmap_file(mf);
    if (e == 0)
    {
        fprintf(stderr, "An mummap error was expected\n");
        return 1;
    }
    return 0;
}

int test_mmap_file()
{
    int errors = 0;
    char *file = "test01.txt";
    mmfile_t mf = {0};
    mmap_file(file, &mf);
    if (mf.fd < 0)
    {
        fprintf(stderr, "%s can't open %s for reading\n", __func__, file);
        return 1;
    }
    if (mf.size == 0)
    {
        fprintf(stderr, "%s fstat error! [%s]\n", __func__, strerror(errno));
        return 1;
    }
    if (mf.src == MAP_FAILED)
    {
        fprintf(stderr, "%s mmap error! [%s]\n", __func__, strerror(errno));
        return 1;
    }
    int e = munmap_file(mf);
    if (e != 0)
    {
        fprintf(stderr, "%s unmap error! [%s]\n", __func__, strerror(errno));
        return 1;
    }
    return errors;
}

int main()
{
    int errors = 0;

    errors += test_mmap_file_error("ERROR");
    errors += test_mmap_file_error("/dev/null");
    errors += test_munmap_file_error();
    errors += test_mmap_file();

    return errors;
}
