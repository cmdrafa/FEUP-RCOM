void writeMsg(applicationLayer * al, char aFlag, char cFlag);

int readResponse(applicationLayer * al, int * flag, char aFlag, char cFlag);

void configure(applicationLayer * al, linkLayer * ll, struct termios * oldtio);

void resetConfiguration(applicationLayer * al, struct termios * oldtio);

void triggerAlarm();

int readSenderResponse(applicationLayer * al, linkLayer * ll);

char * stuff(char **deStuffed, int deStuffedLength, int * bufSize);

int readInfo(applicationLayer * al, int * flag, char * buffer);

int desStuff(char * destuffed, char ** stuffed, linkLayer * ll);

int removeFrameHeaderAndTrailer(char ** buffer, int sizeOfInfoRead);
