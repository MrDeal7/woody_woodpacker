#include "woody_woodpacker.h"


int dencryptFd(int fd, int outputFd, t_data data)
{
	
	////////////////////DECOMPRESS/////////////////////////////////////
	
	uint8_t decrypted_buffer[64];
	int bytesR = 0;
	uint8_t cipherText[64];
	int k = 0;

	while (k < data.encryptedSize) {
		size_t toRead = 64;
		// if ((off_t)toRead > data.encryptedSize - k)
		// 	toRead = (size_t)(data.encryptedSize - k);
		bytesR = read(fd, cipherText, toRead);
		if (bytesR <= 0)
			break;
			
			
		////////////////////DECRYPTS/////////////////////////////////////
		uint32_t state[16];
		fill32BitsBlock(state, data.key, data.nonce);
		chacha20Rounds(state);
		uint8_t *keystream = (uint8_t *)state;
		for (int i = 0; i < bytesR; i++) {
			decrypted_buffer[i] = cipherText[i] ^ keystream[i];			
   		}

		write(outputFd, decrypted_buffer, bytesR);
		k += bytesR;
	}

	return (0);
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
	
	int memfd = syscall(SYS_memfd_create, "decrypted", MFD_CLOEXEC);

	if (memfd < 0) {
		return (1);
	}

	if (dencryptFd(fd, memfd, data) != 0) {
		return (1);
	}
	
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