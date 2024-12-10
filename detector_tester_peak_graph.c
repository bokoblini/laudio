#include <stdio.h>
#include <stdlib.h>

#include "detectors/areadetector/peakdetector.h"
#include "frames.h"

float print_peak_big(PeakDetectorInput *input, int current_frame_num) {
  float* current_frame = input->frames->data[current_frame_num];
  float biggest = 0;
  for (int i = 0; i < input->frames->frame_len; ++i) {
    if (biggest < current_frame[i]) {
      biggest = current_frame[i];
    }
  }
  return biggest;
}

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

  PeakDetectorInput input;
  input.frames = &frames;

  int rv = sscanf(argv[2], "%d", &input.frame_num);
  if (!rv) {
    fprintf(stderr, "sscanf failed\n");
  }

  int frame_end;
  rv = sscanf(argv[3], "%d", &frame_end);
  if (!rv) {
    fprintf(stderr, "sscanf failed\n");
  }

  for (int i = input.frame_num; i < frame_end; ++i) {
    float sum = print_peak_big(&input, i);
    printf("%d %f\n", i - input.frame_num, sum);
  }

  printf("\n");
}