// This program parse an input file and return the most frequently used words with their frequency.
// In this context a word is a continuous sequence of characters from 'a' to 'z'.
// The words are case-insensitive, so uppercase letters are always mapped in lowercase.
//
// The output is similar to that of the following bash command:
// cat <FILENAME> | tr -cs 'a-zA-Z' '[\n*]' | grep -v "^$" | tr '[:upper:]' '[:lower:]'| sort | uniq -c | sort -nr | head -20
//
// wordfreq.c
//
// @category   Tools
// @author     Nicola Asuni <info@tecnick.com>
// @copyright  2017-2018 Nicola Asuni - Tecnick.com
// @license    MIT (see LICENSE)
// @link       https://github.com/tecnickcom/wordfreq
//
// LICENSE
//
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

#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wordfreq.h"

#ifndef VERSION
#define VERSION "0.0.0-0"
#endif

#define MAX_RETURN_VALUES 20  //!< Default maximum number of words to return.

int main(int argc, char *argv[])
{
    uint8_t k = MAX_RETURN_VALUES;
    if (argc > 2)
    {
        k = (uint8_t)strtoul(argv[2], NULL, 10);
    }
    if ((argc < 2) || (k == 0))
    {
        fprintf(stderr, "WordFreq %s\n\
            Find the most frequently used words with their frequency.\n\
            Usage: wordfreq <INPUT_FILE> [MAX_RESULTS]\n", VERSION);
        return 1;
    }
    return wordfreq(argv[1], k);
}
