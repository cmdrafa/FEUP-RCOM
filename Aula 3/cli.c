#include "cli.h"

void clearScreen() {
	printf("\033[2J");
}

int initialMenu() {
  clearScreen();

  printf("\n\n\n"
    "********************************************************\n"
    "******************* RCOM project - 1 *******************\n"
    "********************************************************\n"
    "**                                                    **\n"
    "**         Choose Which side of the program:          **\n"
    "**                                                    **\n"
    "**         1 - Transmitter                            **\n"
    "**         2 - Receiver                               **\n"
    "**                                                    **\n"
    "********************************************************\n"
    "\n\n");

    int choice = -1;

    scanf("%d", &choice);

    flushIn();

    if (choice == 1 || choice == 2) {
    return choice;
  }

  return -1;
}

int choosePort() {
  clearScreen();

  printf("\n\n\n"
    "********************************************************\n"
    "******************* RCOM project - 1 *******************\n"
    "********************************************************\n"
    "**                                                    **\n"
    "**         Choose the port you want to use:           **\n"
    "**                                                    **\n"
    "**         1 - /dev/ttyS0                             **\n"
    "**         2 - /dev/ttyS4                             **\n"
    "**                                                    **\n"
    "********************************************************\n"
    "\n\n");

    int choice = -1;

    scanf("%d", &choice);

    flushIn();

    if (choice == 1 || choice == 2) {
    return choice;
  }

  return -1;
}

int chooseBaudrate() {
  clearScreen();

  printf("\n\n\n"
    "********************************************************\n"
    "******************* RCOM project - 1 *******************\n"
    "********************************************************\n"
    "**                                                    **\n"
    "**         Choose the Baudrate you want to use:       **\n"
    "**                                                    **\n"
    "**         1 - B300                                   **\n"
    "**         2 - B600                                   **\n"
    "**         3 - B1200                                  **\n"
    "**         4 - B1800                                  **\n"
    "**         5 - B2400                                  **\n"
    "**         6 - B4800                                  **\n"
    "**         7 - B9600                                  **\n"
    "**         8 - B19200                                 **\n"
    "**         9 - B38400                                 **\n"
    "**        10 - B57600                                 **\n"
    "**        11 - B115200                                **\n"
    "**                                                    **\n"
    "********************************************************\n"
    "\n\n");

    int choice = -1;

    scanf("%d", &choice);

    flushIn();

    if (choice >= 1 && choice <= 11) {
    return choice;
  }

  return -1;
}

int chooseMaxSize() {
  clearScreen();

  printf("\n\n\n"
    "********************************************************\n"
    "******************* RCOM project - 1 *******************\n"
    "********************************************************\n"
    "**                                                    **\n"
    "**      Choose the Maximum size of bytes/packet:      **\n"
    "**                                                    **\n"
    "**                     [11 - 2000]                    **\n"
    "**                                                    **\n"
    "********************************************************\n"
    "\n\n");

    int choice = -1;

    scanf("%d", &choice);

    flushIn();

    if (choice >= 11 && choice <= 2000) {
    return choice;
  }

  return -1;
}

int chooseTimeout() {
  clearScreen();

  printf("\n\n\n"
    "********************************************************\n"
    "******************* RCOM project - 1 *******************\n"
    "********************************************************\n"
    "**                                                    **\n"
    "**            Choose the timeout in seconds:          **\n"
    "**                                                    **\n"
    "**                      [1 - 60]                      **\n"
    "**                                                    **\n"
    "**                  0 - default value                 **\n"
    "**                                                    **\n"
    "********************************************************\n"
    "\n\n");

    int choice = -1;

    scanf("%d", &choice);
    
    if (choice == 0) choice = 3;
    
    flushIn();

    if (choice >= 1 && choice <= 60) {
    return choice;
  }

  return -1;
}

int chooseNumTransmissions() {
  clearScreen();

  printf("\n\n\n"
    "********************************************************\n"
    "******************* RCOM project - 1 *******************\n"
    "********************************************************\n"
    "**                                                    **\n"
    "**            Choose the number of attempts:          **\n"
    "**                                                    **\n"
    "**                      [1 - 60]                      **\n"
    "**                                                    **\n"
    "**                                                    **\n"
    "********************************************************\n"
    "\n\n");

    int choice = -1;

    scanf("%d", &choice);

    flushIn();

    if (choice >= 1 && choice <= 60) {
    return choice;
  }

  return -1;
}

void showInitialInfo(linkLayer * ll, applicationLayer * al) {
  clearScreen();

  char modeW[] = "Transmitter\0";
  char modeR[] = "Receiver\0";
  int s = -1;
  
  if ((*al).status == 'W') {
    s = 0;
  } else {
    s = 1;
  }
  
  if (s == 0) {
  
  printf("\n\n\n"
    "************************************\n"
    "********* RCOM project - 1 *********\n"
    "************************************\n"
    "                                    \n"
    "       # Initial Information #      \n"
    "                                    \n"
    "                                    \n"
    "            Mode: %s\n"
    "       Baud rate: %d\n"
    "    Msg Max Size: %d\n"
    "        Attempts: %d\n"
    "         Timeout: %d\n\n"
    "************************************\n"
    "\n\n", modeW, (*ll).baudRate, (*ll).packSize, (*ll).numTransmissions, (*ll).timeout);

  } else {
      printf("\n\n\n"
    "************************************\n"
    "********* RCOM project - 1 *********\n"
    "************************************\n"
    "                                    \n"
    "       # Initial Information #      \n"
    "                                    \n"
    "                                    \n"
    "            Mode: %s\n"
    "       Baud rate: %d\n"
    "    Msg Max Size: %d\n"
    "        Attempts: %d\n"
    "         Timeout: %d\n\n"
    "************************************\n"
    "\n\n", modeR, (*ll).baudRate, (*ll).packSize, (*ll).numTransmissions, (*ll).timeout);

  }

  return;
}

void flushIn() {
  char ch;
  while ((ch = getchar()) != '\n' && ch != EOF);
}
