#include "utilities.h"

void debug(char * msg1, char * msg) {
	if (DEBUG_MODE == 1) {
		printf("DEBUG: %s: %s\n", msg1, msg);
	}
}

void msg(char * m) {
	printf("PROGRAM: %s\n", m);
}

void stringMsg(char * m, char * m2) {
	printf("PROGRAM: %s: %s\n", m, m2);
}

void getName(char * url, char ** filename) {
	char * temp = url;
	int i = 0;
	while(temp != NULL) {
		if(i > 0)
			*filename = temp + 1;
		else
			*filename = temp;
		temp = strchr(*filename, '/');
		i++;
	}
}