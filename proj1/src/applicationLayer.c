#include "applicationLayer.h"


int * flag;
int * count;
int * stop;

applicationLayer * al;
linkLayer * ll;

int main(int argc, char** argv) {

	//********************* Check if the arguments are corrected *****************************
	if (argc != 1)
	{
		printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
		exit(1);
	}
	//****************************************************************************************



	al = (applicationLayer *) malloc(sizeof(applicationLayer));
	ll = (linkLayer *) malloc(sizeof(linkLayer));
	fillLinkLayer();

	int firstChoice = -1;
	while (firstChoice < 0) { firstChoice = initialMenu(); }

	if (firstChoice == 1) {	(*al).status = 'W';	}
	else { (*al).status = 'R';	}

	firstChoice = -1;
	while (firstChoice < 0) {	firstChoice = choosePort(); }

	char port[] = "/dev/ttySX\0";
	if (firstChoice == 1) { port[9] = '0'; }
	else { port[9] = '4'; }
	strncpy((*ll).port, port, 11);

	firstChoice = -1;
	while (firstChoice == -1) { firstChoice = chooseBaudrate(); }
	switch(firstChoice) {
	  case 1:
	    (*ll).baudRate = B300;
	    break;
	  case 2:
	    (*ll).baudRate = B600;
	    break;
	  case 3:
	     (*ll).baudRate = B1200;
	    break;
	  case 4:
	     (*ll).baudRate = B1800;
	    break;
	  case 5:
	     (*ll).baudRate = B2400;
	    break;
	  case 6:
	     (*ll).baudRate = B4800;
	    break;
	  case 7:
	     (*ll).baudRate = B9600;
	    break;
	  case 8:
	     (*ll).baudRate = B19200;
	    break;
	  case 9:
	     (*ll).baudRate = B38400;
	    break;
	  case 10:
	     (*ll).baudRate = B57600;
	    break;
	  case 11:
	     (*ll).baudRate = B115200;
	    break;
	  default:
	    printf("\nError in choice of baudrate\n");
	    return -1;
	}

	firstChoice = -1;
	while (firstChoice < 0) { firstChoice = chooseMaxSize(); }
	(*ll).packSize = firstChoice;

	firstChoice = -1;
	while (firstChoice < 0) { firstChoice = chooseTimeout(); }
	(*ll).timeout = firstChoice;

	firstChoice = -1;
	while (firstChoice < 0) { firstChoice = chooseNumTransmissions(); }
	(*ll).numTransmissions = firstChoice;

	count = malloc(sizeof(int));
	flag = malloc(sizeof(int));
	stop = malloc(sizeof(int));
	*count = 0;
	*flag = TRUE;
	*stop = FALSE;

	struct termios oldtio;

	(void) signal(SIGALRM, triggerAlarm); // instala rotina que atende interrupcao

	showInitialInfo(ll, al);

	printf("\n####  Attempting connection...   ####\n");
	if (ll_open(flag, stop, count, ll, &oldtio) < 0) {
		printf("\nError in ll_open\n");
		return -1;
	}
	printf("\n####  Connection established.   ####\n");

	if ((*al).status == 'W') {
		if ((*al).debug == TRUE) {
		printf("\n______________________________Sending control packet 1____________________________________\n");
		}
		if (sendFile() < 0) {
			printf("\nError in sendFile\n");
			return -1;
		}
		if ((*al).debug == TRUE) {
		printf("\n______________________________Sent control packet 1_______________________________________\n");
		}
	}
	else if ((*al).status == 'R') {
		if ((*al).debug == TRUE) {
		printf("\n______________________________Receiving control packet 1____________________________________\n");
		}

		printf("\n");
		if (readFile() < 0) {
			printf("\nError in readFile\n");
			return -1;
		}
		if ((*al).debug == TRUE) {
		printf("\n______________________________Received control packet 1_______________________________________\n");
		}
	}

	printStats(al, ll->stat);

	printf("\n\n####  Terminating connection...   ####\n");
	if(ll_close(flag, stop, count, ll, &oldtio) < 0) {
		printf("\nError in ll_close\n");
		return -1;
	}
	printf("\n####  Connection terminated.   ####\n");

	free(count);
	free(flag);
	free(stop);

	free(al);
	free((*ll).stat);
	free(ll);

	//printf("\nTerminated!\n");

	return 0;
}

Statistics* initStat(Statistics * stats) {
  (*stats).sentMessages = 0;
  (*stats).receivedMessages = 0;
  (*stats).timeouts = 0;
  (*stats).numSentRR = 0;
  (*stats).numReceivedRR = 0;
  (*stats).numSentREJ = 0;
  (*stats).numReceivedREJ = 0;
  return;
}

void fillLinkLayer() {
	(*ll).baudRate = BAUDRATE;
	(*ll).sequenceNumber = 0;
	(*ll).timeout = TIMEOUT;
	(*ll).numTransmissions = ATTEMPTS;
	(*ll).packSize = MAX_PACKET_SIZE + 6;
	(*al).debug = FALSE;

	(*ll).status = (*al).status;

	(*ll).stat = malloc(sizeof(Statistics));
	initStat((*ll).stat);
}

char * createFirstControlPacket(int * packetSize, char ** fileSizeChar) {
	*packetSize = 24;
	char * control = malloc(sizeof(char) * (*packetSize));
	int sizeOfName = 11;
	uint8_t size = (sizeOfName & 0xFF);
	//char con[] = "10 ola.txt18";
	char con[] = "10 pinguim.gif18";
	con[2] = size;
	strcpy(control, con);

	//strncpy(control + 12, *fileSizeChar, 8);
	strncpy(control + 16, *fileSizeChar, 8);

	return control;
}

