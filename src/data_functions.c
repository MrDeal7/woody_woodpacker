#include "woody_woodpacker.h"


//File foot
// stub size is stored in bytes 52-44 from the end of the file
//nonce and key are stored in the last 44 bytes of the file

t_data getData(int fd)
{
	t_data data;

	data.size = lseek(fd, 0, SEEK_END);

	lseek(fd, data.size - FOOT_SIZE, SEEK_SET);

	read(fd, &data.stubSize, sizeof(data.stubSize));
	read(fd, data.key, KEY_SIZE);
	read(fd, data.nonce, NONCE_SIZE);
	data.encryptedSize = data.size - FOOT_SIZE - data.stubSize;

	lseek(fd, data.stubSize, SEEK_SET);
	return data;
}

void printData(int outputFd, t_data data)
{
	write(outputFd, &data.stubSize, sizeof(data.stubSize));
	write(outputFd, data.key, 32);
	write(outputFd, data.nonce, 12);
}