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
		char con[] = "107pinguim.gif18";
		strcpy(control, con);
		strncpy(control + 16, *fileSizeChar, 8);
		return control;
	}
	
	int sendFile() {
	 int packetSize;
	 
	 FILE * pfd = fopen("./pinguim.gif", "r");
	 
	 int fileSize = getFileSize(pfd);
	 char * fileSizeChar = malloc(sizeof(char) * 8);
	 sprintf(fileSizeChar, "%d", fileSize);
	 
	 char * packet_1 = createFirstControlPacket(&packetSize, &fileSizeChar);
	 free(fileSizeChar);
	 
	 //Sends First control packet
	 llwrite(stop, al, ll, packet_1, packetSize);
	 
	 int sentBytes = 0;
	 int packetCounter = 0;
	 int numberOfPackets = fileSize / MAX_SIZE;
	 if ((fileSize % MAX_SIZE) > 0)
	   numberOfPackets++;
	 while (packetCounter < numberOfPackets) {
	  printf("\nPacket Number: %d\n", packetCounter);
	  packetCounter++;
	 }
	 
	 //Sends Last control Packet
	 *packet_1 = '2';
	 llwrite(stop, al, ll, packet_1, packetSize);
	 
	 free(packet_1); 
	 fclose(pfd);
	 return 0;
	}
	
	int readFile() {
	  char * packet_1;
	  int sizeOfPacket;
	  
	  int cn = FALSE;
	  while (cn == FALSE) {
	    sizeOfPacket = llread(al, ll, &packet_1);
	    	    
	    int i = 0;
	    printf("\n");
	    while (i < sizeOfPacket) {
	      printf("%c", *(packet_1 + i));
	      i++;
	    }
	    printf("\n");
	    
	    printf("\nSize of packet is: %d\n", sizeOfPacket);
	    printf("\n");
	    
	    if (*packet_1 == '2') {
	      cn = TRUE;
	    }
	    
	    free(packet_1);
	  }
	  
	  return 0;
	}