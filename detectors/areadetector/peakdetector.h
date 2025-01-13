#ifndef LAUDIO_PEAKDETECTOR_H
#define LAUDIO_PEAKDETECTOR_H

#include "../../frames.h"

typedef struct {
  Frames *frames;
  float height;
  int frame_num;
  int channel;
  int from_coefficient;
} PeakDetectorInput;

int peakdetector_detect(PeakDetectorInput *input);

#endif // LAUDIO_PEAKDETECTOR.H
