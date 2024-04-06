streamdumper: streamdumper.c
	gcc -Wall -o streamdumper $(shell pkg-config --cflags libpulse) \
		streamdumper.c $(shell pkg-config --libs libpulse)