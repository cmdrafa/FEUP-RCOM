all: main.c utilities.c url.c conection.c
	gcc -Wall -o download main.c utilities.c url.c conection.c