int sendFile() {
	int packetSize;

	FILE * pfd = fopen("./pinguim.gif", "r");
	int fileSize = getFileSize(pfd);

	printf("\n\nSize of file expected: %d\n\n", fileSize);

	char * fullFile = malloc(sizeof(char) * fileSize);
	char * fullFileStart = fullFile;
	fread(fullFile, fileSize, 1, pfd);
	char * fileSizeChar = malloc(sizeof(char) * 8);
	sprintf(fileSizeChar, "%d", fileSize);

	char * packet_1 = createFirstControlPacket(&packetSize, &fileSizeChar);
	free(fileSizeChar);

	//Sends First control packet
	if ((*al).debug == TRUE) {
	printf("\n_________________________________________________\nFirst Control Packet");
	}
	llwrite(stop, ll, packet_1, packetSize);

	int packetCounter = 0;
	int numberOfPackets = fileSize / (((*ll).packSize - 6) - 4);
	if ((fileSize % (((*ll).packSize - 6) - 4)) > 0)
	numberOfPackets++;
	while (packetCounter < numberOfPackets) {

		if ((*al).debug == TRUE) {
		printf("\n_________________________________________________\nPacket Number: %d", packetCounter);
		}
		char * infoPacket = malloc(sizeof(char) * ((*ll).packSize - 6));
		*infoPacket = '0';
		*(infoPacket + 1) = (char) packetCounter;
		char k = (char) (((*ll).packSize - 6) - 4);
		uint8_t l1 = ((k & 0xFF00) >> 8);
		*(infoPacket + 2) = l1;
		uint8_t l2 = (k & 0x00FF);
		*(infoPacket + 3) = l2;

		int i = 4;
		while (i < ((*ll).packSize - 6) && ((packetCounter*(((*ll).packSize - 6)-4) + (i-4)) < fileSize)) {
			*(infoPacket + i) = *fullFile;
			fullFile++;
			i++;
		}

		if ((*al).debug == TRUE) {
		printf("\nPacket size is: %d\n", i);
		}
		if (llwrite(stop, ll, infoPacket, i) < 0)
		{
		 printf("\nError in llwrite\n");
		 free(packet_1);
		 free(fullFileStart);
		 fclose(pfd);
		 free(infoPacket);

		 printProgressBar((packetCounter*(((*ll).packSize - 6)-4) + (i-4)), fileSize);

		 return -1;
		}

		printProgressBar((packetCounter*(((*ll).packSize - 6)-4) + (i-4)), fileSize);

		free(infoPacket);
		packetCounter++;
		(*ll).stat->sentMessages++;
	}
	printf("\n");

	//Sends Last control Packet
	*packet_1 = '2';
	if ((*al).debug == TRUE) {
	printf("\n_________________________________________________\nLast Control Packet");
	}
	llwrite(stop, ll, packet_1, packetSize);

	free(packet_1);
	free(fullFileStart);
	fclose(pfd);
	return 0;
}

int readFile() {

	char * finalFile;
	char * finalFileToStore;
	char * fileName;
	int fileSize;
	int packetCounter = 0;

	int cn = FALSE;
	while (cn == FALSE) {
		char * packet_1;
		int sizeOfPacket = -1;
		if ((*al).debug == TRUE) {
		printf("\n__________________________________________________________________\nPacket Received\n");
		}
		while (sizeOfPacket < 0) {
			sizeOfPacket = llread( ll, &packet_1);
			if (sizeOfPacket < 0)
				free(packet_1);
		}
		if ((*al).debug == TRUE) {
		printf("\nSize of packet is: %d\n", sizeOfPacket);
		}

		if (sizeOfPacket < 0) {
			free(packet_1);
			continue;
		} else if (*packet_1 == '2') {
			cn = TRUE;

			char * name = malloc(sizeof(char) * 50);
			char path [] = "./received/";
			int i = 0;
			while (i < 11) {
				*(name + i) = path[i];
				i++;
			}
			strcpy(name+i, fileName);

			FILE * pfd = fopen(name, "w");
			fwrite(finalFile, fileSize, 1, pfd);

			fclose(pfd);
		} else if (*packet_1 == '1') {
			getNameAndSizeOfFile(&packet_1, sizeOfPacket, &fileSize, &fileName);

			finalFile = malloc(sizeof(char) * fileSize);
			finalFileToStore = finalFile;
		} else if (*packet_1 == '0') {
			int i = 4;
			while (i < sizeOfPacket) {
				*finalFileToStore = *(packet_1 + i);
				i++;
				finalFileToStore++;
			}

			printProgressBar((packetCounter*(((*ll).packSize - 6)-4) + (i-4)), fileSize);
			packetCounter++;
			ll->stat->receivedMessages++;

		} else {
			free(packet_1);
		}
	}
	printf("\n");

	free(fileName);
	free(finalFile);

	return 0;
}

void getNameAndSizeOfFile(char ** packet_1, int sizeOfPacket, int * fileSize, char ** fileName) {
	int j = *(*packet_1 + 2);
	int i = 3;

	*fileName = malloc(sizeof(char) * (j + 2));

	//Get the name of the file
	while (i < (j + 3)) {
		*(*fileName + (i - 3)) = *(*packet_1 + i);
		i++;
	}
	*(*fileName + (i - 3)) = '\0';

	//Get the size of the file
	//i++;	//Size of the byte that describes the size
	i++;	//Because of the 8 (size of 2 bytes)
	char rest[sizeOfPacket - i];
	j = i;
	j++;


	while (i < sizeOfPacket) {
		rest[i - j] = *(*packet_1 + i);
		i++;
	}

	rest[i - j] = '\0';

	*fileSize = atoi(rest);
}
