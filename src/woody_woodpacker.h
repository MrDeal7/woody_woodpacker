#ifndef WOODY_WOODPACKER_H
#define WOODY_WOODPACKER_H

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <sys/syscall.h>

#define ROTL(a, b) (((a) << (b)) | ((a) >> (32 - (b))))

int fillKey(uint8_t *buf, int len);
void fill32BitsBlock(uint32_t *state, uint8_t *key, uint8_t *nonce);
void chacha20Rounds(uint32_t *state);

#endif