	#include <sys/types.h>
	#include <sys/stat.h>
	#include <fcntl.h>
	#include <termios.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <unistd.h>
	#include <signal.h>

     
	#define BAUDRATE B9600
	#define MODEMDEVICE "/dev/ttyS0"
	#define _POSIX_SOURCE 1 // POSIX compliant source 
	#define FALSE 0
	#define TRUE 1

	#define FLAG 0x7e
	#define A 0x03
	#define C 0x03
	#define SETLEN 5
	#define TIMEOUT 3
	#define ATTEMPTS 3

	volatile int STOP = FALSE;
	int flag = 1,count = 0;


	void writeMsg(int * fd) {
		printf("Sending!");
		tcflush(*fd, TCOFLUSH); // Clean output buffer
		unsigned char SET[SETLEN];
		SET[0] = FLAG;
		SET[1] = A;
		SET[2] = C;
		SET[3] = SET[1] ^ SET[2]; //BCC
		SET[4] = FLAG;
		write(*fd, SET, SETLEN);
		printf("...\n");
	}

	int readResponse(int * fd) {
		
		// READ RESPONSE
		
		int res;
		char buf[SETLEN];
		char f[SETLEN];
		tcflush(*fd, TCIFLUSH);
		int i = 0;
	
		printf("Reading!");
		while (STOP==FALSE && !flag) // input 
		{
			res = read(*fd,buf,1); // returns after 1 chars have been input 
		
			if(res != 0) {
				f[i] = buf[0];
				if(f[i] == FLAG && i!=0) {
					STOP = TRUE;
				}
				else {
					i++;
				}
			}
		}
	
		if(flag) return -1;
	
		if(f[3] != (f[1]^f[2]))
		{
			printf("Error on BCC");
			return -1;
		}
		printf("...\n");
		printf("%x, %x, %x, %x, %x\n", f[0], f[1], f[2],f[3],f[4]);

		return 0;
	}

	void configure(int * fd, char * serial_port, struct termios * oldtio) {
	
		struct termios newtio;
		printf("Configuration started!");
		*fd = open(serial_port, O_RDWR | O_NOCTTY );

		if (*fd <0) {perror(serial_port); exit(-1); }

		printf("...\n");

		if ( tcgetattr(*fd,oldtio) == -1) { // save current port settings 
			perror("tcgetattr");
			exit(-1);
		}

		printf("Old config saved...\n");

		bzero(&newtio, sizeof(newtio));
		newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
		newtio.c_iflag = IGNPAR;
		newtio.c_oflag = OPOST;
		// set input mode (non-canonical, no echo,...) 
		newtio.c_lflag = 0;
		newtio.c_cc[VTIME] = 0.1;
		newtio.c_cc[VMIN] = 1; // blocking read until 1 chars received 

		printf("Saving new config! ");

		if ( tcsetattr(*fd,TCSANOW,&newtio) == -1)
		{
			perror("tcsetattr");
			exit(-1);
		}

		printf("...\n");
	}

	void resetConfiguration(int * fd, struct termios * oldtio) {
		printf("Restoring default config");

		if ( tcsetattr(*fd,TCSANOW,oldtio) == -1)
		{
			perror("tcsetattr");
			exit(-1);
		}

		printf("...\n");
		close(*fd);
	}
	
	void triggerAlarm() {
	
		flag = 1;
		count++;
		printf("Timeout Expired: %ds\n", TIMEOUT);
	}

	int main(int argc, char** argv)
	{
		int fd;
		struct termios oldtio;

		if ( (argc < 2) ||
		((strcmp("/dev/ttyS0", argv[1])!=0) &&
		(strcmp("/dev/ttyS1", argv[1])!=0) &&
		(strcmp("/dev/ttyS4", argv[1])!=0)))
		{
			printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
			exit(1);
		}

		configure(&fd, argv[1], &oldtio);
		(void) signal(SIGALRM, triggerAlarm); // instala rotina que atende interrupcao
		// Resend

		while(count < ATTEMPTS) {

			if(flag) {
				alarm(TIMEOUT);
				printf("\nAttempts remaining: %d \n", (ATTEMPTS - count - 1));
				writeMsg(&fd);
				flag = 0;
				if(readResponse(&fd) == 0) {
					printf("-> Response received!\n");
					break;
				}
			}
		}

		resetConfiguration(&fd, &oldtio);
		return 0;
	}


