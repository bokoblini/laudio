#include "areadetector.h"

#include <stdio.h>
#include <string.h>

#include "../../frames.h"

// output must point to a space holding steps number of ints, where
// steps = input->frames->frame_len - input->width
void detect_area(AreaDetectorInput* input, int* output) {
  int steps = input->frames->frame_len - input->width;
  memset(output, 0, steps);
  float* current_frame = input->frames->data[input->frameth];

  for (int i = 0; i < steps; ++i) {
    float sum = 0.0;
    for (int j = 0; j < input->width; ++j) {
      sum += current_frame[j + i];
    }
    if (sum >= input->area) {
      output[i] = 1;
    }
  }
};