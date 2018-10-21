// Nicola Asuni

#if __STDC_VERSION__ >= 199901L
#define _XOPEN_SOURCE 600
#else
#define _XOPEN_SOURCE 500
#endif

#include <stdio.h>
#include <string.h>
#include <time.h>
#include "../src/wordfreq.h"

int test_wordfreq()
{
    int e = wordfreq("test01.txt", 10);
    if (e != 0)
    {
        fprintf(stderr, "%s worfreq error: %d\n", __func__, e);
        return 1;
    }
    return 0;
}

int test_parse_data()
{
    // memory-map the input file
    mmfile_t mf = {0,0,0};
    char file[] = "mobydick.txt";
    mmap_file(file, &mf);
    if (mf.fd < 0)
    {
        fprintf(stderr, "%s ERROR: can't open '%s' file.\n", __func__, file);
        return 1;
    }
    if (mf.size == 0)
    {
        fprintf(stderr, "%s ERROR: fstat [%s]\n", __func__, strerror(errno));
        return 1;
    }
    if (mf.src == MAP_FAILED)
    {
        fprintf(stderr, "%s ERROR: mmap [%s]\n", __func__, strerror(errno));
        return 1;
    }

    trie_node_t *root = new_trie_node();
    if (!root)
    {
        fprintf(stderr, "%s ERROR: Unable to allocate memory.\n", __func__);
        return 1;
    }

    static const uint8_t k = 100;
    hifreq_t *hf = new_hifreq(k);
    if (!hf)
    {
        fprintf(stderr, "%s ERROR: Unable to allocate memory.\n", __func__);
        return 5;
    }

    parse_data(mf.src, mf.size, root, hf);

    int errors = 0;
    uint32_t freq[] =
    {
        0,
        4284, 2192, 2185, 1861, 1685, 1366, 1056, 1024, 889, 821,
        783,  616,  603,  595,  577,  564,  551,  542, 541, 458,
        452,  419,  410,  384,  366,  362,  362,  347, 345, 342,
        327,  320,  288,  274,  264,  263,  259,  254, 234, 231,
        227,  214,  212,  211,  199,  199,  192,  192, 191, 189,
        186,  186,  184,  182,  179,  178,  178,  176, 171, 168,
        167,  163,  161,  156,  154,  152,  149,  147, 147, 143,
        142,  138,  132,  129,  127,  126,  126,  125, 124, 124,
        123,  123,  122,  122,  119,  119,  117,  112, 111, 109,
        108,  107,  104,  104,  101,  101,  100,   98,  97,  97,
    };
    if (hf->count != k)
    {
        fprintf(stderr, "%s ERROR: expected (%" PRIu8 ") results, got %" PRIu8 "\n", __func__, k, hf->count);
        ++errors;
    }
    else
    {
        for (uint8_t i = 1; i <= k; i++)
        {
            if (hf->node[i]->freq != freq[i])
            {
                fprintf(stderr, "%s ERROR: different frequency for (%" PRIu8 "): %" PRIu32 " != %" PRIu32 ".\n", __func__, i, hf->node[i]->freq, freq[i]);
                ++errors;
            }
        }
    }

    free_hifreq(hf);
    free_trie_node(root);

    // unmap the file
    int e = munmap_file(mf);
    if (e != 0)
    {
        fprintf(stderr, "%s Got %s error while unmapping the %s file\n", __func__, strerror(errno), file);
        return 1;
    }
    return errors;
}

int main()
{
    int errors = 0;

    errors += test_wordfreq();
    errors += test_parse_data();

    return errors;
}




















