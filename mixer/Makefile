all: \
  mixer

.PHONY: all

PFFFT_DIR := /home/lori/pffft

audio.o: audio.c audio.h processor.h
	gcc -g -Wall $(shell pkg-config --cflags libpulse libpulse-mainloop-glib) -I$(PFFFT_DIR) -c audio.c


processor.o: processor.c processor.h ../detectors/areadetector/peakdetector.h ../frames.h ../windowfunction.h
	gcc -g -Wall $(shell pkg-config --cflags libpulse libpulse-mainloop-glib) -I$(PFFFT_DIR) -c processor.c

mixer: mixer.c audio.o processor.o ../detectors/areadetector/peakdetector.o ../frames.o audio.h ../windowfunction.h
	gcc -g -Wall -o mixer $(shell pkg-config --cflags gtk4 libpulse-mainloop-glib libpulse) -I$(PFFFT_DIR) \
	  mixer.c audio.o processor.o ../detectors/areadetector/peakdetector.o ../frames.o ../windowfunction.o \
	  $(PFFFT_DIR)/pffft.o $(shell pkg-config --libs gtk4 libpulse-mainloop-glib libpulse) -lm
