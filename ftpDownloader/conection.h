#include "url.h"

typedef struct
{
    int socketFd; // file descriptor to control socket
    int data_socket_fd; // file descriptor to data socket

    int passvAnswer[6];
} FTP;

int startConection(urlStruct * url, FTP * ftp);
int showResponse(FTP * ftp);
int sendAndReceiveControl(int cmd, FTP * ftp, urlStruct * url);
int receivePassvAnswer(FTP * ftp);
int getControl(FTP * ftp, urlStruct * url);