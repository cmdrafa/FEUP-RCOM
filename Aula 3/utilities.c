#include "utilities.h"

int getFileSize(FILE* file) {
	long int currentPosition = ftell(file);

	if (fseek(file, 0, SEEK_END) == -1) {
		printf("ERROR: Could not get file size.\n");
		return -1;
	}

	long int size = ftell(file);

	fseek(file, 0, currentPosition);

	return size;
}