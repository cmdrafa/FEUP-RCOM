void writeMsg(applicationLayer * al, char aFlag, char cFlag);

int readResponse(applicationLayer * al, int * flag, char aFlag, char cFlag);

void configure(applicationLayer * al, linkLayer * ll, struct termios * oldtio);

void resetConfiguration(applicationLayer * al, struct termios * oldtio);

void triggerAlarm();

int readSenderResponse(applicationLayer * al, linkLayer * ll);

char * stuff(char **unStuffed, int unStuffedLength, int * bufSize);

int readInfo(applicationLayer * al, int * flag, char * buffer);

int unStuff(char * unstuffed, char ** stuffed, linkLayer * ll);

int removeFrameHeaderAndTrailer(char ** buffer, int sizeOfInfoRead);
