	#include "ll_open.h"
	
	int * flagPointer;
	int * countPointer;	

	//*********************** Function to send the message **************************
	void writeMsg(int * fd) {
		printf("Sending!");
		tcflush(*fd, TCOFLUSH); // Clean output buffer
		
		//******* Setting the flags to send **********
		unsigned char SET[SETLEN];
		SET[0] = FLAG;
		SET[1] = A;
		SET[2] = C_SET;
		SET[3] = SET[1] ^ SET[2]; //BCC
		SET[4] = FLAG;
		//********************************************

		write(*fd, SET, SETLEN); //Sending the info
		printf("Information sent...\n");
	}
	//*******************************************************************************

	//************** Read the response of the receiver ******************************
	int readResponse(int * fd, int * flag) {
		
		int res;
		int i = 0;
		char response[5];		

		//************* While that controls the reading of the response of the receiver *********
		unsigned int stateMachine = 0;
		while (stateMachine < 5) { // state machine control 
			char readChar;						
			res = read(*fd,&readChar,1); // returns after 1 char input 

			if (!*flag && (res == 1)) {
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
							case C_UA:
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
							case A^C_UA:
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
								printf("Correct response read: ");
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
			else if (*flag)
				break;			
		}		

		//***************************************************************************************
	
		if(*flag)
			return -1;

		return 0;
	}
	
	//************** Function to configure the port and store the old configurations **************
	void configure(int * fd, char * serial_port, struct termios * oldtio) {
		
		//Initialized variable to set the new config to the port
		struct termios newtio;
		printf("Configuration started!");

		//Open the serial port
		*fd = open(serial_port, O_RDWR | O_NOCTTY | O_NONBLOCK );
		
		//Check for errors of opening the port
		if (*fd <0) {
			perror(serial_port);
			exit(-1);
		}

		if ( tcgetattr(*fd,oldtio) == -1) { // save current port settings 
			perror("tcgetattr");
			exit(-1);
		}

		printf("Old config saved...\n");

		//***** Set the new configuration of the port *******************
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
		//***************************************************************		

		printf("Configuration set\n");
	}				
	//*********************************************************************************************
	
	//************* Reset the serial port configuration *****************
	void resetConfiguration(int * fd, struct termios * oldtio) {
		printf("Restoring default config");
		
		//Sleep before reseting the configuration to prevent errors in communication
		sleep(1);
		if ( tcsetattr(*fd,TCSANOW,oldtio) == -1)
		{
			perror("tcsetattr");
			exit(-1);
		}

		close(*fd);
	}
	//*******************************************************************
	
	//********** Function to trigger alarm ***********
	void triggerAlarm() {
		*flagPointer = TRUE;
		*countPointer = *countPointer + 1;
		printf("Timeout Expired: %ds\n", TIMEOUT);
	}
	//************************************************

	int ll_open(int * flag, int * stop, int * count, int fd, char side, char * port, struct termios * oldtio) {
		
		configure(&fd, port, oldtio);

		//*********** While cycle to control the sending of the message **************
		if (side == 'W') {
			printf("\nThis is the sender...\n");

			flagPointer = flag;
			countPointer = count;

			tcflush(fd, TCIFLUSH);
			while(*count < ATTEMPTS) {
	
				if(&flag) {
					alarm(TIMEOUT);

					printf("\nAttempts remaining: %d \n", (ATTEMPTS - *count - 1));
					writeMsg(&fd);
					tcflush(fd, TCIFLUSH);
					*flag = FALSE;
					printf("waiting...\n");
					if(readResponse(&fd, flag) == 0) {
						printf("-> Response received!\n");
						break;
					}
				}
			}
			//****************************************************************************
		}
		else if (side == 'R') {
			printf("\nThis is the receiver\n");

			//**** While that controls the reception of the info as well as the possible errors ****
			char response[5]; 
			int res;		

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
							case C_SET:
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
			unsigned char UA[UALENGTH];			//Variable to send Flags
			UA[0] = FLAG;
			UA[1] = A;
			UA[2] = C_UA;
			UA[3] = A^C_UA;
			UA[4] = FLAG;
			//**************************************		
	
			tcflush(fd, TCOFLUSH);
		
			//*** Respond to the sender *******	
			res = write(fd, UA, UALENGTH);
		}

		resetConfiguration(&fd, oldtio);	
	}


