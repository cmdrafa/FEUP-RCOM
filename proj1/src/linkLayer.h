	#include "utilities.h"
	#include "linkLayerAux.h"


	int ll_open(int * flag, int * stop, int * count, linkLayer * ll, struct termios * oldtio);

	int ll_close(int * flag, int * stop, int * count, linkLayer * ll, struct termios * oldtio);

	int llwrite(int * stop, linkLayer * ll, char * buffer, int length);

	int llread(linkLayer * ll, char ** buffer);
