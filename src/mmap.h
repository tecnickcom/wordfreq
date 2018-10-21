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
 * @file mmap.h
 * @brief Memory-map functions
 */

#ifndef WORDFREQ_MMAP_H
#define WORDFREQ_MMAP_H

#include <inttypes.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

/**
 * Struct containing the memory mapped file info.
 */
typedef struct mmfile_t
{
    uint8_t *src;  //!< Pointer to the memory map.
    int fd;        //!< File descriptor.
    uint64_t size; //!< File size in bytes.
} mmfile_t;

/**
 * Memory map the specified file.
 *
 * @param file  Path to the file to map.
 * @param mf    Structure containing the memory mapped file.
 *
 * @return Returns the memory-mapped file descriptors.
 */
static inline void mmap_file(const char *file, mmfile_t *mf)
{
    mf->src = (uint8_t *)MAP_FAILED; // NOLINT
    mf->fd = -1;
    mf->size = 0;
    struct stat statbuf;
    if (((mf->fd = open(file, O_RDONLY)) < 0) || (fstat(mf->fd, &statbuf) < 0))
    {
        return;
    }
    mf->size = (uint64_t)statbuf.st_size;
    mf->src = (uint8_t *)mmap(0, mf->size, PROT_READ, MAP_PRIVATE, mf->fd, 0);
}

/**
 * Unmap and close the memory-mapped file.
 *
 * @param mf Descriptor of memory-mapped file.
 *
 * @return On success, munmap() returns 0,
 *         on failure -1, and errno is set (probably to EINVAL).
 */
static inline int munmap_file(mmfile_t mf)
{
    int err = munmap(mf.src, mf.size);
    if (err != 0)
    {
        return err;
    }
    return close(mf.fd);
}

#endif  // WORDFREQ_MMAP_H
