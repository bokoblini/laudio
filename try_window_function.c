#define __USE_GNU
#include <math.h>
#include <stdio.h>

#include "windowfunction.h"

# define M_PIl		3.141592653589793238462643383279502884L

int main(int argc, char *argv[]) {
  float period_time;
  sscanf(argv[1], "%f", &period_time);
  fprintf(stderr, "%f\n", period_time);

  float sinus_out[1024];
  float sinus_tr[1024];
  for (int i = 0; i < 1024; ++i) {
    sinus_out[i] = sin(2.0 * M_PIl * (double)i / period_time);
    sinus_tr[i] = sinus_out[i];
  }

  WindowFunction wf;
  window_function_setup(&wf, 1024);
  window_function_apply_hann(&wf, sinus_tr, 1024);

  for (int i = 0; i < 1024; ++i) {
    printf("%d %f %f %f\n", i, sinus_out[i], sinus_tr[i], wf.w[i]);
  }
}