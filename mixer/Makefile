all: \
  mixer

.PHONY: all

audio.o: audio.c audio.h
	gcc -Wall $(shell pkg-config --cflags libpulse libpulse-mainloop-glib) -c audio.c

mixer: mixer.c audio.o audio.h
	gcc -Wall -o mixer $(shell pkg-config --cflags gtk4 libpulse-mainloop-glib libpulse) \
	  mixer.c audio.o \
	  $(shell pkg-config --libs gtk4 libpulse-mainloop-glib libpulse)