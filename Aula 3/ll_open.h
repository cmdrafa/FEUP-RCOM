	#include <sys/types.h>
	#include <sys/stat.h>
	#include <fcntl.h>
	#include <termios.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <unistd.h>
	#include <signal.h>

	#define BAUDRATE B9600
	#define MODEMDEVICE "/dev/ttyS0"
	#define _POSIX_SOURCE 1 // POSIX compliant source
	#define FALSE 0
	#define TRUE 1

	#define FLAG 0x7e
	#define A_1 0x03
	#define A_2 0x01

	#define C_SET 0x07
	#define C_UA 0x03
	#define C_DISC 0x0b

	#define BCC A^C_SET
	#define SETLEN 5
	#define TIMEOUT 3
	#define ATTEMPTS 3

	#define UALENGTH 5

	void writeMsg(int * fd, char aFlag, char cFlag);

	int readResponse(int * fd, int * flag, char aFlag, char cFlag);

	void configure(int * fd, char * serial_port, struct termios * oldtio);

	void resetConfiguration(int * fd, struct termios * oldtio);

	void triggerAlarm();

	int ll_open(int * flag, int * stop, int * count, int * fd, char side, char * port, struct termios * oldtio);

	int ll_close(int * flag, int * stop, int * count, int * fd, char side, char * port, struct termios * oldtio);
