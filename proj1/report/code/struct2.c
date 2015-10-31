typedef struct {
  int fd;
  char port[20];
  int baudRate;
  unsigned int sequenceNumber;
  unsigned int timeout;
  unsigned int numTransmissions;
  int packSize;
  char status;
  int debug;
  Statistics * stat;
} linkLayer;