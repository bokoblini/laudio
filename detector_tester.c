#include <stdio.h>
#include <stdlib.h>

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

  FastChangingDetectorInput detector_input;
  detector_input.n = 4;
  detector_input.g = 0.1;
  detector_input.frames = &frames;
  detector_input.frames_begin = 50;
  detector_input.frames_end = frames.data_len;

  int *detector_out = (int*)malloc(frames.frame_len * sizeof(int));
  detect_fast_changing(detector_out, &detector_input);

  for (int i = 0; i < frames.frame_len; ++i) {
    printf("%d", detector_out[i]);
  }  
  printf("\n");
}