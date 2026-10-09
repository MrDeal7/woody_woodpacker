#include "woody_woodpacker.h"

void printKey(uint8_t *buf, int len)
{
	printf("key: ");
	for (int i = 0; i < len; i++) {
		printf("%02x", buf[i]);
	}
	printf("\n");
}




int encryptFile(int fd, int outputFd, size_t stubSize)
{
	t_data data;
	
	if (fillKey(data.key, 32) < 0) {
		return (1);
	}
	if (fillKey(data.nonce, 12) < 0) {
		return (1);
	}
	printKey(data.key, 32);
	printKey(data.nonce, 12);

	int bytesTotal = 0;
    int k = 0;
	uint8_t plainText[64];
	uint8_t cipherText[1024];

	while ((bytesTotal = read(fd, plainText, 64)) > 0) {
        
        
        ////////////////////ENCRYPTS/////////////////////////////////////
        uint32_t state[16];
		fill32BitsBlock(state, data.key, data.nonce);
		chacha20Rounds(state);
        
		uint8_t *keystream = (uint8_t *)state;
		for (int i = 0; i < bytesTotal; i++) {
            cipherText[k * 64 + i] = plainText[i] ^ keystream[i];
		}
        k++;
        ////////////////////COMPRESS/////////////////////////////////////




        ////////////////////WRITE/////////////////////////////////////
        if(bytesTotal < 64)
        {
            write(outputFd, cipherText, (k - 1) * 64 + bytesTotal);
            k = 0;
        }
        if(k == 16)
        {
		    write(outputFd, cipherText, 15 * 64 + bytesTotal);
            k = 0;
        }
	}
    if(k != 0)
    {
        write(outputFd, cipherText, k * 64);
    }
	
	data.stubSize = (off_t)stubSize;

	printData(outputFd, data);

	return (0);
}