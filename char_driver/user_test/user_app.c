#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define DEVICE_NODE "/dev/char_dev"
/* Check entry point open */
int open_char_dev()
{
	int fd = open(DEVICE_NODE, O_RDWR);
	if( fd < 0 )
	{
		printf("Can not open the device file\n");
		exit(1);
	}
	return fd;
}

/* Check entry point release */
void close_char_dev(int fd)
{
	close(fd);
}

int main()
{
	int ret = 0;
	char option = 'q';
	int fd = -1;
	printf("Select below options:\n");
   	printf("\to (to open a device node)\n");
    	printf("\tc (to close the device node)\n");
    	printf("\tq (to quit the application)\n");
    	while (1) {
        	printf("Enter your option: ");
        	scanf(" %c", &option);
        	switch (option) {
            		case 'o':
                		if (fd < 0)
                    			fd = open_char_dev();
                		else
                    			printf("%s has already opened\n", DEVICE_NODE);
                		break;
            		case 'c':
                		if (fd > -1)
                    			close_char_dev(fd);
                		else
                    			printf("%s has not opened yet! Can not close\n", DEVICE_NODE);
                		fd = -1;
                		break;
            		case 'q':
                		if (fd > -1)
                    			close_char_dev(fd);
                		printf("Quit the application. Good bye!\n");
                		return 0;
            		default:
                		printf("invalid option %c\n", option);
                	break;
        	}
    	};
}
