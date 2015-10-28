	#include "utilities.h"
	#include "linkLayerAux.h"


	int ll_open(int * flag, int * stop, int * count, applicationLayer * al, linkLayer * ll, struct termios * oldtio);

	int ll_close(int * flag, int * stop, int * count, applicationLayer * al, linkLayer * ll, struct termios * oldtio);

	int llwrite(int * stop, applicationLayer * al, linkLayer * ll, char * buffer, int length);

	int llread(applicationLayer * al, linkLayer * ll, char ** buffer);
