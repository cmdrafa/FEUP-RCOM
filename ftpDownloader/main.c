#include "conection.h"

int main(int argc, char *argv[])
{
    urlStruct * url = malloc(sizeof(urlStruct));

    if (argc != 2) {  
        fprintf(stderr,"usage: getip address\n");
        exit(1);
    }
    getUrlInfo(argv[1], url);

    FTP ftp;

    if (startConection(url, &ftp) < 0) {
        msg("Error starting connection\n");
        return -1;
    }

    if (getControl(&ftp, url) < 0) {
        msg("ERROR - getControl()");
        return -1;
    }

    close(ftp.socketFd);
    msg("Sockets Closed, Terminating...");

    free(url->user);
    free(url->password);
    free(url->urlPath);
    free(url);

    return 0;
}
