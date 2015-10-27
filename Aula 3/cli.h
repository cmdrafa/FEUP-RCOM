//#include <stdio.h>
#include "utilities.h"

  void clearScreen();

  int initialMenu();

  void flushIn();
	
  int chooseBaudrate();
  
  int chooseMaxSize();
  
  int chooseTimeout();
  
  int chooseNumTransmissions();
  
  void showInitialInfo(linkLayer * ll, applicationLayer * al);
  
  void printProgressBar(int current, int total);
  
  void printStats(applicationLayer * al, Statistics * stats);