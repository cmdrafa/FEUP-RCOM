/*Non-Canonical Input Processing*/
     
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <fcntl.h>
	#include <termios.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <unistd.h>
     
	#define BAUDRATE B9600
	#define _POSIX_SOURCE 1
	
	#define FALSE 0
	#define TRUE 1
     
	#define FLAG 0x7e
	#define A 0x03
	#define C 0x07
	#define BCC A^C
	#define UALENGTH 5

	volatile int STOP=FALSE;

	int main(int argc, char** argv)
	{
		int c, res, fd;
		struct termios oldtio,newtio;
		char buf[1];
		char f[UALENGTH];
		unsigned char UA[UALENGTH];
		if ( (argc < 2) ||
		((strcmp("/dev/ttyS0", argv[1])!=0) &&
		(strcmp("/dev/ttyS4", argv[1])!=0) )) {
		printf("Usage:\tWrong serial Port\n\tex: Serial /dev/ttyS1\n");
		exit(1);
	}

	/*
	Open serial port for r/w and not for controlling tty to avoid getting killed
	*/

	fd = open(argv[1], O_RDWR | O_NOCTTY );

	if (fd <0) {
		perror(argv[1]); exit(-1);
	}

	tcgetattr(fd,&oldtio); /* save port settings */
	bzero(&newtio, sizeof(newtio));
	newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
	newtio.c_iflag = IGNPAR;
	newtio.c_oflag = 0;
	
	/* set input mode*/
	newtio.c_lflag = 0;
	newtio.c_cc[VTIME] = 0.1; /* inter-character timer unused */
	newtio.c_cc[VMIN] = 1; /* blocking read until we receive 1 char*/
	tcflush(fd, TCIFLUSH);
	tcsetattr(fd,TCSANOW,&newtio);
	printf("New termios structure set\n");
	int i = 0;

	while (STOP==FALSE) { /* input */
		res = read(fd,buf,1); /* returns after 1 char input */

		f[i] = buf[0];

		if(f[i] == FLAG && i!=0)
			STOP = TRUE;
		else
			i++;
	}

	UA[0] = FLAG;
	UA[1] = A;
	UA[2] = C;
	UA[3] = BCC;
	UA[4] = FLAG;

	if(f[3] != (f[1]^f[2]))
	{
	 	printf("Error on BCC!");
		exit(1);
	}

	printf("%x, %x, %x, %x, %x\n", f[0], f[1], f[2],f[3],f[4]);
	tcflush(fd, TCOFLUSH);
	sleep(1);
	res = write(fd, UA, UALENGTH);
	sleep(1);
	tcsetattr(fd,TCSANOW,&oldtio);
	close(fd);
	return 0;
	}


