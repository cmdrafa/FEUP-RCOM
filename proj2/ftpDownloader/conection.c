#include "conection.h"

int startConection(urlStruct * url, FTP * ftp) {
	debug("#################### DEBUG START CONNECTION ####################", "BEGIN");
	int	socket_fd;
    struct sockaddr_in server_addr;

	// Configuring server address
	bzero((char*)&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(url->hostIp);				// 32 bit Internet address network byte ordered
	server_addr.sin_port = htons(PORT_FTP);							// server TCP port (21) must be network byte ordered

	// Opening the control TCP socket
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(socket_fd < 0) {
		printf("Error opening control TCP socket\n");
		return -1;
	} else {
		char temp[MAX_STRING_DEBUG_SIZE];
		sprintf(temp, "%d", socket_fd);
		debug("Opened control TCP socket with Fd    ", temp);
	}

	// Connecting to the server...
	int connectRet;
	if((connectRet = connect(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr))) < 0) {
		perror("connect()");
		printf("Error connecting to the server to open the control connection\n");
		return -1;
	} else {
		char temp[MAX_STRING_DEBUG_SIZE];
		sprintf(temp, "%d", connectRet);
		debug("Connected to server with return      ", temp);
	}

	ftp->socketFd = socket_fd;

	msg("Connected");
	debug("#################### DEBUG START CONNECTION ####################", "END");

	return 0;
}

int showResponse(FTP * ftp) {
	char answerFromServer[MAX_STRING_DEBUG_SIZE] = "";	
	if(read(ftp->socketFd, answerFromServer, MAX_STRING_DEBUG_SIZE) > 0) {
		responseMsg("Server response", answerFromServer);
		return 0;
	}
	msg("Could not read response form server");
	return -1;
}

int receivePassvAnswer(FTP * ftp) {
	char passvAnswer[MAX_STRING_DEBUG_SIZE];	
	if(read(ftp->socketFd, passvAnswer, MAX_STRING_DEBUG_SIZE) > 0) {
		responseMsg("Server response", passvAnswer);
		if(6 != sscanf(passvAnswer, "%*[^(](%d,%d,%d,%d,%d,%d)\n", &(ftp->passvAnswer[0]), &(ftp->passvAnswer[1]), &(ftp->passvAnswer[2]), &(ftp->passvAnswer[3]), &(ftp->passvAnswer[4]), &(ftp->passvAnswer[5])))
		{
			stringMsg("Could not read the 6 bytes from the server response", passvAnswer);
			return -1;
		}
		return 0;
	}
	
	msg("Could not read response form server socket - PASSV");
	return -1;
}

int getControl(FTP * ftp, urlStruct * url, FTP * receiverFtp) {
	if(sendAndReceiveControl(CMD_USER, ftp, receiverFtp, url) != 0) {
		return -1;
	}

	
	if (strlen(url->password) != 0) {
		msg("Entering in anonymous mode");
	}
	
	if(sendAndReceiveControl(CMD_PASS, ftp, receiverFtp, url) != 0) {
		return -1;
	}
	
	if(sendAndReceiveControl(CMD_PASSV, ftp, receiverFtp, url) != 0) {
		return -1;
	}
	
	return 0;
}

int sendAndReceiveControl(int cmdSelector, FTP * ftp, FTP * receiverFtp, urlStruct * url) {
	debug("#################### DEBUG SEND AND RECEIVE CONTROL ####################", "BEGIN");
	char cmd[MAX_STRING_DEBUG_SIZE];
	switch(cmdSelector) {
		case CMD_USER:
			strcpy(cmd, "user \0");
			strcat(cmd, url->user);
			break;
		case CMD_PASS:
			strcpy(cmd, "pass \0");
			strcat(cmd, url->password);
			break;
		case CMD_PASSV:
			strcpy(cmd, "pasv \0");
			break;
		default:
			break;
	}
	strcat(cmd, "\n");
	debug("Command to Send", cmd);
	if(write(ftp->socketFd, cmd, strlen(cmd)) < 0) {
		perror("write()");
		return -1;
	} else {
		debug("Command Sent", cmd);
	}
	sleep(1);
	switch(cmdSelector) {
		case CMD_PASSV:
			if(0 == receivePassvAnswer(ftp)) {
				// Parse new IP address
				receiverFtp->port = ftp->passvAnswer[4] * 256 + ftp->passvAnswer[5];
				memset(receiverFtp->ip, 0, MAX_STRING_DEBUG_SIZE); // clearing the array, "just in case"
				sprintf(receiverFtp->ip, "%d.%d.%d.%d", ftp->passvAnswer[0], ftp->passvAnswer[1], ftp->passvAnswer[2], ftp->passvAnswer[3]);
				stringMsg("IP address to receive file", (char *) &receiverFtp->ip);
				char temp[MAX_STRING_DEBUG_SIZE];
				sprintf(temp, "%d", receiverFtp->port);
				stringMsg("Port to receive file      ", temp);
				
				debug("#################### DEBUG SEND AND RECEIVE CONTROL ####################", "END");
				return 0;
			}

			msg("Could not receive the 'passvAnswer' response");
			return -1;
		default:
			showResponse(ftp);
			break;
	}
	debug("#################### DEBUG SEND AND RECEIVE CONTROL ####################", "END");
	return 0;
}

