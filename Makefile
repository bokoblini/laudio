all: \
	streamdumper \
	txtfft \
	rgen

.PHONY: all

PFFFT_DIR := /home/lori/pffft

streamdumper: streamdumper.c
	gcc -Wall -o streamdumper $(shell pkg-config --cflags libpulse) \
		streamdumper.c $(shell pkg-config --libs libpulse)

txtfft: txtfft.c fftlib.o fftlib.h
	gcc -Wall -o txtfft -I$(PFFFT_DIR) txtfft.c fftlib.o $(PFFFT_DIR)/pffft.o -lm

rgen: rgen.c
	gcc -Wall -o rgen rgen.c -lm

fftlib.o: fftlib.c fftlib.h
	gcc -Wall -I$(PFFFT_DIR) -c fftlib.c -lm