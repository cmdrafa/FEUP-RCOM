#include "linkLayer.h"

int * flagPointer;
int * countPointer;

linkLayer ll;

//*********************** Function to send the message **************************
void writeMsg(applicationLayer * al, char aFlag, char cFlag) {
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

	tcflush((*al).fd, TCIFLUSH);
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
		while(*count < (*ll).numTransmissions) {

			if(&flag) {
				alarm(TIMEOUT);

				//printf("\nAttempts remaining: %d ", (ATTEMPTS - *count - 1));

				tcflush((*al).fd, TCOFLUSH);

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
		if (*count == (*ll).numTransmissions)
			return -1;
	}
	else if ((*al).status == 'R') {
		printf("\nThis is the receiver");
		int flagT = FALSE;
		tcflush((*al).fd, TCIFLUSH);
		while (readResponse(al, &flagT, A_1, C_SET) != 0) { continue; }
		writeMsg(al, A_1, C_UA);
	}

	printf("\n----------------------------------------------------\nFinished ll_open()");
	return 0;
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
		while(*count < (*ll).numTransmissions) {

			if(&flag) {
				alarm(TIMEOUT);

				//printf("\nAttempts remaining: %d ", (ATTEMPTS - *count - 1));
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
		if (*count == (*ll).numTransmissions)
			return -1;

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

	char * buffer_2 = malloc(sizeof(char) * ((((*ll).packSize - 6) * 2) + 16));

	int nRead = -1;
	while (nRead < 0) {
		nRead = readInfo(al, &flagT, buffer_2);
	}

	int sizeOfInfoRead = unStuff(buffer_2, buffer);

	if (*(*buffer + 2) == C_0) {
		printf("\nSequence number received was: 0");
	}
	else if (*(*buffer + 2) == C_1) {
		printf("\nSequence number received was: 1");
	}

	printf(" | Sequence number asked was: %d", (*ll).sequenceNumber);

	printf("\nA: 0x%x", *(*buffer + 1));
	printf("\nC: 0x%x", *(*buffer + 2));
	printf("\nBcc1: 0x%x", *(*buffer + 3));
	printf("\nBcc2: 0x%x", *(*buffer + sizeOfInfoRead - 2));

	int length = sizeOfInfoRead - 6;
	int q = 4;
	char bcc2 = 0x0;
	while (q < (length + 4)) {
	  bcc2 = bcc2 ^ *(*buffer + q);
	  q++;
	}

	printf("\nBcc2-2: 0x%x", bcc2);

	if ((*(*buffer + 3) != (*(*buffer + 1) ^ *(*buffer + 2))) || (*(*buffer + sizeOfInfoRead - 2) != bcc2)) {
		if ((*ll).sequenceNumber == 0) {

			printf("\n****\n2nd ERROR receiving 1, wanted 0, sending REJ 0\n****");
			writeMsg(al, A_1, C_REJ_0);
			free(buffer_2);
			return -5;
		} else if ((*ll).sequenceNumber == 1) {
			printf("\n****\n2nd ERROR receiving 0, wanted 1, sending REJ 1\n****");
			writeMsg(al, A_1, C_REJ_1);
			free(buffer_2);
			return -4;
		}
	}
	else if ((*ll).sequenceNumber == 0 && *(*buffer + 2) == C_0) {
		writeMsg(al, A_1, C_RR_1);
		(*ll).sequenceNumber = 1;
	} else if ((*ll).sequenceNumber == 1 && *(*buffer + 2) == C_1) {
		writeMsg(al, A_1, C_RR_0);
		(*ll).sequenceNumber = 0;
	} else if ((*ll).sequenceNumber == 0 && *(*buffer + 2) == C_1) {
		writeMsg(al, A_1, C_REJ_0);
		printf("\n****\nERROR receiving 1, wanted 0, sending REJ 0\n****");
		free(buffer_2);
		return -2;
	} else if ((*ll).sequenceNumber == 1 && *(*buffer + 2) == C_0) {
		writeMsg(al, A_1, C_REJ_1);
		printf("\n****\nERROR receiving 0, wanted 1, sending REJ 1\n****");
		free(buffer_2);
		return -3;
	}

	free(buffer_2);

	sizeOfInfoRead = removeFrameHeaderAndTrailer(buffer, sizeOfInfoRead);

	return sizeOfInfoRead;
}

int llwrite(int * stop, applicationLayer * al, linkLayer * ll, char * buffer, int length) {

	//Fill the toSend char array
	char * toS = malloc(sizeof(char) * (length + 6));
	char * toSend = malloc(sizeof(char) * (length + 6));
	*toSend = FLAG;
	*(toSend + 1) = A_1;

	if ((*ll).sequenceNumber == 0) {
		*(toSend + 2) = C_0;
	}
	else {
		*(toSend + 2) = C_1;
	}

	*(toSend + 3) = *(toSend + 1) ^ *(toSend + 2);

	unsigned int j = 0;
	while (j < length) {
		*(toSend + j + 4) = *buffer;
		buffer++;
		j++;
	}

	*(toSend + length + 4) = 0x0;
	int q = 4;
	while (q < (length + 4)) {
	  *(toSend + length + 4) = *(toSend + length + 4) ^ *(toSend + q);
	  q++;
	}

	*(toSend + length + 5) = FLAG;

	strcpy(toS, toSend);

	printf("\nA: 0x%x", *(toSend + 1));
	printf("\nC: 0x%x", *(toSend + 2));
	printf("\nBcc1: 0x%x", *(toSend + 3));
	printf("\nBcc2: 0x%x", *(toSend + length + 4));

	//Finished filling the char array

	int bufSize = 0;

	char *toSendStuffed = stuff(&toSend, length + 6, &bufSize);

	*countPointer = 0;
	//*********** While cycle to control the sending of the message **************
	int rr = FALSE;
	while(*countPointer < (*ll).numTransmissions && rr == FALSE) {
		printf("\nSending Packet with seqNum: 0x%x", *(toSendStuffed + 2));
		if(&flagPointer) {
			alarm(TIMEOUT);
			//printf("\nAttempts remaining: %d ", (ATTEMPTS - *countPointer - 1));

			tcflush((*al).fd, TCOFLUSH);
			write(al->fd, toSendStuffed, bufSize); //Sending the info
			//*******************************************

			*flagPointer = FALSE;
			int resp = readSenderResponse(al, ll);
			if(resp == 0) {
				(*ll).sequenceNumber = 0;
				printf("\n-> Received RR | Receiver asking for packet 0");
				rr = TRUE;
			}
			else if(resp == 1) {
				(*ll).sequenceNumber = 1;
				printf("\n-> Received RR | Receiver asking for packet 1");
				rr = TRUE;
			}
			else if (resp == -2) {
				printf("\n-> Received REJ | Receiver asking for packet 0");
			}
			else if (resp == -3) {
				printf("\n-> Received REJ | Receiver asking for packet 1");
			} else {
			  printf("\nTIMEOUT - did not read response");
			}
			printf("\n");
			usleep(0.5 * 1000000);
			//*******************************************
		} else {
			printf("\nTIMEOUT expired");
		}

	}
	if (*countPointer == (*ll).numTransmissions) {
	  free(toSendStuffed);
	  free(toS);
	  free(toSend);
	  return -1;
	}
	free(toSendStuffed);
	free(toS);
	free(toSend);
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
				} else if ((readChar == C_REJ_0) || (readChar == C_REJ_1) || (readChar == C_RR_0) || (readChar == C_RR_1)) {
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

	if (response[2] == C_REJ_0 && (stateMachine == 5))
	return -2;
	else if (response[2] == C_REJ_1 && (stateMachine == 5))
	return -3;
	else if (response[2] == C_RR_0 && (stateMachine == 5)) {
		return 0;
	}
	else if (response[2] == C_RR_1 && (stateMachine == 5)) {
		return 1;
	} else {
		return -1;
	}
}

char * stuff(char ** unStuffed, int unStuffedLength, int * bufSize) {
	int totalLength = ((unStuffedLength + 1) * 2) + 5;

	char * toRet = malloc(sizeof(char) * totalLength);
	int i = 4;
	int j = 0;
	while (j < 4) {
		*(toRet + j) = *(*unStuffed + j);
		j++;
	}
	while (i < (unStuffedLength - 1)) {
		if (*(*unStuffed + i) == FLAG) {
			*(toRet + j) = ESCAPE;
			j++;
			*(toRet + j) = FLAG_EXC;
		}
		else if (*(*unStuffed + i) == ESCAPE) {
			*(toRet + j) = ESCAPE;
			j++;
			*(toRet + j) = ESCAPE_EXC;
		}
		else {
			*(toRet + j) = *(*unStuffed + i);
		}
		i++;
		j++;
	}
	*(toRet + j) = FLAG;
	j++;

	*bufSize = j;
	return toRet;
}

int unStuff(char * unstuffed, char ** stuffed) {
	char * temp = malloc(sizeof(char) * ((((*ll).packSize - 6) * 2) + 16));

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


		if ((stuffedC > 2) && (*(unstuffed + 1) == FLAG)) {
			unstuffed++;
			stuffedC++;
			*(temp + stuffedC) = FLAG;
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

	printf("\nSize of distuffed: %d\n", stuffedC);

	return stuffedC;
}

int removeFrameHeaderAndTrailer(char ** buffer, int sizeOfInfoRead) {
	char * finalB = malloc(sizeof(char) * (sizeOfInfoRead - 6));

	int counter = 4;
	int i = 0;
	while (i < sizeOfInfoRead - 6) {
		*(finalB + i) = *(*buffer + counter);
		i++;
		counter++;
	}

	free(*buffer);
	*buffer = finalB;

	return sizeOfInfoRead - 6;
}
