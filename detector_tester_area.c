#include <stdio.h>
#include <stdlib.h>

#include "detectors/areadetector/areadetector.h"
#include "frames.h"

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
  int rv = sscanf(argv[4], "%f", &input.area);
  if (!rv) {
    fprintf(stderr, "sscanf failed\n");
  }
  input.frames = &frames;
  rv = sscanf(argv[2], "%d", &input.frameth);
  if (!rv) {
    fprintf(stderr, "sscanf failed\n");
  }

  rv = sscanf(argv[3], "%d", &input.width);
  if (!rv) {
    fprintf(stderr, "sscanf failed\n");
  }

  int* detector_out =
      (int*)malloc((frames.frame_len - input.width) * sizeof(int));
  detect_area(&input, detector_out);

  for (int i = 0; i < frames.frame_len - input.width; ++i) {
    printf("%d", detector_out[i]);
  }
  printf("\n");
}