#include <stdio.h>
#include <stdlib.h>

#include "detectors/areadetector/areadetector.h"
#include "frames.h"

void detect_area_graph(AreaDetectorInput* input) {
  int steps = input->frames->frame_len - input->width;
  float* current_frame = input->frames->data[input->frameth];

  for (int i = 0; i < steps; ++i) {
    float sum = 0.0;
    for (int j = 0; j < input->width; ++j) {
      sum += current_frame[j + i];
    }
    printf("%d %d %f\n", i + input->width/2, input->width, sum/(float)input->width);
  }
};

// args: Filename, number of frame, width, area
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

  AreaDetectorInput input;
  input.frames = &frames;
  int rv = sscanf(argv[2], "%d", &input.frameth);
  if (!rv) {
    fprintf(stderr, "sscanf failed\n");
  }

  for (int i = 0; i < frames.frame_len; ++i) {
    input.width = i;
    detect_area_graph(&input);
  }
}