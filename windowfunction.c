#include "windowfunction.h"

#include <stdio.h>
#include <stdlib.h>
#define __USE_GNU
#include <math.h>

void window_function_setup(WindowFunction *wf, int len) {
  wf->w = (float*)malloc(len * sizeof(float));
  if (!wf->w) {
    fprintf(stderr, "window function malloc failed");
    exit(1);
  }

  for (int n = 0; n < len; ++n) {
    wf->w[n] = 0.5*(1 - cosf(2 * M_PIl * n/len));
  }  
}

void window_function_apply_hann(WindowFunction *wf, float *fft_window, int len) {
  for (int i = 0; i < len; ++i) {
    fft_window[i] *= wf->w[i];
  }
}