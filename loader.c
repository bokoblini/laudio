#include "loader.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int read_txtfft(char *file_name, float **buf, int *len) {
  FILE *input_file = fopen(file_name, "r");
  if (!input_file) {
    perror("read_txtfft fopen failed");
    return -1;
  }

  int buf_size = 1000;
  float *data = (float *)malloc(buf_size * sizeof(float));
  int read_len = 0;
  while (1) {
    char buf[256];
    char *rv = fgets(buf, 255, input_file);
    if (!rv) {
      if (ferror(input_file)) {
        perror("input file reading with fgets failed");
        if (!fclose(input_file)) {
          perror("error while reading input file (fclose)");
          return -1;
        }
        return -1;
      }
      if (feof(input_file)) {
        *buf = data;
        *len = read_len;
        if (!fclose(input_file)) {
          perror("error while reading input file (fclose)");
          return -1;
        }
        return 0;
      }
    }
    int ord_num;
    float val;
    int rc = sscanf(buf, "%d%f", &ord_num, &val);
    if (rc != 2) {
      fprintf(stderr, "hibas sor: %s\n", buf);
      if (!fclose(input_file)) {
        perror("error while reading input file (fclose)");
        return -1;
      }
      return -1;
    }
    if (buf_size == read_len) {
      float *tmp = (float *)malloc(buf_size * 2 * sizeof(float));
      if (!tmp) {
        fprintf(stderr, "error while reading input file (new malloc failed)\n");
        if (!fclose(input_file)) {
          perror("error while reading input file (fclose)");
          return -1;
        }
        return -1;
      }
      memcpy(tmp, data, buf_size);
      buf_size *= 2;
      free(data);
      data = tmp;
    }
    data[read_len] = val;
    ++read_len;
  }
}