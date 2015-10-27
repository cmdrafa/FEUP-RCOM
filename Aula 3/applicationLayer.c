#include "applicationLayer.h"


int * flag;
int * count;
int * stop;

applicationLayer * al;
linkLayer * ll;

int main(int argc, char** argv) {
	al = malloc(sizeof(applicationLayer));
	ll = malloc(sizeof(linkLayer));
	fillLinkLayer();

	count = malloc(sizeof(int));
	flag = malloc(sizeof(int));
	stop = malloc(sizeof(int));
	*count = 0;
	*flag = TRUE;
	*stop = FALSE;

	struct termios oldtio;
	//********************* Check if the arguments are corrected *****************************
	if ( (argc < 3) || ((strcmp("/dev/ttyS0", argv[1])!=0) && (strcmp("/dev/ttyS1", argv[1])!=0) &&	(strcmp("/dev/ttyS4", argv[1])!=0)))
	{
		printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
		exit(1);
	}
	//****************************************************************************************

	(*al).status = *argv[2];

	strncpy((*ll).port, argv[1], strlen(argv[1]));
	(*ll).port[strlen(argv[1])] = '\0';

	(void) signal(SIGALRM, triggerAlarm); // instala rotina que atende interrupcao

	if (ll_open(flag, stop, count, al, ll, &oldtio) < 0) {
		perror("llopen");
		return -1;
	}

	if ((*al).status == 'W') {
		printf("\n______________________________Sending control packet 1____________________________________\n");

		if (sendFile() < 0) {
			perror("sendFile");
			return -1;
		}

		printf("\n______________________________Sent control packet 1_______________________________________\n");
	}
	else if ((*al).status == 'R') {
		printf("\n______________________________Receiving control packet 1____________________________________\n");

		if (readFile() < 0) {
			perror("readFile");
			return -1;
		}

		printf("\n______________________________Received control packet 1_______________________________________\n");
	}

	if(ll_close(flag, stop, count, al, ll, &oldtio) < 0) {
		perror("llclose");
		return -1;
	}

	free(count);
	free(flag);
	free(stop);

	free(al);
	free(ll);

	printf("\nTerminated!\n");

	return 0;
}

void fillLinkLayer() {
	(*ll).baudRate = BAUDRATE;
	(*ll).sequenceNumber = 0;
	(*ll).timeout = TIMEOUT;
	(*ll).numTransmissions = ATTEMPTS;
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
	//FILE * pfd = fopen("./ola.txt", "r");
	int fileSize = getFileSize(pfd);

	char * fullFile = malloc(sizeof(char) * fileSize);
	char * fullFileStart = fullFile;
	fread(fullFile, fileSize, 1, pfd);
	char * fileSizeChar = malloc(sizeof(char) * 8);
	sprintf(fileSizeChar, "%d", fileSize);

	char * packet_1 = createFirstControlPacket(&packetSize, &fileSizeChar);
	free(fileSizeChar);

	//Sends First control packet
	printf("\n_________________________________________________\nFirst Control Packet");
	llwrite(stop, al, ll, packet_1, packetSize);

	int sentBytes = 0;
	int packetCounter = 0;
	int numberOfPackets = fileSize / (MAX_PACKET_SIZE - 4);
	if ((fileSize % (MAX_PACKET_SIZE - 4)) > 0)
	numberOfPackets++;
	while (packetCounter < numberOfPackets) {
		printf("\n_________________________________________________\nPacket Number: %d", packetCounter);
		char * infoPacket = malloc(sizeof(char) * MAX_PACKET_SIZE);
		*infoPacket = '0';
		*(infoPacket + 1) = (char) packetCounter;
		char k = (char) (MAX_PACKET_SIZE - 4);
		uint8_t l1 = ((k & 0xFF00) >> 8);
		*(infoPacket + 2) = l1;
		uint8_t l2 = (k & 0x00FF);
		*(infoPacket + 3) = l2;

		int i = 4;
		while (i < MAX_PACKET_SIZE && ((packetCounter*(MAX_PACKET_SIZE-4) + (i-4)) < fileSize)) {
			*(infoPacket + i) = *fullFile;
			fullFile++;
			i++;
		}

		printf("\nPacket size is: %d\n", i);
		llwrite(stop, al, ll, infoPacket, i);

		printf("\nExited llwrite\n");

		free(infoPacket);
		packetCounter++;
	}

	//Sends Last control Packet
	*packet_1 = '2';
	printf("\n_________________________________________________\nLast Control Packet");
	llwrite(stop, al, ll, packet_1, packetSize);

	free(packet_1);
	free(fullFileStart);
	fclose(pfd);
	return 0;
}

int readFile() {
	char * packet_1;
	int sizeOfPacket;

	char * finalFile;
	char * finalFileToStore;
	char * fileName;
	int fileSize;

	int cn = FALSE;
	while (cn == FALSE) {
		printf("\n__________________________________________________________________\nPacket Received");

		sizeOfPacket = llread(al, ll, &packet_1);

		printf("\nSize of packet is: %d", sizeOfPacket);

		if (*packet_1 == '2') {
			cn = TRUE;
			//FILE * pfd = fopen("./ola/ola2.txt", "w");
			FILE * pfd = fopen("./received/pinguimR.gif", "w");
			fwrite(finalFile, fileSize, 1, pfd);

			fclose(pfd);
		} else if (*packet_1 == '1') {
			getNameAndSizeOfFile(&packet_1, sizeOfPacket, &fileSize, &fileName);
			finalFile = malloc(sizeof(char) * fileSize);
			finalFileToStore = finalFile;
		} else {
			int i = 4;
			while (i < sizeOfPacket) {
				*finalFileToStore = *(packet_1 + i);
				i++;
				finalFileToStore++;
			}

		}

		free(packet_1);
	}

	free(fileName);
	free(finalFile);

	return 0;
}

void getNameAndSizeOfFile(char ** packet_1, int sizeOfPacket, int * fileSize, char ** fileName) {
	int j = *(*packet_1 + 2) - 1;
	int i = 3;

	*fileName = malloc(sizeof(char) * (j + 1));

	//Get the name of the file
	while (i < (j + 3)) {
		*(*fileName + (i - 3)) = *(*packet_1 + i);
		i++;
	}
	*(*fileName + (i - 3)) = '\0';

	//Get the size of the file
	i++;	//Size of the byte that describes the size
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
