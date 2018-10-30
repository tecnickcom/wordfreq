// Copyright (c) 2017-2018 Nicola Asuni - Tecnick.com
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

/**
 * @file wordfreq.h
 * @brief Functions to retrieve the most frequently used words in a file.
 *
 * The functions provided here allows to parse an input file and
 * return the most frequently used words.
 * In this context a word is a continuous sequence of characters from 'a' to 'z'.
 * The words are case-insensitive, so uppercase letters are always mapped in lowercase.
 */

#ifndef WORDFREQ_WORDFREQ_H
#define WORDFREQ_WORDFREQ_H

#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "mmap.h"

#define ALPHABET_SIZE     26  //!< 26 slots each for 'a' to 'z'.
#define NOCH            0xff  //!< Code used to identify an invalid character.
#define MAX_WORD_LENGTH  250  //!< Maximum word lenght.

/**
 * Returns the character index.
 * Encode characters ['a','z'] and ['A','Z'] to [0,25].
 * All other characters are mapped to 0.
 *
 * @param c  Character to encode.
 *
 * @return Returns the character index.
 */
static inline uint8_t get_char_index(const uint8_t c)
{
    static const uint8_t map[] =
    {
        NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,
        NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,
        NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,
        NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,
        NOCH,0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,
        0x0f,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,NOCH,NOCH,NOCH,NOCH,NOCH,
        NOCH,0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,
        0x0f,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,NOCH,NOCH,NOCH,NOCH,NOCH,
        NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,
        NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,
        NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,
        NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,
        NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,
        NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,
        NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,
        NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,NOCH,
    };
    return map[c];
}

/**
 * Decode a character index back to character code: [0,25] to ['a','z'].
 *
 * @param c  Character index to decode.
 *
 * @return Returns the character code.
 */
static inline uint8_t get_index_char(const uint8_t c)
{
    return (c + 'a');
}

/**
 * Struct containing a trie node.
 */
typedef struct trie_node_t
{
    bool isend;                               //!< True if the node represent the last character of a word.
    uint8_t  hfidx;                           //!< Position in the hifreq list.
    uint32_t freq;                            //!< Word frequency (number of occurrences).
    struct trie_node_t *child[ALPHABET_SIZE]; //!< Pointers to child nodes, one for each alphabet letter.
} trie_node_t;

/**
 * Returns new trie node.
 *
 * @return Pointer to the new trie node.
 */
static inline trie_node_t *new_trie_node(void)
{
    trie_node_t *node = (trie_node_t *)malloc(sizeof(trie_node_t));
    if (!node)
    {
        return NULL;
    }
    node->isend = false;
    node->hfidx = 0;
    node->freq = 0;
    for (uint8_t i = 0; i < ALPHABET_SIZE; i++)
    {
        node->child[i] = NULL;
    }
    return node;
}

/**
 * Frees a trie node by node recursively.
 *
 * @param node Pointer to a trie node.
 */
static inline void free_trie_node(trie_node_t *node)
{
    for (uint8_t i = 0; i < ALPHABET_SIZE; i++)
    {
        if (node->child[i])
        {
            free_trie_node(node->child[i]);
        }
    }
    free(node);
}

/**
 * Struct containing a single hifreq item.
 */
typedef struct hifreq_item_t
{
    trie_node_t *node;          //!< Pointer to trie leaf node.
    char word[MAX_WORD_LENGTH]; //!< Word.
} hifreq_item_t;

/**
 * Struct containing the list of high-frequency words (min heap).
 * The maximum frequency word is at position 1.
 */
typedef struct hifreq_t
{
    uint8_t size;        //!< Max number of words to store.
    uint8_t count;       //!< Number of slots filled.
    hifreq_item_t *item; //!< List of nodes + words.
} hifreq_t;

/**
 * Returns a new hifreq object.
 *
 * @param size Maximum number of words.
 *
 * @return Pointer to the new hifreq.
 */
static inline hifreq_t *new_hifreq(uint8_t size)
{
    hifreq_t *hf = (hifreq_t *)malloc(sizeof(hifreq_t));
    hf->size = size;
    hf->count = 0;
    hf->item = (hifreq_item_t *)malloc(((uint64_t)size + 1) * sizeof(hifreq_item_t));
    if (!hf->item)
    {
        free(hf);
        return NULL;
    }
    for (uint8_t i = 0; i <= size; i++)
    {
        hf->item[i].node = NULL;
        hf->item[i].word[0] = 0;
    }
    return hf;
}

/**
 * Free the hifreq object.
 *
 * @param hf Pointer to hifreq object.
 */
static inline void free_hifreq(hifreq_t *hf)
{
    free(hf->item);
    free(hf);
}

/**
 * Swap two hifreq items.
 *
 * @param hf Pointer to hifreq object.
 * @param a Position of the first item.
 * @param b Position of the second item.
 */
static void swap_items(hifreq_t *hf, uint8_t a, uint8_t b)
{
    hf->item[a].node->hfidx = b;
    hf->item[b].node->hfidx = a;
    hifreq_item_t tmp = hf->item[a];
    hf->item[a] = hf->item[b];
    hf->item[b] = tmp;
}


/**
 * Heapify the min heap.
 *
 * @param hf  Pointer to hifreq object.
 * @param idx Item position.
 */
