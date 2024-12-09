all: \
	streamdumper \
	txtfft \
	rgen \
	multitxtfft \
	gtkpelda \
	slidergrapher \
	rawaudioconverter \
	try_window_function \
	detector_tester \
	detector_tester_area \
	detector_tester_area_graph

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

multitxtfft: multitxtfft.c fftlib.o fftlib.h
	gcc -Wall -o multitxtfft -I$(PFFFT_DIR) multitxtfft.c fftlib.o $(PFFFT_DIR)/pffft.o -lm

gtkpelda: gtkpelda.c
	gcc -Wall -o gtkpelda $(shell pkg-config --cflags gtk4) gtkpelda.c $(shell pkg-config --libs gtk4)

slidergrapher: slidergrapher.c loader.c loader.h windowfunction.o windowfunction.h frames.h frames.o
	gcc -Wall -o slidergrapher -I$(PFFFT_DIR) $(shell pkg-config --cflags gtk4) slidergrapher.c loader.c $(shell pkg-config --libs gtk4) $(PFFFT_DIR)/pffft.o windowfunction.o frames.o -lm

rawaudioconverter: rawaudioconverter.c
	gcc -Wall -o rawaudioconverter rawaudioconverter.c

windowfunction.o: windowfunction.c windowfunction.h
	gcc -Wall -c windowfunction.c

try_window_function: try_window_function.c windowfunction.o windowfunction.h
	gcc -Wall -o try_window_function try_window_function.c windowfunction.o -lm 

frames.o: frames.c frames.h
	gcc -Wall -c frames.c

detector_tester: detector_tester.c detectors/speeddetector/speedsites.h detectors/speeddetector/speedsites.o frames.o
	gcc -Wall -o detector_tester detector_tester.c detectors/speeddetector/speedsites.o frames.o

detector_tester_area: detector_tester_area.c detectors/areadetector/areadetector.h detectors/areadetector/areadetector.o frames.o
	gcc -Wall -o detector_tester_area detector_tester_area.c detectors/areadetector/areadetector.o frames.o

detector_tester_area_graph: detector_tester_area_graph.c frames.o
	gcc -Wall -o detector_tester_area_graph detector_tester_area_graph.c frames.o