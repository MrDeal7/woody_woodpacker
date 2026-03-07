#include "woody_woodpacker.h"

int main(void)
{
	int fd = open("/proc/self/exe", O_RDONLY);

	if (fd < 0) {
		return (1);
	}

	uint8_t key[32];
	uint8_t nonce[12];

	off_t size = lseek(fd, 0, SEEK_END);
	lseek(fd, size - 12, SEEK_SET);
	read(fd, nonce, 12);
	lseek(fd, size - 44, SEEK_SET);
	read(fd, key, 32);

	lseek(fd, STUB_SIZE, SEEK_SET);


	off_t encryptedSize = size - 44 - STUB_SIZE;
	uint8_t *decrypted_buffer = malloc(encryptedSize);
	if (!decrypted_buffer) {
		return (1);
	}

	int bytesR = 0;
	uint8_t cipherText[64];
	int k = 0;
	printf("....WOODY....\n");
	while ((bytesR = read(fd, cipherText, 64)) > 0 && k < encryptedSize) {
		uint32_t state[16];
		fill32BitsBlock(state, key, nonce);
		chacha20Rounds(state);
		uint8_t *keystream = (uint8_t *)state;
	
		for (int i = 0; i < bytesR; i++) {
			decrypted_buffer[k + i] = cipherText[i] ^ keystream[i];
   		}
		k += bytesR;
	}
}