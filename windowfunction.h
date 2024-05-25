#ifndef WINDOW_FUNCTION
#define WINDOW_FUNCTION

typedef struct {
  float *w;
} WindowFunction;

void window_function_setup(WindowFunction *wf, int len);

void window_function_apply_hann(WindowFunction *wf, float *fft_window, int len);

#endif //WINDOW_FUNCTION