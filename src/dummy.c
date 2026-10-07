#include "woody_woodpacker.h"


int dummy_encrypt(int inputFd, int outputFd)
{
    char buffer[1000];
    int bread;
    do
    {
        bread = read(inputFd, buffer, 1000);
        write(outputFd, buffer, bread);
    }
    while(bread == 1000);

	return (0);
}



// int dummy_dencrypt()
// {
    
	
// }