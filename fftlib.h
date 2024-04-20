#ifndef FFTLIB_H
#define FFTLIB_H

#include <pffft.h>

void txtfft_run(char* file_name, int num_data, int from, float input_buf[],
                float output_buf[], float work_buf[], PFFFT_Setup* setup);


#endif  // FFTLIB_H