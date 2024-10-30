# ifndef LAUDIO_SPEEDSITES_H
# define LAUDIO_SPEEDSITES_H

#include "../../frames.h"

typedef struct {
  int n;
  double g;
  Frames *frames;
  int frames_begin;
  int frames_end;
} FastChangingDetectorInput;

// output must be a previously allocated int* with the length of a frame
void detect_fast_changing(int *output, FastChangingDetectorInput *input);

#endif // LAUDIO_SPEEDSITES_H