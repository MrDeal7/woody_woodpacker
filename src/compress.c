#include "woody_woodpacker.h"


int BinaryCompression(int fd, int outputFd, size_t stubSize)
{
	
    t_data  data;
	int bytesTotal = 0;
    uint8_t plainText[64];
	uint8_t cipherText[64];
	

	while ((bytesTotal = read(fd, plainText, 64)) > 0) {
		

		for (int i = 0; i < bytesTotal; i++)
			cipherText[i] = plainText[i] + 1;
	
		write(outputFd, cipherText, bytesTotal);
	    }
    data.stubSize = (off_t)stubSize;
	printData(outputFd, data);
	return (0);
}



uint8_t* BinaryDecrompression(int fd, t_data data)
{
	
    uint8_t *decrypted_buffer = malloc(data.encryptedSize);
	if (!decrypted_buffer) {
		return (NULL);
	}

	int bytesR = 0;
	uint8_t cipherText[64];
	int k = 0;

	while (k < data.encryptedSize) {
		size_t toRead = 64;
		if ((off_t)toRead > data.encryptedSize - k)
			toRead = (size_t)(data.encryptedSize - k);
		bytesR = read(fd, cipherText, toRead);
		if (bytesR <= 0)
			break;
	
		for (int i = 0; i < bytesR; i++) {
			decrypted_buffer[k + i] = cipherText[i] -1;
   		}
		k += bytesR;
	}
	return (decrypted_buffer);
}