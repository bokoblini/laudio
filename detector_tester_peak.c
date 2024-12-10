#include <stdio.h>
#include <stdlib.h>

#include "detectors/areadetector/peakdetector.h"
#include "frames.h"

// args: Filename, max height, frame_num 
int main(int argc, char* argv[]) {
  FILE* input_file = fopen(argv[1], "r");
  if (!input_file) {
    perror("detectortester fopen failed");
    return -1;
  }

  Frames frames;
  frames_setup(&frames);

  frames_load_from_file(&frames, input_file);

  fprintf(stderr, "data_len: %d\n", frames.data_len);

  PeakDetectorInput input;
  input.frames = &frames;

  int rv = sscanf(argv[2], "%f", &input.height);
  if (!rv) {
    fprintf(stderr, "sscanf failed\n");
  }

  rv = sscanf(argv[3], "%d", &input.frame_num);
  if (!rv) {
    fprintf(stderr, "sscanf failed\n");
  }

  int* detector_out = (int*)malloc(frames.frame_len * sizeof(int));
  detect_peak(&input, detector_out);

  for (int i = 0; i < frames.frame_len; ++i) {
    printf("%d", detector_out[i]);
  }
  printf("\n");
}