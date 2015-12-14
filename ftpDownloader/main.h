#include "utilities.h"

typedef struct{
	char * user;
	char * password;
	struct hostent * h;
	char * urlPath;
}urlStruct;

void getUrlInfo(char * completeString, urlStruct * url);

