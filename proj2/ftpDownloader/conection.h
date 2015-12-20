#include "url.h"

typedef struct
{
    int socketFd; // file descriptor to control socket
    int dataSocketFd; // file descriptor to data socket

    int passvAnswer[6];
    int port;
    char ip[MAX_STRING_DEBUG_SIZE];

} FTP;

int startConection(urlStruct * url, FTP * ftp);
int showResponse(FTP * ftp);
int sendAndReceiveControl(int cmd, FTP * ftp, FTP * receiverFtp, urlStruct * url);
int receivePassvAnswer(FTP * ftp);
int getControl(FTP * ftp, urlStruct * url, FTP * receiverFTP);
int startReceiverConection(urlStruct * url, FTP * ftp);
int receiveFile(urlStruct * url, FTP * ftp, FTP * receiverFtp);