#ifndef LAUDIO_AREADETECTOR_H
#define LAUDIO_AREADETECTOR_H

#include "../../frames.h"

typedef struct {
  Frames* frames;
  int frameth;
  int width;
  float area;
} AreaDetectorInput;

void detect_area(AreaDetectorInput *input, int *output);

#endif // LAUDIO_AREADETECTOR_H