static void heapify(hifreq_t *hf, uint8_t idx)
{
    uint8_t left, right, small;
    left = (2 * idx);
    right = (left + 1);
    small = idx;
    if ((left <= hf->count) && (hf->item[left].node->freq < hf->item[small].node->freq))
    {
        small = left;
    }
    if ((right <= hf->count) && (hf->item[right].node->freq < hf->item[small].node->freq))
    {
        small = right;
    }
    if (small != idx)
    {
        swap_items(hf, small, idx);
        heapify(hf, small);
    }
}

/**
 * Update the hifreq list.
 *
 * @param hf   Pointer to hifreq object.
 * @param node Pointer to a trie node.
 * @param word Current word.
 */
static inline void update_hifreq(hifreq_t *hf, trie_node_t *node, const char *word)
{
    // update existing node (word)
    if (node->hfidx != 0)
    {
        heapify(hf, node->hfidx);
        return;
    }
    // add new node (word) - insert sort
    if (hf->count < hf->size)
    {
        ++(hf->count);
        node->hfidx = hf->count;
        hf->item[hf->count].node = node;
        memcpy(hf->item[hf->count].word, word, MAX_WORD_LENGTH);
        for (uint8_t i = (hf->count / 2); i > 0; --i)
        {
            heapify(hf, i);
        }
        return;
    }
    // replace min frequency node (word)
    if (node->freq > hf->item[1].node->freq)
    {
        hf->item[1].node->hfidx = 0;
        node->hfidx = 1;
        hf->item[1].node = node;
        memcpy(hf->item[1].word, word, MAX_WORD_LENGTH);
        heapify(hf, node->hfidx);
    }
}

/**
 * Reorder the items in descending order.
 *
 * @param hf hifreq object.
 */
static inline void order_hifreq(hifreq_t *hf)
{
    uint8_t count = hf->count;
    while (hf->count > 2)
    {
        swap_items(hf, 1, hf->count);
        --(hf->count);
        heapify(hf, 1);
    }
    if (hf->count == 2)
    {
        swap_items(hf, 1, 2);
    }
    hf->count = count;
}

/**
 * Parse the input file byte-by-byte and update the data structures.
 *
 * @param src  Pointer to the memory mapped file data.
 * @param size File size in bytes.
 * @param root Root of the trie data structure.
 * @param mh   Pointer to the hifreq object.
 */
static inline void parse_data(const uint8_t *src, uint64_t size, trie_node_t *root, hifreq_t *hf)
{
    uint8_t idx; // character index
    trie_node_t *node = root;
    char word[MAX_WORD_LENGTH] = "";
    uint8_t pos = 0;
    for (uint64_t i = 0; i < size; i++)
    {
        idx = get_char_index(*src++);
        if (idx == NOCH)
        {
            if (pos > 0)
            {
                node->isend = true;
                ++(node->freq);
                word[pos] = 0;
                update_hifreq(hf, node, word);
                pos = 0;
            }
            node = root;
            continue;
        }
        if (!node->child[idx])
        {
            node->child[idx] = new_trie_node();
        }
        node = node->child[idx];
        word[pos] = get_index_char(idx);
        if (pos < (MAX_WORD_LENGTH - 1))
        {
            ++pos;
        }
    }
    node->isend = true;
    ++(node->freq);
    root->isend = false;
    order_hifreq(hf);
}

/**
 * Print the high frequency words.
 *
 * @param hf hifreq object.
 */
static inline void print_hifreq(hifreq_t *hf)
{
    for (uint8_t i = 1; i <= hf->count; i++)
    {
        fprintf(stdout, "%10" PRIu32 " %s\n", hf->item[i].node->freq, hf->item[i].word);
    }
}

/**
 * Parse an input file and print the most frequently used words with their frequency.
 *
 * @param file File to parse.
 * @param k    Number of words to return.
 *
 * @return Error code, 0 in case of success.
 */
static inline int wordfreq(const char *file, uint8_t k)
{
    // memory-map the input file
    mmfile_t mf = {0,0,0};
    mmap_file(file, &mf);
    if (mf.fd < 0)
    {
        fprintf(stderr, "ERROR: can't open '%s' file.\n", file);
        return 1;
    }
    if (mf.size == 0)
    {
        fprintf(stderr, "ERROR: fstat [%s]\n", strerror(errno));
        return 2;
    }
    if (mf.src == MAP_FAILED)
    {
        fprintf(stderr, "ERROR: mmap [%s]\n", strerror(errno));
        return 3;
    }

    trie_node_t *root = new_trie_node();
    if (!root)
    {
        fprintf(stderr, "ERROR: Unable to allocate memory.\n");
        return 4;
    }

    hifreq_t *hf = new_hifreq(k);
    if (!hf)
    {
        fprintf(stderr, "ERROR: Unable to allocate memory.\n");
        return 5;
    }

    parse_data(mf.src, mf.size, root, hf);
    print_hifreq(hf);

    free_hifreq(hf);
    free_trie_node(root);

    // unmap the file
    int e = munmap_file(mf);
    if (e != 0)
    {
        fprintf(stderr, "Got %s error while unmapping the %s file\n", strerror(errno), file);
        return 6;
    }
    return 0;
}

#endif  // WORDFREQ_WORDFREQ_H
