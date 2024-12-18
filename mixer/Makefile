all: \
  mixer

.PHONY: all

audio.o: audio.c audio.h
	gcc -g -Wall $(shell pkg-config --cflags libpulse libpulse-mainloop-glib) -c audio.c


processor.o: processor.c processor.h ../detectors/areadetector/peakdetector.h ../frames.h
	gcc -g -Wall $(shell pkg-config --cflags libpulse libpulse-mainloop-glib) -c processor.c

mixer: mixer.c audio.o processor.o ../detectors/areadetector/peakdetector.o ../frames.o audio.h
	gcc -g -Wall -o mixer $(shell pkg-config --cflags gtk4 libpulse-mainloop-glib libpulse) \
	  mixer.c audio.o processor.o ../detectors/areadetector/peakdetector.o ../frames.o \
	  $(shell pkg-config --libs gtk4 libpulse-mainloop-glib libpulse)
