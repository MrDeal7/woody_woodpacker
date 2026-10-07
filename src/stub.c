#include "woody_woodpacker.h"

typedef struct s_data {
	uint8_t key[32];
	uint8_t nonce[12];
	off_t size;
	off_t encryptedSize;
	off_t stubSize;
}t_data;

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

uint8_t* dencryptFd(int fd, t_data data)
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
		uint32_t state[16];
		fill32BitsBlock(state, data.key, data.nonce);
		chacha20Rounds(state);
		uint8_t *keystream = (uint8_t *)state;
	
		for (int i = 0; i < bytesR; i++) {
			decrypted_buffer[k + i] = cipherText[i] ^ keystream[i];
			decrypted_buffer[k + i] = cipherText[i];
   		}
		k += bytesR;
	}
	return (decrypted_buffer);
}

int main(int argc, char *argv[])
{
	(void)argc;

	int fd = open("/proc/self/exe", O_RDONLY);

	if (fd < 0) {
		return (1);
	}

	printf("....WOODY....\n");

	t_data data = getData(fd);
	uint8_t *decrypted_buffer = dencryptFd(fd, data);

	if (!decrypted_buffer) {
		return (1);
	}
	
	int memfd = syscall(SYS_memfd_create, "decrypted", MFD_CLOEXEC);
	if (memfd < 0) {
		free(decrypted_buffer);
		return (1);
	}
	write(memfd, decrypted_buffer, (off_t)((off_t)lseek(fd, 0, SEEK_END) - 48 - sizeof(off_t) - data.stubSize));
	free(decrypted_buffer);
	char memfd_path[1024];
	snprintf(memfd_path, sizeof(memfd_path), "/proc/self/fd/%d", memfd);	
	extern char **environ;

	// syscall(SYS_execve, memfd_path, argv, environ); // syscall exits
	if (syscall(SYS_execve, memfd_path, argv, environ) == -1) {
    perror("execve");
    return (1);
	}

	return (1);
}