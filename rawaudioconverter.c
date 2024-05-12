// Usage: ./rawaudioconverter to_bin|to_raw filename
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void to_bin(char* file_name) {
  FILE* file_stream = fopen(file_name, "r");
  if (!file_stream) {
    perror("fopen failed");
    exit(1);
  }
  while (1) {
    char line_buf[256];
    char* rv = fgets(line_buf, 255, file_stream);
    if (!rv) {
      if (ferror(file_stream)) {
        perror("input file reading with fgets failed");
        if (fclose(file_stream)) {
          perror("error while reading input file (fclose1)");
          exit(1);
        }
        exit(1);
      }
      if (feof(file_stream)) {
        if (fclose(file_stream)) {
          perror("error while reading input file (fclose2)");
          exit(1);
        }
        return;
      }
      fprintf(stderr, "fgets failed to read from stream\n");
      exit(1);
    }
    int num;
    float val;
    int rc = sscanf(line_buf, "%d %f", &num, &val);
    if (rc != 2) {
      fprintf(stderr, "sscanf failed to read from line_buf\n");
      exit(1);
    }
    ssize_t wrc = write(1, &val, sizeof(float));
    if (wrc != sizeof(float)) {
      if (wrc == -1) {
        perror("write error");
        exit(1);
      }
      fprintf(stderr, "partial write occured\n");
      exit(1);
    }
  }
}

void to_raw(char* file_name) {
  FILE* file_stream = fopen(file_name, "r");
  if (!file_stream) {
    perror("fopen failed");
    exit(1);
  }

  int n = 0;
  while (1) {
    float f;
    size_t rc = fread(&f, sizeof(float), 1, file_stream);
    if (rc != 1) {
      if (ferror(file_stream)) {
        perror("input file reading with fread failed");
        if (fclose(file_stream)) {
          perror("error while reading input file (fclose1)");
          exit(1);
        }
        exit(1);
      }
      if (feof(file_stream)) {
        if (fclose(file_stream)) {
          perror("error while reading input file (fclose2)");
          exit(1);
        }
        return;
      }
      fprintf(stderr, "fread failed to read from stream\n");
      exit(1);
    }
    printf("%d %f\n", n++, f);
    fflush(stdout);
  }
}

int main(int argc, char* argv[]) {
  if (argc != 3) {
    fprintf(stderr, "usage: ./rawaudioconverter to_bin|to_raw filename\n");
    exit(1);
  }

  if (strcmp(argv[1], "to_raw") == 0) {
    to_raw(argv[2]);
  } else if (strcmp(argv[1], "to_bin") == 0) {
    to_bin(argv[2]);
  } else {
    fprintf(stderr, "usage: ./rawaudioconverter to_bin|to_raw filename\n");
    exit(1);
  }
}