int startReceiverConection(urlStruct * url, FTP * ftp) {
	debug("#################### DEBUG START RECEIVER CONECTION ####################", "BEGIN");
	struct sockaddr_in server_addr;
	char *host_ip;
	int socket_fd;

	if (NULL == (url->h = gethostbyname(ftp->ip))) {
		msg("Could not get host");
		return -1;
	}

	host_ip = inet_ntoa(*((struct in_addr *)url->h->h_addr));

	stringMsg("Host name  ", url->h->h_name);
	stringMsg("IP Address ", host_ip);

	// Configuring server address
	bzero((char*)&server_addr,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(host_ip);	//32 bit Internet address network byte ordered
	server_addr.sin_port = htons(ftp->port);				//server TCP port must be network byte ordered | this is the new port received from the control TCP connection!
	
	// Opening the receiver TCP socket
	socket_fd = socket(AF_INET, SOCK_STREAM,0);
	if (socket_fd < 0) {
		msg("Error opening receiver TCP socket");
		return -1;
	}

	int connectRet;
	if((connectRet = connect(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr))) < 0) {
		perror("connect()");
		msg("Error connecting to the server to open the control connection");
		return -1;
	} else {
		char temp[MAX_STRING_DEBUG_SIZE];
		sprintf(temp, "%d", connectRet);
		debug("Connected to server with return      ", temp);
	}
	

	ftp->socketFd = socket_fd;
	ftp->dataSocketFd = socket_fd;
	msg("Connected to receiver");
	debug("#################### DEBUG START RECEIVER CONECTION ####################", "END");

	return 0;
}

int receiveFile(urlStruct * url, FTP * ftp, FTP * receiverFtp) {
	debug("#################### DEBUG RECEIVE FILE ####################", "BEGIN");
   	msg("Receiving File...");
	char cmd[MAX_STRING_DEBUG_SIZE] = "";
	strcpy(cmd, "retr ");
	strcat(cmd, url->urlPath);
	strcat(cmd, "\n");
	stringMsg("Command to be sent", cmd);
	if(write(ftp->socketFd, cmd, strlen(cmd)) < 0){
		msg("ERROR - retr command could not be sent");
		return -1;
	}
	stringMsg("Command sent", cmd);

	char * filename;
	getName(url->urlPath, &filename);
	debug("Filename is", filename);

	FILE* file;
	int bytes;

	if (!(file = fopen(filename, "w"))) {
		msg("ERROR: Cannot open file.");
		return -1;
	}

	char temp[MAX_STRING_DEBUG_SIZE];
	sprintf(temp,"%d",  receiverFtp->dataSocketFd);
	debug("Receiver Data Socket fd", temp);

	char buf[1024];
	while ((bytes = read(receiverFtp->dataSocketFd, buf, sizeof(buf)))) {
		if (bytes < 0) {
			msg("ERROR: Nothing was received.");
			return -1;
		}

		if ((bytes = fwrite(buf, bytes, 1, file)) < 0) {
			msg("ERROR: Cannot write data in file.\n");
			return -1;
		}
		debug("In read cycle", "Read 1 byte");
	}

	fclose(file);

	msg("File received");
	debug("#################### DEBUG RECEIVE FILE ####################", "END");

	return 0;
}