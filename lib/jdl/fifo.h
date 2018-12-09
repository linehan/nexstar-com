#pragma once
#ifndef __FIFO_H
#define __FIFO_H

#include <stdbool.h>
#include <stdint.h>

struct fifo_t {
        uint32_t r; /* read */
        uint32_t w; /* write */
        uint32_t length;/* length % FIFO_BUFSIZE */
        char *buffer;
};

void fifo_reset  (struct fifo_t *f);
void fifo_alloc  (struct fifo_t *f, size_t size);
void fifo_enqueue(struct fifo_t *f, char v);
char fifo_dequeue(struct fifo_t *f);
char fifo_readn  (struct fifo_t *f, int n);

#endif
