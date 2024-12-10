#include "peakdetector.h"

#include <stdio.h>
#include <string.h>

#include "../../frames.h"

// output must point to a space holding frame->frame_len number of ints
void detect_peak(PeakDetectorInput *input, int *output) {
  memset(output, 0, input->frames->frame_len);

  float* current_frame = input->frames->data[input->frame_num];

  for (int i = 0; i < input->frames->frame_len; ++i) {
    if (current_frame[i] >= input->height) {
      output[i] = 1;
    }
  }
}