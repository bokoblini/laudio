#include <pffft.h>
#include <stdio.h>
#include <stdlib.h>

#include "fftlib.h"

// txtfft adatfile elso hany lepes darabszam

int main(int argc, char* argv[]) {
  int elso_adat;
  int rv = sscanf(argv[2], "%d", &elso_adat);
  if (rv != 1) {
    fprintf(stderr, "sscanf elso_adat error\n");
    exit(1);
  }

  int hany_adat;
  rv = sscanf(argv[3], "%d", &hany_adat);
  if (rv != 1) {
    fprintf(stderr, "sscanf hany_adat error\n");
    exit(1);
  }

  int lepes;
  rv = sscanf(argv[4], "%d", &lepes);
  if (rv != 1) {
    fprintf(stderr, "sscanf lepes error\n");
    exit(1);
  }

  int darabszam;
  rv = sscanf(argv[5], "%d", &darabszam);
  if (rv != 1) {
    fprintf(stderr, "sscanf darabszam error\n");
    exit(1);
  }

  float* xs = (float*)pffft_aligned_malloc(sizeof(float) * hany_adat);
  if (!xs) {
    fprintf(stderr, "pffft_aligned_malloc failed to allocate memory\n");
    exit(1);
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

  for(int j = 0; j < darabszam; ++j) {
    txtfft_run(argv[1], hany_adat, elso_adat + j*lepes, xs, output, work, setup);

    for (int i = 0; i < hany_adat; ++i) {
      printf("%d %d %f\n", j, i, output[i]);
    }
    printf("\n\n");
  }

  pffft_aligned_free(xs);
  pffft_aligned_free(work);
  pffft_aligned_free(output);
  pffft_destroy_setup(setup);
}