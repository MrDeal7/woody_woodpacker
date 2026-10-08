#include "woody_woodpacker.h"


// Verifies if the file is valid and a 64-bit ELF returns the opened file fd on sucess and -1 on failure

int is_valid64bitELF(char *file_name)
{
	int fd = open(file_name, O_RDONLY);
	if (fd < 0) {
		printf("Can't open file\n");
		return(-1);
	}

	char header[20];
	read(fd, header, 20);
	if (header[0] == 0x7f && header[1] == 'E' && header[2] == 'L' && header[3] == 'F' && header[4] == 2)
	{
		lseek(fd, 0, SEEK_SET);
		return(fd);
	}
	else {
		printf("Wrong type of file\n");
		close(fd);
		return(-1);
	}
}

int main(int argc, char **argv)
{
	if (argc < 2 || argc > 2) {
		printf("Wrong ammount of args\n");
		exit(1);
	}

	int fd = is_valid64bitELF(argv[1]);
	if(fd < 0)
		exit(1);
	printf("64-bit ELF\n");

	int stubFd = open("./resources/stub", O_RDONLY);
	if (stubFd < 0) {
		printf("Can't open stub\n");
		close(fd);
		return (1);
	}

	int outputFd = open("woody", O_CREAT | O_WRONLY | O_TRUNC, 0755);
	if (outputFd < 0) {
		printf("Error while creating output file\n");
		close(stubFd);
		close(fd);
		return(1);
	}

	uint8_t stubBuffer[4096];
	ssize_t stubBytes;
	size_t stubSize = 0;
	while ((stubBytes = read(stubFd, stubBuffer, sizeof(stubBuffer))) > 0) {
		write(outputFd, stubBuffer, stubBytes); // write stub in woody
		stubSize += stubBytes;
	}
	close(stubFd);

	if (encryptFile(fd, outputFd, stubSize) != 0) {
		close(outputFd);
		close(fd);
		return (1);
	}

	close(outputFd);
	close(fd);
	return (0);

}