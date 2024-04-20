#include <pffft.h>
#include <stdio.h>
#include <stdlib.h>

// txtfft adatfile elso hany

int main(int argc, char* argv[]) {
  FILE* audiofl = fopen(argv[1], "r");
  if (!audiofl) {
    perror("fopen errror");
    exit(1);
  }

  int elso_adat;
  int rv = sscanf(argv[2], "%d", &elso_adat);
  if (rv == EOF) {
    perror("sscanf elsoadat error");
    exit(1);
  }

  int hany_adat;
  rv = sscanf(argv[3], "%d", &hany_adat);
  if (rv == EOF) {
    perror("sscanf hany_adat error");
    exit(1);
  }

  char buf[40];
  for (int i = 0; i < elso_adat; ++i) {
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

  float* xs = (float*)pffft_aligned_malloc(sizeof(float) * hany_adat);
  if (!xs) {
    fprintf(stderr, "pffft_aligned_malloc failed to allocate memory\n");
    exit(1);
  }

  for (int i = 0; i < hany_adat; ++i) {
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
    xs[i] = f;
  }

  PFFFT_Setup* setup = pffft_new_setup(hany_adat, PFFFT_REAL);

  float* output = (float*)pffft_aligned_malloc(sizeof(float) * hany_adat);
  if (!output) {
    fprintf(stderr, "pffft_aligned_malloc failed to allocate memory\n");
    exit(1);
  }

  float* work = (float*)pffft_aligned_malloc(sizeof(float) * hany_adat);
  if (!work) {
    fprintf(stderr, "pffft_aligned_malloc failed to allocate memory\n");
    exit(1);
  }

  pffft_transform_ordered(setup, xs, output, work, PFFFT_FORWARD);

  for (int i = 0; i < hany_adat; ++i) {
    printf("%d %f\n", i, output[i]);
  }

  pffft_aligned_free(xs);
  pffft_aligned_free(work);
  pffft_aligned_free(output);
  pffft_destroy_setup(setup);
}