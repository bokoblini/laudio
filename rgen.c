// 1. parameter: milyen fuggveny; valid: sin
// 2. parameter: hany adat
// tovabbi parameterek: generator fuggo

#define _GNU_SOURCE
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 1. parameter: hany periodus
void gen_sin(float adat[], int hany_adat, int argc, char* argv[]) {
  int hany_periodus;
  int rc = sscanf(argv[0], "%d", &hany_periodus);
  if (rc != 1) {
    fprintf(stderr, "elso generator parameter: hany periodus\n");
    exit(1);
  }

  float dx = 2.0f * M_PI * (float)hany_periodus / (float)hany_adat;
  for (int i = 0; i < hany_adat; ++i) {
    adat[i] = sin(dx * (float)i);
  }
}

int main(int argc, char* argv[]) {
  int hany_adat;
  int rc = sscanf(argv[2], "%d", &hany_adat);
  if (rc != 1) {
    fprintf(stderr, "masodik parameter: hany adat\n");
    exit(1);
  }
  float* adat = (float*)malloc(hany_adat * sizeof(float));
  if (!adat) {
    fprintf(stderr, "failed malloc\n");
    exit(1);
  }

  int remaining_argc = argc - 3;
  char** remaining_argv = argv + 3;

  if (strcmp("sin", argv[1]) == 0) {
    gen_sin(adat, hany_adat, remaining_argc, remaining_argv);
  }
  for (int i = 0; i < hany_adat; ++i) {
    printf("%d %f\n", i, adat[i]);
  }

  free(adat);
}