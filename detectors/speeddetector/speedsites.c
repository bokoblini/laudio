#include "speedsites.h"

#include <stdlib.h>
#include <string.h>

void detect_fast_changing(int *output, FastChangingDetectorInput *input) {
  int *fast_changing_list =
      (int *)malloc(input->frames->frame_len * sizeof(int));

  memset(fast_changing_list, 0, sizeof(int) * input->frames->frame_len);
  memset(output, 0, sizeof(int) * input->frames->frame_len);

  for (int i = input->frames_begin + 1; i < input->frames_end; ++i) {
    float *prev = input->frames->data[i - 1];
    float *curr = input->frames->data[i];

    for (int j = 0; j < input->frames->frame_len; ++j) {
      if (curr[j] - prev[j] >= input->g) {
        ++fast_changing_list[j];
      } else {
        fast_changing_list[j] = 0;
      }
      if (fast_changing_list[j] >= input->n) {
        output[j] = 1;
      }
    }
  }
}