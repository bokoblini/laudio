#ifndef LAUDIO_FRAMES_H
#define LAUDIO_FRAMES_H

typedef struct {
  float** data;
  int data_len;
  int data_reserved;
  int frame_len;
} Frames;

void frames_setup(Frames *frames);
float* frames_add(Frames *frames);

void frames_load_from_file(Frames* frames, char* filename);

#endif  // LAUDIO_FRAMES_H
