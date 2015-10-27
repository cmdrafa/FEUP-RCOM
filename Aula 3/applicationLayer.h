	#include "linkLayer.h"
	//#include "cli.h"

	int main(int argc, char** argv);

	void fillLinkLayer();

	char * createFirstControlPacket(int * packetSize, char ** fileSizeChar);

	int sendFile();

	void getNameAndSizeOfFile(char ** packet_1, int sizeOfPacket, int * fileSize, char ** fileName);
