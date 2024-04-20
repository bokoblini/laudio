#include "fftlib.h"

#include <pffft.h>
#include <stdio.h>
#include <stdlib.h>

void skip(FILE* audiofl, int num_lines) {
  char buf[40];
  for (int i = 0; i < num_lines; ++i) {
    char* res = fgets(buf, 40, audiofl);
    if (res == NULL) {
      if (feof(audiofl)) {
        fprintf(stderr, "EOF while reading file\n");
        exit(1);
      }
      if (ferror(audiofl)) {
        fprintf(stderr, "error reading file\n");
        exit(1);
      }
      fprintf(stderr, "fgets error\n");
      exit(1);
    }
  }
}

void read(FILE* audiofl, int num_data, float* return_data) {
  char buf[40];
  for (int i = 0; i < num_data; ++i) {
    char* res = fgets(buf, 40, audiofl);
    if (res == NULL) {
      if (feof(audiofl)) {
        fprintf(stderr, "EOF while reading file\n");
        exit(1);
      }
      if (ferror(audiofl)) {
        fprintf(stderr, "error reading file\n");
        exit(1);
      }
      fprintf(stderr, "fgets error\n");
      exit(1);
    }
    float f;
    int n;

    int rc = sscanf(buf, "%d %f", &n, &f);
    if (rc != 2) {
      fprintf(stderr, "error reading buf\n");
      exit(1);
    }
    return_data[i] = f;
  }
}

void txtfft_run(char* file_name, int num_data, int from, float input_buf[],
                float output_buf[], float work_buf[], PFFFT_Setup* setup) {
  FILE* audiofl = fopen(file_name, "r");
  if (!audiofl) {
    perror("fopen errror");
    exit(1);
  }

  skip(audiofl, from);
  read(audiofl, num_data, input_buf);
  pffft_transform_ordered(setup, input_buf, output_buf, work_buf,
                          PFFFT_FORWARD);
}
