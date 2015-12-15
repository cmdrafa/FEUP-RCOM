#include "url.h"

void getUrlInfo(char * completeString, urlStruct * url) {
    debug("#################### DEBUG URL INFO ####################", "BEGIN");

    if(strncmp(completeString, "ftp://", 6)) {
        printf("Wrong Url on argument, expected begining like: 'ftp://'\n");
        exit(1);
    }
    //######################## debug code ########################
    char debugString[MAX_STRING_DEBUG_SIZE];
    char debugString_2[MAX_STRING_DEBUG_SIZE];
    char debugString_3[MAX_STRING_DEBUG_SIZE];
    char debugString_4[MAX_STRING_DEBUG_SIZE];
    char debugString_5[MAX_STRING_DEBUG_SIZE];
    //######################## debug code ########################

    char * at = strchr(completeString, '@');
    if (at == NULL) {
        msg("Entering anonymous Mode");
    }

    char * toTwoPoints = strchr(completeString + 6, ':');
    char * slashAfterAt = strchr(completeString + 7, '/');

    if (slashAfterAt == NULL) {
        printf("ERROR - Wrong paramater -> URL | Expected something like: ftp://[<user>:<password>@]<host>/<url-path>\n");
        exit(1);
    }

    int lengthOfUserAndPassword;
    int lengthOfUser;
    int lengthOfPassword;
    int lengthOfHost;
    int lengthOfUrlPath;

    if (at != NULL) {
        lengthOfUserAndPassword = (int) (at - completeString - 6);
        lengthOfUser = (int) (toTwoPoints - completeString - 6);
        lengthOfPassword = lengthOfUserAndPassword - lengthOfUser - 1;
        lengthOfHost = (int) (slashAfterAt - at - 1);
        lengthOfUrlPath = strlen(completeString) - (9 + lengthOfUser + lengthOfPassword + lengthOfHost);
    } else {
        lengthOfUserAndPassword = 0;
        lengthOfUser = (int) 0;
        lengthOfPassword = 0;
        lengthOfHost = (int) (slashAfterAt - completeString - 6);
        lengthOfUrlPath = strlen(completeString) - (7 + lengthOfUser + lengthOfPassword + lengthOfHost);
    }

    if(lengthOfHost <= 0 || lengthOfUrlPath <= 0) {
        printf("ERROR - Wrong paramater -> URL | Expected something like: ftp://[<user>:<password>@]<host>/<url-path>\n");
        exit(1);
    }

    //######################## debug code ########################
    sprintf(debugString, "%d", lengthOfUser);
    sprintf(debugString_2, "%d", lengthOfPassword);
    sprintf(debugString_3, "%d", lengthOfHost);
    sprintf(debugString_5, "%d", lengthOfUrlPath);
    debug("Number of characters of the User     ", debugString);
    debug("Number of characters of the Password ", debugString_2);
    debug("Number of characters of the Host     ", debugString_3);
    debug("Number of characters of the Url Path ", debugString_5);
    //######################## debug code ########################

    url->password = malloc(sizeof(char) * lengthOfPassword);
    url->urlPath = malloc(sizeof(char) * lengthOfUrlPath);
    char hostTemp[MAX_STRING_DEBUG_SIZE];
    if (at != NULL) {
        url->user = malloc(sizeof(char) * lengthOfUser);
        strncpy(url->user, completeString + 6, lengthOfUser);
        strncpy(url->password, completeString + lengthOfUser + 7, lengthOfPassword);
        strncpy(hostTemp, at + 1, lengthOfHost);
    } else {
        url->user = malloc(sizeof(char) * strlen("anonymous"));
        strncpy(url->user, "anonymous", strlen("anonymous"));
        strncpy(hostTemp, completeString + 6, lengthOfHost);
    }
    strncpy(url->urlPath, slashAfterAt + 1, lengthOfUrlPath);
    hostTemp[lengthOfHost] = '\0';

    //######################## debug code ########################
    debug("User field                           ", url->user);
    debug("Password field                       ", "<password>");
    debug("Host field                           ", hostTemp);
    debug("Url Path field                       ", url->urlPath);
    //######################## debug code ########################

    if ((url->h=gethostbyname(hostTemp)) == NULL) {  
        herror("gethostbyname");
        exit(1);
    }

    sprintf(debugString_4, "%s", inet_ntoa(*((struct in_addr *)url->h->h_addr)));
    url->hostIp = malloc(sizeof(char) * strlen(debugString_4));
    strncpy(url->hostIp, debugString_4, strlen(debugString_4));

    //######################## debug code ########################
    debug("Host name                            ", url->h->h_name);
    debug("IP Address                           ", url->hostIp);
    //######################## debug code ########################

    debug("#################### DEBUG URL INFO ####################", "END");
    return;
}
