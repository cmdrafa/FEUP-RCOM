	#include "app.h"

	int * flag;
	int * count;
	int * stop;

	applicationLayer * al;
	linkLayer * ll;

	int main(int argc, char** argv)
	{
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

		ll_open(flag, stop, count, al, ll, &oldtio);
		ll_close(flag, stop, count, al, ll, &oldtio);

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
