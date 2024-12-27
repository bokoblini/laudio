#include "peakdetector.h"

#include <stdio.h>
#include <string.h>

#include "../../frames.h"

// output must point to a space holding frame->frame_len number of ints
int peakdetector_detect(PeakDetectorInput *input) {
  float* current_frame = input->frames->data[input->frame_num];

  float m = 0.0;


  for (int i = 0; i < input->frames->frame_len; ++i) {
    if (current_frame[i] > m) {
      m = current_frame[i];
    }
    if (current_frame[i] >= input->height) {
      return 1;
    }
  }
  // fprintf(stderr, "max = %f\n", m);
  return 0;
}