	#include "app.h"
	
	int * flag;
	int * count;
	int * stop;

	int main(int argc, char** argv)
	{
		count = malloc(sizeof(int));
		flag = malloc(sizeof(int));
		stop = malloc(sizeof(int));
		*count = 0;
		*flag = TRUE;
		*stop = FALSE;

		int fd;
		struct termios oldtio;
		
		//********************* Check if the arguments are corrected *****************************
		if ( (argc < 3) || ((strcmp("/dev/ttyS0", argv[1])!=0) && (strcmp("/dev/ttyS1", argv[1])!=0) &&	(strcmp("/dev/ttyS4", argv[1])!=0)))
		{
			printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
			exit(1);
		}
		//****************************************************************************************

		char port[strlen(argv[1]) + 1];
		strncpy(port, argv[1], strlen(argv[1]));
		port[strlen(port) - 1] = '\0';

		(void) signal(SIGALRM, triggerAlarm); // instala rotina que atende interrupcao

		ll_open(flag, stop, count, fd, *argv[2], port, &oldtio);
		
		free(count);
		free(flag);
		free(stop);

		printf("\nTerminated!\n");		

		return 0;
	}
