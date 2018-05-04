	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <unistd.h>
	#include <fcntl.h>
	#include <termios.h>
	
	int main(int argc, char* argv[]) {
	struct termios serial;
	char buffer[255];
	
	int fd = open("/dev/ttyS0", O_RDWR | O_NOCTTY | O_NDELAY);
	
	if (fd == -1) {
	perror("/dev/ttyS0");
	return -1;
	}
	
	if (tcgetattr(fd, &serial) < 0) {
		perror("Getting configuration");
		return -1;
	}
	
	// Set up Serial Configuration
	serial.c_iflag = 0;
	serial.c_oflag = 0;
	serial.c_lflag = 0;
	serial.c_cflag = 0;
	serial.c_cc[VMIN] = 0;
	serial.c_cc[VTIME] = 0;
	
	serial.c_cflag = B9600 | CS8 | CREAD;
	
	tcsetattr(fd, TCSANOW, &serial); // Apply configuration
	
	FILE *text;
	text= fopen("prueba.txt","r"); //archivo para probar
	int n=0;
	while(1){
		buffer[n]=getc(text);
		if(n>28){
			buffer[30]='\0';
			/*int count = write(uart0_filestream, buffer, 30);
			if (count < 0){
				//printf("UART TX error\n");
			}*/
			int wcount = write(fd, buffer, 30);
			if (wcount < 0){
				perror("Write");
				return -1;
			}
			else {
				printf("Sent %d characters\n", wcount);
			}
			n=0;
			buffer[0]='\0';	
		}
		else if(buffer[n]=='q'){
			//buffer[n+1]='\0';
			int wcount = write(fd, buffer, n+1);
			if (wcount < 0){
				perror("Write");
				return -1;
			}
			else {
				printf("Sent %d characters\n", wcount);
			}
			buffer[wcount]='\0';
			n=0;
			break;
			
		}
		n++;
	}
	
	/*int rcount = read(fd, buffer, sizeof(buffer));
	if (rcount < 0){
	perror("Read");
	return -1;
	}
	else {
	printf("Received %d characters\n", rcount);
	}
	
	buffer[rcount] = '\0';*/
	
	printf("Received: %s\n", buffer);
	
	close(fd);
}
