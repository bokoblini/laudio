#include "frames.h"

#include <stdio.h>
#include <stdlib.h>

void frames_setup(Frames* frames) {
  frames->data = NULL;
  frames->data_len = 0;
  frames->data_reserved = 0;
  frames->frame_len = 0;
}

float* frames_add(Frames* frames) {
  if (frames->data_len >= frames->data_reserved) {
    int new_reserved = 2 * frames->data_reserved;
    if (new_reserved == 0) {
      new_reserved = 1;
    }
    float** new_data = (float**)malloc(new_reserved * sizeof(float*));
    if (!new_data) {
      fprintf(stderr, "could not malloc for new frames data\n");
      exit(1);
    }

    for (int i = 0; i < frames->data_len; ++i) {
      new_data[i] = frames->data[i];
    }

    float** old_data = frames->data;
    frames->data = new_data;
    frames->data_reserved = new_reserved;
    if (old_data) {
      free(old_data);
    }
  }

  ++frames->data_len;
  frames->data[frames->data_len - 1] =
      (float*)malloc(frames->frame_len * sizeof(float));
  return frames->data[frames->data_len - 1];
}