#include "conection.h"

int startConection(urlStruct * url, FTP * ftp) {
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
	return 0;
}

int showResponse(FTP * ftp) {
	char answerFromServer[MAX_STRING_DEBUG_SIZE];	
	if(read(ftp->socketFd, answerFromServer, MAX_STRING_DEBUG_SIZE) > 0) {
		stringMsg("Server response", answerFromServer);
		return 0;
	}
	msg("Could not read response form server");
	return -1;
}

int receivePassvAnswer(FTP * ftp) {
	char passvAnswer[MAX_STRING_DEBUG_SIZE];	
	if(read(ftp->socketFd, passvAnswer, MAX_STRING_DEBUG_SIZE) > 0) {
		stringMsg("Server response", passvAnswer);
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

int getControl(FTP * ftp, urlStruct * url) {
	if(sendAndReceiveControl(CMD_USER, ftp, url) != 0) {
		return -1;
	}
	
	if(sendAndReceiveControl(CMD_PASS, ftp, url) != 0) {
		return -1;
	}
	
	if(sendAndReceiveControl(CMD_PASSV, ftp, url) != 0) {
		return -1;
	}
	
	return 0;
}

int sendAndReceiveControl(int cmdSelector, FTP * ftp, urlStruct * url) {
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
			strcpy(cmd, "passv \0");
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
		debug("Command Sent", "");
	}
	sleep(1);
	switch(cmdSelector) {
		case CMD_PASSV:
			if(0 == receivePassvAnswer(ftp)) {
				msg("ESTA TUDO BEM MAS FALTA ESTA PARTE");
			/*
				// Parse new IP address
				ctrl_ftp.port = ctrl_ftp.pasv[4] * 256 + ctrl_ftp.pasv[5];
				memset(ctrl_ftp.ip, 0, MAX_SIZE_MEDIUM); // clearing the array, "just in case"
				sprintf(ctrl_ftp.ip, "%d.%d.%d.%d", ctrl_ftp.pasv[0], ctrl_ftp.pasv[1], ctrl_ftp.pasv[2], ctrl_ftp.pasv[3]);
				printf("IP address to receive file: %s\n", ctrl_ftp.ip);
				printf("Port to receive file      : %d\n", ctrl_ftp.port);
				
				return 0;*/
			}

			/*printf("Could not receive the 'pasv' response\n");
			return -1;*/
		default:
			showResponse(ftp);
			break;
	}
	return 0;
}