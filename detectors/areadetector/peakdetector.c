#include "peakdetector.h"

#include <stdio.h>
#include <string.h>

#include "../../frames.h"

static const int buckets = 16;

#ifdef DEBUG_DETECTOR
void debug_distribution(int channel, float* data, int len) {
  int bs = len / buckets;
  if (bs < 1) {
    bs = 1;
  }
  int begin = 0;
  fprintf(stderr, "%d %d ", channel, bs);
  while (1) {
    int end = begin + bs;
    if (end > len) {
      end = len;
    }
    if (end <= begin) {
      break;
    }
    float bucket_max = 0.0f;
    for (int i = begin; i < end; ++i) {
      if (data[i] > bucket_max) {
        bucket_max = data[i];
      }
    }
    fprintf(stderr, "%f ", bucket_max);
    begin = end;
  }
  fprintf(stderr, "\n");
}
#endif

// output must point to a space holding frame->frame_len number of ints
int peakdetector_detect(PeakDetectorInput *input) {
  float* current_frame = input->frames->data[input->frame_num];

  float m = 0.0;

  for (int i = input->from_coefficient; i < input->frames->frame_len; ++i) {
#ifdef DEBUG_DETECTOR
    debug_distribution(input->channel, current_frame, input->frames->frame_len);
#endif
    if (current_frame[i] > m) {
      m = current_frame[i];
    }
    if (current_frame[i] >= input->height) {
      return 1;
    }
  }
  //fprintf(stderr, "max = %f\n", m);
  return 0;
}
