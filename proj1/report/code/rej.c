if (ll->debug == TRUE) {
	  printf("\nBcc2-2: 0x%x", bcc2);
	}

	if ((*(*buffer + 3) != (*(*buffer + 1) ^ *(*buffer + 2))) || (*(*buffer + sizeOfInfoRead - 2) != bcc2)) {
		if ((*ll).sequenceNumber == 0) {

			if (ll->debug == TRUE) {
			  printf("\n****\n2nd ERROR receiving 1, wanted 0, sending REJ 0\n****");
			}
			ll->stat->numSentREJ++;
			writeMsg(&(ll->fd), A_1, C_REJ_0);
			free(buffer_2);
			return -5;
		} else if ((*ll).sequenceNumber == 1) {
			if (ll->debug == TRUE) {
			  printf("\n****\n2nd ERROR receiving 0, wanted 1, sending REJ 1\n****");
			}
			ll->stat->numSentREJ++;
			writeMsg(&(ll->fd), A_1, C_REJ_1);
			free(buffer_2);
			return -4;
		}
	}
	else if ((*ll).sequenceNumber == 0 && *(*buffer + 2) == C_0) {
		writeMsg(&(ll->fd), A_1, C_RR_1);
		ll->stat->numSentRR++;
		(*ll).sequenceNumber = 1;
	} else if ((*ll).sequenceNumber == 1 && *(*buffer + 2) == C_1) {
		writeMsg(&(ll->fd), A_1, C_RR_0);
		ll->stat->numSentRR++;
		(*ll).sequenceNumber = 0;
	} else if ((*ll).sequenceNumber == 0 && *(*buffer + 2) == C_1) {
		writeMsg(&(ll->fd), A_1, C_REJ_0);
		if (ll->debug == TRUE) {
		  printf("\n****\nERROR receiving 1, wanted 0, sending REJ 0\n****");
		}
		ll->stat->numSentREJ++;
		free(buffer_2);
		return -2;
	} else if ((*ll).sequenceNumber == 1 && *(*buffer + 2) == C_0) {
		writeMsg(&(ll->fd), A_1, C_REJ_1);
		if (ll->debug == TRUE) {
		  printf("\n****\nERROR receiving 0, wanted 1, sending REJ 1\n****");
		}
		ll->stat->numSentREJ++;
		free(buffer_2);
		return -3;
	}