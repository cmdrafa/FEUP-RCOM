#include <stdio.h> 
#include <stdlib.h> 
#include <errno.h> 
#include <netdb.h> 
#include <sys/types.h>
#include <netinet/in.h> 
#include <string.h>

#define DEBUG_MODE 1
#define MAX_STRING_DEBUG_SIZE 100

void debug(char * msg1, char * msg);