#ifndef LAUDIO_PEAKDETECTOR_H
#define LAUDIO_PEAKDETECTOR_H

#include "../../frames.h"

typedef struct {
  Frames *frames;
  float height;
  int frame_num;
} PeakDetectorInput;

void detect_peak(PeakDetectorInput *input, int *output);

#endif // LAUDIO_PEAKDETECTOR.H