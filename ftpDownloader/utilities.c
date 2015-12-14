#include "utilities.h"

void debug(char * msg1, char * msg) {
	if (DEBUG_MODE == 1) {
		printf("%s: %s\n", msg1, msg);
	}
}