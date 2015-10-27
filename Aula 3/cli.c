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

void flushIn() {
  char ch;
  while ((ch = getchar()) != '\n' && ch != EOF);
}
