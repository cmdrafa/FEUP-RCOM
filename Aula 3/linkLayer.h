 #include "utilities.h"

	void writeMsg(applicationLayer * al, char aFlag, char cFlag);

	int readResponse(applicationLayer * al, int * flag, char aFlag, char cFlag);

	void configure(applicationLayer * al, linkLayer * ll, struct termios * oldtio);

	void resetConfiguration(applicationLayer * al, struct termios * oldtio);

	void triggerAlarm();

	int ll_open(int * flag, int * stop, int * count, applicationLayer * al, linkLayer * ll, struct termios * oldtio);

	int ll_close(int * flag, int * stop, int * count, applicationLayer * al, linkLayer * ll, struct termios * oldtio);

	int readSenderResponse(applicationLayer * al, linkLayer * ll);

	int llwrite(int * stop, applicationLayer * al, linkLayer * ll, char * buffer, int length);
	
	char * stuff(char *unStuffed, int totalLength);
	
	int llread(applicationLayer * al, linkLayer * ll, char * buffer);
	
	int readInfo(applicationLayer * al, int * flag, char * buffer);
	
	int unStuff(char * unstuffed, char * stuffed);
