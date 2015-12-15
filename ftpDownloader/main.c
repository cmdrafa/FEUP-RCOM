#include "conection.h"

int main(int argc, char *argv[])
{
    debug("", "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");

    urlStruct * url = malloc(sizeof(urlStruct));
    urlStruct * receiverUrl = malloc(sizeof(urlStruct));

    if (argc != 2) {  
        fprintf(stderr,"usage: getip address\n");
        exit(1);
    }
    getUrlInfo(argv[1], url);

    FTP ftp;
    FTP receiverFtp;

    if (startConection(url, &ftp) < 0) {
        msg("Error starting connection\n");
        return -1;
    }

    if (getControl(&ftp, url, &receiverFtp) < 0) {
        msg("ERROR - getControl()");
        return -1;
    }

    if (startReceiverConection(receiverUrl, &receiverFtp) < 0) {
        msg("Error starting receiver connection");
        return -1;
    }

    if (receiveFile(url, &ftp, &receiverFtp) < 0) {
        msg("Error receiving file");
        return -1;
    }

    close(receiverFtp.socketFd);
    close(ftp.socketFd);
    msg("Sockets Closed, Terminating...");

    free(url->user);
    free(url->password);
    free(url->urlPath);
    free(url);

    return 0;
}
