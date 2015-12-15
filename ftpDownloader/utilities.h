#include <stdio.h> 
#include <stdlib.h> 
#include <errno.h> 
#include <netdb.h> 
#include <sys/types.h>
#include <netinet/in.h> 
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define DEBUG_MODE 1
#define MAX_STRING_DEBUG_SIZE 100
#define PORT_FTP 21

#define CMD_USER 0
#define CMD_PASS 1
#define CMD_PASSV 2


typedef struct{
	char * user;
	char * password;
	struct hostent * h;
	char * urlPath;
	char * hostIp;
}urlStruct;

void debug(char * msg1, char * msg);
void msg(char * m);
void stringMsg(char * m, char * m2);
void getName(char * url, char ** filename);