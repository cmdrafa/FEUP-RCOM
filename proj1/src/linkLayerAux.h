void writeMsg(int * fd, char aFlag, char cFlag);

int readResponse(int * fd, int * flag, char aFlag, char cFlag);

void configure(linkLayer * ll, struct termios * oldtio);

void resetConfiguration(int * fd, struct termios * oldtio);

void triggerAlarm();

int readSenderResponse(linkLayer * ll);

char * stuff(char **deStuffed, int deStuffedLength, int * bufSize);

int readInfo(int * fd, int * flag, char * buffer);

int desStuff(char * destuffed, char ** stuffed, linkLayer * ll);

int removeFrameHeaderAndTrailer(char ** buffer, int sizeOfInfoRead);
