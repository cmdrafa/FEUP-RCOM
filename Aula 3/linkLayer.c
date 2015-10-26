	#include "linkLayer.h"

	int * flagPointer;
	int * countPointer;
	
	linkLayer ll;
	
	//*********************** Function to send the message **************************
	void writeMsg(applicationLayer * al, char aFlag, char cFlag) {
		printf("\nSending!");
		tcflush((*al).fd, TCOFLUSH); // Clean output buffer

		//******* Setting the flags to send **********
		unsigned char SET[SETLEN];
		SET[0] = FLAG;
		SET[1] = aFlag;
		SET[2] = cFlag;
		SET[3] = SET[1] ^ SET[2]; //BCC
		SET[4] = FLAG;
		//********************************************

		write(al->fd, SET, SETLEN); //Sending the info
		printf("\nInformation sent...");
	}
	//*******************************************************************************

	//************** Read the response of the receiver ******************************
	int readResponse(applicationLayer * al, int * flag, char aFlag, char cFlag) {

		int res;
		char response[5];
		
		
		//************* While that controls the reading of the response of the receiver *********
		unsigned int stateMachine = 0;
		while (stateMachine < 5) { // state machine control
			char readChar;
			res = read((*al).fd,&readChar,1); // returns after 1 char input
			
			if (!*flag && (res == 1)) {
				switch (stateMachine) {
					case 0:
						if (readChar == FLAG) {
							response[stateMachine] = readChar;
							stateMachine = 1;
						}
						break;
					case 1:
						if (readChar == FLAG) {
							break;
						} else if (readChar == aFlag) {
							response[stateMachine] = readChar;
							stateMachine = 2;
							break;
						} else {
							stateMachine = 0;
							break;
						}
					case 2:
						if (readChar == FLAG) {
							stateMachine = 1;
							break;
						} else if (readChar == cFlag) {
							response[stateMachine] = readChar;
							stateMachine = 3;
							break;
						} else {
							stateMachine = 0;
							break;
						}
					case 3:
					  if (readChar == FLAG) {
							stateMachine = 1;
							break;
						} else if (readChar == (aFlag^cFlag)) {
							response[stateMachine] = readChar;
							stateMachine = 4;
							break;
						} else {
							stateMachine = 0;
							break;
						}
					case 4:
						switch(readChar) {
							case FLAG:
								response[stateMachine] = readChar;
								printf("\nCorrect response read: ");
								printf("\n                              0x%x, 0x%x, 0x%x, 0x%x, 0x%x.", response[0], response[1], response[2], response[3], response[4]);
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
	
	//************** Read the response of the receiver ******************************
	int readInfo(applicationLayer * al, int * flag, char * buffer) {

		int res;
		int end = FALSE;
		char * bufferP = buffer;
		int c = 0;

		//************* While that controls the reading of the response of the receiver *********
		while (end == FALSE) { // state machine control
			char readChar;
			res = read((*al).fd,&readChar,1); // returns after 1 char input

			if (!*flag && (res == 1)) {
				*bufferP = readChar;

				if (c > 3) {
					if ((*bufferP) == FLAG) {
						end = TRUE;
					}
				}		
				c++;
				bufferP++;
			}
			else if (*flag)
				break;
		}
		//***************************************************************************************
		
		if(*flag)
			return -1;

		return c;
	}

	//************** Function to configure the port and store the old configurations **************
	void configure(applicationLayer * al, linkLayer * ll, struct termios * oldtio) {

		//Initialized variable to set the new config to the port
		struct termios newtio;
		printf("\nConfiguration started!");

		//Open the serial port
		al->fd = open((*ll).port, O_RDWR | O_NOCTTY | O_NONBLOCK );

		//Check for errors of opening the port
		if (al->fd <0) {
			perror((*ll).port);
			exit(-1);
		}

		if ( tcgetattr(al->fd,oldtio) == -1) { // save current port settings
			perror("tcgetattr");
			exit(-1);
		}

		printf("\nOld config saved...");

		//***** Set the new configuration of the port *******************
		bzero(&newtio, sizeof(newtio));
		newtio.c_cflag = (*ll).baudRate | CS8 | CLOCAL | CREAD;
		newtio.c_iflag = IGNPAR;
		newtio.c_oflag = OPOST;
		// set input mode (non-canonical, no echo,...)
		newtio.c_lflag = 0;
		newtio.c_cc[VTIME] = 0.1;
		newtio.c_cc[VMIN] = 1; // blocking read until 1 chars received

		printf("\nSaving new config! ");

		if ( tcsetattr(al->fd,TCSANOW,&newtio) == -1)
		{
			perror("tcsetattr");
			exit(-1);
		}
		//***************************************************************

		printf("\nConfiguration set");
	}
	//*********************************************************************************************

	//************* Reset the serial port configuration *****************
	void resetConfiguration(applicationLayer * al, struct termios * oldtio) {
		printf("\nRestoring default config");

		//Sleep before reseting the configuration to prevent errors in communication
		sleep(1);
		if ( tcsetattr((*al).fd,TCSANOW,oldtio) == -1)
		{
			perror("tcsetattr");
			exit(-1);
		}

		close((*al).fd);
	}
	//*******************************************************************

	//********** Function to trigger alarm ***********
	void triggerAlarm() {
		*flagPointer = TRUE;
		*countPointer = *countPointer + 1;
		printf("\nTimeout Expired: %ds", TIMEOUT);
	}
	//************************************************

	int ll_open(int * flag, int * stop, int * count, applicationLayer * al, linkLayer * ll, struct termios * oldtio) {

		printf("\n----------------------------------------------------\nStarted ll_open()");

		configure(al, ll, oldtio);

		flagPointer = flag;
		countPointer = count;

		//*********** While cycle to control the sending of the message **************
		if ((*al).status == 'W') {
			printf("\nThis is the sender...");

			tcflush((*al).fd, TCIFLUSH);
			while(*count < ATTEMPTS) {

				if(&flag) {
					alarm(TIMEOUT);

					printf("\nAttempts remaining: %d ", (ATTEMPTS - *count - 1));

					writeMsg(al, A_1, C_SET);
					tcflush((*al).fd, TCIFLUSH);
					*flag = FALSE;
					printf("\nwaiting...");
					if(readResponse(al, flag, A_1, C_UA) == 0) {
						printf("\n-> Response received!");
						break;
					}
				}
			}
			//****************************************************************************
		}
		else if ((*al).status == 'R') {
			printf("\nThis is the receiver");
			int flagT = FALSE;
			tcflush((*al).fd, TCIFLUSH);
			while (readResponse(al, &flagT, A_1, C_SET) != 0) { continue; }
			writeMsg(al, A_1, C_UA);
		}

		printf("\n----------------------------------------------------\nFinished ll_open()");
	}

	int ll_close(int * flag, int * stop, int * count, applicationLayer * al, linkLayer * ll, struct termios * oldtio) {

		printf("\n----------------------------------------------------\nStarted ll_close()");

		flagPointer = flag;
		countPointer = count;
		
		*flagPointer = FALSE;

		//*********** While cycle to control the sending of the message **************
		if ((*al).status == 'W') {
			printf("\nWill send DISC...");

			tcflush((*al).fd, TCIFLUSH);
			while(*count < ATTEMPTS) {

				if(&flag) {
					alarm(TIMEOUT);

					printf("\nAttempts remaining: %d ", (ATTEMPTS - *count - 1));
					writeMsg(al, A_1, C_DISC);
					tcflush((*al).fd, TCIFLUSH);
					*flag = FALSE;
					printf("\nwaiting...");
					if(readResponse(al, flag, A_2, C_DISC) == 0) {
						printf("\n-> Response received!");
						break;
					}
				}
			}

			printf("\nSending last message (C_UA)");
			writeMsg(al, A_2, C_UA);
			//****************************************************************************
		}
		else if ((*al).status == 'R') {
			printf("\nWill receive DISC and respond the same\n");
			int flagT = FALSE;
			tcflush((*al).fd, TCIFLUSH);
			while (readResponse(al, &flagT, A_1, C_DISC) != 0) { continue; }
			writeMsg(al, A_2, C_DISC);
			while (readResponse(al, &flagT, A_2, C_UA) != 0) { continue; }
		}

		resetConfiguration(al, oldtio);

		printf("\n----------------------------------------------------\nFinished ll_close()");
	}
	
	int llread(applicationLayer * al, linkLayer * ll, char ** buffer) {
		int flagT = FALSE;
		tcflush((*al).fd, TCIFLUSH);
		
		char * buffer_2 = malloc(sizeof(char) * MAX_SIZE * 2);
		
		int nRead = -1;
		while (nRead < 0) {
			nRead = readInfo(al, &flagT, buffer_2);
		}
		
		int sizeOfInfoRead = unStuff(buffer_2, buffer);
		
		sizeOfInfoRead = removeFrameHeaderAndTrailer(buffer, sizeOfInfoRead);
		
		if ((*ll).sequenceNumber == 0) {
		  writeMsg(al, A_1, C_RR_1);
		  (*ll).sequenceNumber = 1;
		} else {
		  writeMsg(al, A_1, C_RR_0);
		  (*ll).sequenceNumber = 0;
		}
		
		free(buffer_2);
		return sizeOfInfoRead;
	}
	
	int llwrite(int * stop, applicationLayer * al, linkLayer * ll, char * buffer, int length) {
		
		//Fill the toSend char array
		char * toS = malloc(sizeof(char) * (length + 6));
		char toSend[length + 6];
		toSend[0] = FLAG;
		toSend[1] = A_1;
		
		if ((*ll).sequenceNumber == 0) {
			toSend[2] = C_0;
		}
		else {
			toSend[2] = C_1;
		}
		
		toSend[3] = toSend[1] ^ toSend[2];
		
		unsigned int j = 0;
		
		while (j < length) {			
			toSend[j + 4] = *buffer;
			buffer++;
			j++;
		}
		
		toSend[length + 4] = toSend[3];
		toSend[length + 5] = FLAG;
		
		strcpy(toS, toSend);
		
		//Finished filling the char array
		
		int bufSize = 0;
		char *toSendStuffed = stuff(toSend, ((length + 1) * 2) + 5, &bufSize);
		
		int asd = 0;
		char * dd = toSendStuffed;
		while (asd < bufSize) {
			printf("\n0x%x", *dd);
			dd++;
			asd++;
		}
		
		*countPointer = 0;
		//*********** While cycle to control the sending of the message **************
		tcflush((*al).fd, TCIFLUSH);
		while(*countPointer < (*ll).numTransmissions) {
			if(&flagPointer) {
				alarm(TIMEOUT);
				printf("\nAttempts remaining: %d ", (ATTEMPTS - *countPointer - 1));
				tcflush((*al).fd, TCOFLUSH); // Clean output buffer
				write(al->fd, toSendStuffed, bufSize); //Sending the info
	
				//*******************************************
				tcflush((*al).fd, TCIFLUSH);
				*flagPointer = FALSE;
				
				//TODO - test the response of the buffer
				
				printf("\nactual seqnum: %x\n", (*ll).sequenceNumber);
				int resp = readSenderResponse(al, ll);
				if(resp == 0) {
					(*ll).sequenceNumber = 0;
					printf("\n-> Correct Response received!");
					break;
				}
				else if(resp == 1) {
					(*ll).sequenceNumber = 1;
					printf("\n-> Correct Response received!");
					break;
				}
				else if (resp == -2) {
					(*ll).sequenceNumber == 0;
					break;
				}
				else if (resp == -3) {
					(*ll).sequenceNumber = 1;
					break;
				}
				//*******************************************
			}
		}
		free(toSendStuffed);
		free(toS);
		//****************************************************************************
	}
	
	//************** Read the response of the receiver ******************************
	int readSenderResponse(applicationLayer * al, linkLayer * ll) {

		int res;
		char response[5];

		//************* While that controls the reading of the response of the receiver *********
		unsigned int stateMachine = 0;
		while (stateMachine < 5) { // state machine control
			char readChar;
			res = read((*al).fd,&readChar,1); // returns after 1 char input

			if (!*flagPointer && (res == 1)) {
							printf("\nTrying to read charRead: 0x%x\n", readChar);
				switch (stateMachine) {
					case 0:
						if (readChar == FLAG) {
							response[stateMachine] = readChar;
							stateMachine = 1;
						}
						break;
					case 1:
						if (readChar == FLAG) {
							break;
						} else if (readChar == A_1) {
							response[stateMachine] = readChar;
							stateMachine = 2;
							break;
						} else {
							stateMachine = 0;
							break;
						}
					case 2:
						if (readChar == FLAG) {
							stateMachine = 1;
							break;
						} else if ((readChar == C_RR_0 && (*ll).sequenceNumber == 1) || (readChar == C_RR_1 && (*ll).sequenceNumber == 0)) {
							response[stateMachine] = readChar;
							stateMachine = 3;
							break;
						} else if ((readChar == C_REJ_0) || (readChar == C_REJ_1)) {
							response[stateMachine] = readChar;
							stateMachine = 3;
							break;
						} else {
							stateMachine = 0;
							break;
						}
					case 3:
					  if (readChar == FLAG) {
							stateMachine = 1;
							break;
						} else if (readChar == (response[1]^response[2])) {
							response[stateMachine] = readChar;
							stateMachine = 4;
							break;
						} else {
							stateMachine = 0;
							break;
						}
					case 4:
						switch(readChar) {
							case FLAG:
								response[stateMachine] = readChar;
								printf("\nCorrect response read: ");
								printf("\n                              0x%x, 0x%x, 0x%x, 0x%x, 0x%x.", response[0], response[1], response[2], response[3], response[4]);
								stateMachine = 5;
								break;
							default:
								stateMachine = 0;
								break;
						}
						break;
				}
			}
			else if (*flagPointer)
				break;
		}

		//***************************************************************************************

		if(*flagPointer)
			return -1;
		
		if (response[2] == C_REJ_0)
			return -2;
		else if (response[2] == C_REJ_1)
			return -3;
		else if (response[2] == C_RR_0) {
			return 0;
		}
		else if (response[2] == C_RR_1) {
			return 1;
		}
	}
	
	char * stuff(char * unStuffed, int totalLength, int * bufSize) {
		char * toRet = malloc(sizeof(char) * totalLength);
		int unStuffedLength = ((totalLength - 5) / 2) + 5;
		int i = 4;
		int j = 0;
		while (j < 4) {
			toRet[j] = unStuffed[j];
			j++;
		}
		while (i < (unStuffedLength - 1)) {
			if (unStuffed[i] == FLAG) {
				toRet[j] = ESCAPE;
				j++;
				toRet[j] = FLAG_EXC;
			}
			else if (unStuffed[i] == ESCAPE) {
				toRet[j] = ESCAPE;
				j++;
				toRet[j] = ESCAPE_EXC;	
			}
			else {
				toRet[j] = unStuffed[i];
			}
			i++;
			j++;
		}
		toRet[j] = FLAG;
		j++;
		
		*bufSize = j;
		return toRet;
	}
	
	int unStuff(char * unstuffed, char ** stuffed) {
		char * temp = malloc(sizeof(char) * MAX_SIZE * 2);
		
		int stuffedC = 0;
		int end = FALSE;
		while (end == FALSE) {

			if (((*unstuffed) == ESCAPE) && ((*(unstuffed + 1)) == FLAG_EXC)) {
				(*(temp + stuffedC)) = FLAG;
				unstuffed++;
			}
			else if (((*unstuffed) == ESCAPE) && ((*(unstuffed + 1)) == ESCAPE_EXC)) {
				(*(temp + stuffedC)) = ESCAPE;
				unstuffed++;
			}
			else {
				(*(temp + stuffedC)) = (*unstuffed);	
			}

			if ((stuffedC > 2) && (*(temp + stuffedC) == FLAG)) {
				end = TRUE;	
			}
						
			unstuffed++;
			stuffedC++;
		}
		
		*stuffed = malloc(sizeof(char) * stuffedC);
		int i = 0;
		while (i < stuffedC) {
		  *(*stuffed + i) = *(temp + i);
		  i++;
		}
		free(temp);
				
		return stuffedC;
	}

	int removeFrameHeaderAndTrailer(char ** buffer, int sizeOfInfoRead) {
	  char * finalB = malloc(sizeof(char) * (sizeOfInfoRead - 6));
	  
	  int counter = 4;
	  	  
	  strncpy(finalB, (*buffer + counter), sizeOfInfoRead - 6);
	  
	  free(*buffer);
	  *buffer = finalB;

	  return sizeOfInfoRead - 6;
	}