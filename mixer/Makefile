all: \
  mixer

.PHONY: all

mixer: mixer.c 
	gcc -Wall -o mixer $(shell pkg-config --cflags gtk4) \
	  mixer.c \
	  $(shell pkg-config --libs gtk4)