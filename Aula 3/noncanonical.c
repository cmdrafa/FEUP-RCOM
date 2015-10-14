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
		int c, res, fd;						//Variable initiation
		struct termios oldtio,newtio;		//Variables with the new and old Port configurations
		unsigned char UA[UALENGTH];			//Variable to send Flags

		//************* If clause to check if received arguments are correct ********************
		if ( (argc < 2) || ((strcmp("/dev/ttyS0", argv[1])!=0) && (strcmp("/dev/ttyS4", argv[1])!=0) )) {
			printf("Usage:\tWrong serial Port\n\tex: Serial /dev/ttyS1\n");
			exit(1);
		}
		//***************************************************************************************
	
		//**** Open serial port for r/w and not for controlling tty to avoid getting killed *****
		fd = open(argv[1], O_RDWR | O_NOCTTY );
		
		//****** Check errors when opening port ******
		if (fd <0) {
			perror(argv[1]); exit(-1);
		}
		//********************************************

		//********* Store old configuration of the port and set the new one **************
		tcgetattr(fd,&oldtio); // save port settings 
		bzero(&newtio, sizeof(newtio));
		newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
		newtio.c_iflag = IGNPAR;
		newtio.c_oflag = 0;
		// set mode for input
		newtio.c_lflag = 0;
		newtio.c_cc[VTIME] = 0.1; 
		newtio.c_cc[VMIN] = 1; 
		tcflush(fd, TCIFLUSH);
		tcsetattr(fd,TCSANOW,&newtio);
		printf("New termios structure set\n");
		//********************************************************************************
		
		//**** While that controls the reception of the info as well as the possible errors ****
		char response[5]; 		

		unsigned int stateMachine = 0;
		while (stateMachine < 5) { // state machine control 
			char readChar;						
			res = read(fd,&readChar,1); // returns after 1 char input 
			switch (stateMachine) {
				case 0:
					if (readChar == FLAG) {
						response[stateMachine] = readChar;
						stateMachine = 1;
					}					
					break;
				case 1:
					switch(readChar) {
						case FLAG:
							break;
						case A:
							response[stateMachine] = readChar;
							stateMachine = 2;
							break;
						default:
							stateMachine = 0;
							break;
					}
					break;
				case 2:
					switch(readChar) {
						case FLAG:
							stateMachine = 1;
							break;
						case C:
							response[stateMachine] = readChar;
							stateMachine = 3;			
							break;
						default:
							stateMachine = 0;
							break;
					}
					break;
				case 3:
					switch(readChar) {
						case FLAG:
							stateMachine = 1;
							break;
						case BCC:
							response[stateMachine] = readChar;
							stateMachine = 4;
							break;
						default:
							stateMachine = 0;
							break;
					}
					break;
				case 4:
					switch(readChar) {
						case FLAG:
							response[stateMachine] = readChar;
							printf("Read correct Flags\n");
							printf("0x%x, 0x%x, 0x%x, 0x%x, 0x%x.\n", response[0], response[1], response[2], response[3], response[4]);
							stateMachine = 5;
							break;
						default:
							stateMachine = 0;
							break;
					}
					break;
			}			
		}
		//**************************************************************************************
		
		//**************************************
		//****** Prepare UA response************
		//**************************************
		UA[0] = FLAG;
		UA[1] = A;
		UA[2] = C;
		UA[3] = BCC;
		UA[4] = FLAG;
		//**************************************		

		tcflush(fd, TCOFLUSH);
		
		//*** Respond to the sender *******	
		res = write(fd, UA, UALENGTH);
		//*** Wait before restoring the port settings ***
		sleep(1);
		tcsetattr(fd,TCSANOW,&oldtio);
		close(fd);
		return 0;
	}


