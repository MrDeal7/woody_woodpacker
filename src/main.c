#include "woody_woodpacker.h"

void printKey(uint8_t *buf, int len)
{
	printf("key: ");
	for (int i = 0; i < len; i++) {
		printf("%02x", buf[i]);
	}
	printf("\n");
}

int encryptFile(int fd, int outputFd)
{
	uint8_t key[32];
	uint8_t nonce[12];
	if (fillKey(key, 32) < 0) {
		return (1);
	}
	if (fillKey(nonce, 12) < 0) {
		return (1);
	}
	printKey(key, 32);
	printKey(nonce, 12);

	int bytesTotal = 0;
	uint8_t plainText[64];
	uint8_t cipherText[64];

	while ((bytesTotal = read(fd, plainText, 64)) > 0) {
		uint32_t state[16];
		fill32BitsBlock(state, key, nonce);
		chacha20Rounds(state);

		uint8_t *keystream = (uint8_t *)state;
		for (int i = 0; i < bytesTotal; i++) {
			cipherText[i] = plainText[i] ^ keystream[i];
		}

		write(outputFd, cipherText, bytesTotal);
	}

	write(outputFd, key, 32);
	write(outputFd, nonce, 12);

	return (0);
}

int main(int argc, char **argv)
{
	if (argc < 2 || argc > 2) {
		printf("Wrong ammount of args\n");
		exit(1);
	}

	int fd = open(argv[1], O_RDONLY);
	if (fd < 0) {
		printf("Can't open file\n");
		exit(1);
	}

	char header[20];
	read(fd, header, 20);
	if (header[0] == 0x7f && header[1] == 'E' && header[2] == 'L' && header[3] == 'F' && header[4] == 2)
		printf("64-bit ELF\n");
	else {
		printf("Wront type of file\n");
		exit(1);
	}
	
	lseek(fd, 0, SEEK_SET);

	int outputFd = open("woody", O_CREAT | O_WRONLY | O_TRUNC, 0644);
	if (outputFd < 0) {
		printf("Error while creating output file\n");
		return(1);
	}
	if (encryptFile(fd, outputFd) != 0) {
		return (1);
	}

	close(outputFd);
	close(fd);

}