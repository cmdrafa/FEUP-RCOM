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