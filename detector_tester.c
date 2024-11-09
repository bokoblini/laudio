#include <stdio.h>

#include "detectors/speeddetector/speedsites.h"
#include "frames.h"

// args: Filename, frame size, frame begin, frame end 
int main (int argc, char* argv[]){
  FILE* input_file = fopen(argv[1], "r");
  if (!input_file) {
    perror("detectortester fopen failed");
    return -1;
  }

  Frames frames;
  frames_setup(&frames);

  frames_load_from_file(&frames, input_file);

  fprintf(stderr, "data_len: %d\n", frames.data_len);
}