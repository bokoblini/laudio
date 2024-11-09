#include "frames.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

void save_frame(Frames *frames, float *dbuf, int frame_len) {
  if (frames->frame_len == 0) {
    frames->frame_len = frame_len;
  } else {
    if (frames->frame_len != frame_len) {
      fprintf(stderr, "frame sizes do not match\n");
      exit(1);
    }
  }

  float* new_frame = frames_add(frames);

  memcpy(new_frame, dbuf, frame_len*sizeof(float));
}

void frames_load_from_file(Frames* frames, FILE* input_file) { 
  char buf[1024];
  float dbuf[65536];

  int sample_idx = 0;

  while (fgets(buf, 1024, input_file)) {
    if (*buf == '\n') {
      save_frame(frames, dbuf, sample_idx);
      sample_idx = 0;
      continue;
    }

    float f;
    int sr = sscanf(buf, "%f", &f);
    if (sr != 1) {
      fprintf(stderr, "failed while reading\n");
      exit(1);
    }

    dbuf[sample_idx++] = f;

  }
  if (sample_idx > 0) {
    save_frame(frames, dbuf, sample_idx);
  }
}