#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdint.h>

#define BAUDRATE B9600
#define MODEMDEVICE "/dev/ttyS0"
#define _POSIX_SOURCE 1 // POSIX compliant source
#define FALSE 0
#define TRUE 1

#define FLAG 0x7e
#define A_1 0x03
#define A_2 0x01

#define C_0 0x0
#define C_1 0x20

#define C_RR_0 0x1
#define C_RR_1 0x21
#define C_REJ_0 0x5
#define C_REJ_1 0x25

#define C_SET 0x07
#define C_UA 0x03
#define C_DISC 0x0b

#define ESCAPE 0x7d
#define FLAG_EXC 0x5e
#define ESCAPE_EXC 0x5d

#define BCC A^C_SET
#define SETLEN 5
#define TIMEOUT 3
#define ATTEMPTS 3

#define MAX_PACKET_SIZE 100

#define UALENGTH 5

typedef struct {
  int fd;
  char status;
} applicationLayer;

typedef struct {
  char port[20];
  int baudRate;
  unsigned int sequenceNumber;
  unsigned int timeout;
  unsigned int numTransmissions;
} linkLayer;

int getFileSize(FILE* file);
