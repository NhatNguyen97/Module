#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#define BUFFER 1024
#define DEVICE_NODE "/dev/led_device"
/* Check entry point open */
int open_led_dev()
{
	int fd = open(DEVICE_NODE, O_RDWR);
	if( fd < 0 )
	{
		printf("Can not open the device file\n");
		exit(1);
	}
	return fd;
}

void close_led_dev(int fd)
{
	close(fd);
}

void read_data_led_dev()
{
	int ret = 0;
	char user_buf[BUFFER];
	int fd = open_led_dev();
	ret = read(fd, user_buf, BUFFER);
	close_led_dev(fd);
	if (ret < 0)
		printf("Could not read a message from %s\n", DEVICE_NODE);
	else
		printf("Read a message from %s: %s\n", DEVICE_NODE, user_buf);
}

void write_data_led_dev()
{
	int ret = 0;
	char user_buf[BUFFER];
	printf("Enter your message: ");
	scanf("%s", user_buf);
	int fd = open_led_dev();
	ret = write(fd, user_buf, strlen(user_buf) + 1);
	close_led_dev(fd);
	if (ret < 0)
		printf("Could not write the message to %s\n", DEVICE_NODE);
	else
		printf("Wrote the message to %s\n", DEVICE_NODE);
}

int main()
{
	int ret = 0;
	char option = 'q';
	int fd = -1;
	printf("Select below options:\n");
   	printf("\to (to open a device node)\n");
    	printf("\tc (to close the device node)\n");
	printf("\tr (to read data from device node)\n");
	printf("\tw (to write data to device node)\n");
    	printf("\tq (to quit the application)\n");
   	while (1) {
        	printf("Enter your option: ");
        	scanf(" %c", &option);
        	switch (option) {
            		case 'o':
                		if (fd < 0)
                    			fd = open_led_dev();
                		else
                    			printf("%s has already opened\n", DEVICE_NODE);
                		break;
			case 'r':
				read_data_led_dev();
				break;	
			case 'w':
				write_data_led_dev();
				break;
		
			case 'c':
                		if (fd > -1)
                    			close_led_dev(fd);
                		else
                    			printf("%s has not opened yet! Can not close\n", DEVICE_NODE);
                		fd = -1;
                		break;
            		case 'q':
                		if (fd > -1)
                    			close_led_dev(fd);
                		printf("Quit the application. Good bye!\n");
                		return 0;
            		default:
                		printf("invalid option %c\n", option);
                	break;
        	}
    	}
	return 0;
}
