#include "woody_woodpacker.h"

int fillKey(uint8_t *buf, int len)
{
	int off = 0;

	while (off < len) {
		long n = syscall(SYS_getrandom, buf + off, len - off, 0);
		if (n < 0) {
			if (errno == EINTR)
				continue;
			printf("Problem with syscall\n");
			return (-1);
		}

		off += n;
	}
	return (0);
}

void fill32BitsBlock(uint32_t *state, uint8_t *key, uint8_t *nonce)
{
	// Fill state[0-3] with constants
    // Fill state[4-11] with key bytes (convert to words)
    // Fill state[12] with counter
    // Fill state[13-15] with nonce bytes (convert to words)

	state[0] = 0x61707865;  // "expa"
	state[1] = 0x3320646e;  // "nd 3"
	state[2] = 0x79622d32;  // "2-by"
	state[3] = 0x6b206574;  // "te k"

	state[4] = key[0] | (key[1] << 8) | (key[2] << 16) | (key[3] << 24);
	state[5] = key[4] | (key[5] << 8) | (key[6] << 16) | (key[7] << 24);
	state[6] = key[8] | (key[9] << 8) | (key[10] << 16) | (key[11] << 24);
	state[7] = key[12] | (key[13] << 8) | (key[14] << 16) | (key[15] << 24);
	state[8] = key[16] | (key[17] << 8) | (key[18] << 16) | (key[19] << 24);
	state[9] = key[20] | (key[21] << 8) | (key[22] << 16) | (key[23] << 24);
	state[10] = key[24] | (key[25] << 8) | (key[26] << 16) | (key[27] << 24);
	state[11] = key[28] | (key[29] << 8) | (key[30] << 16) | (key[31] << 24);

	static uint32_t counter = 0;
	state[12] = counter;
	counter++;

	state[13] = nonce[0] | (nonce[1] << 8) | (nonce[2] << 16) | (nonce[3] << 24);
	state[14] = nonce[4] | (nonce[5] << 8) | (nonce[6] << 16) | (nonce[7] << 24);
	state[15] = nonce[8] | (nonce[9] << 8) | (nonce[10] << 16) | (nonce[11] << 24);
	
}

void quarterRound(uint32_t *state, int a, int b, int c, int d)
{
	state[a] += state[b];  state[d] ^= state[a];  state[d] = ROTL(state[d], 16);
	state[c] += state[d];  state[b] ^= state[c];  state[b] = ROTL(state[b], 12);
	state[a] += state[b];  state[d] ^= state[a];  state[d] = ROTL(state[d], 8);
	state[c] += state[d];  state[b] ^= state[c];  state[b] = ROTL(state[b], 7);
}

void chacha20Rounds(uint32_t *state)
{
	for (int i = 0; i < 10; i++) {
		quarterRound(state, 0, 4, 8, 12);
		quarterRound(state, 1, 5, 9, 13);
		quarterRound(state, 2, 6, 10, 14);
		quarterRound(state, 3, 7, 11, 15);

		quarterRound(state, 0, 5, 10, 15);
		quarterRound(state, 1, 6, 11, 12);
		quarterRound(state, 2, 7, 8, 13);
		quarterRound(state, 3, 4, 9, 14);
	}
}