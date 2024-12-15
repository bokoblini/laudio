#ifndef LAUDIO_MIXER_PROCESSOR_H
#define LAUDIO_MIXER_PROCESSOR_H

#define PROCESSOR_BUF_SIZE 4096
#define PROCESSOR_WINDOW_SIZE 2048

#include "../frames.h"

typedef struct {
  float buf[PROCESSOR_BUF_SIZE * sizeof(float)];
  int buf_reserved;

  int begin;
  int len;

  Frames frames;
} LAudioProcessor;

void l_audio_processor_setup(LAudioProcessor* processor);

void l_audio_processor_feed(LAudioProcessor* processor, float* data,
                            int nsamples, int stride);

void l_audio_processor_detect(LAudioProcessor* processor);

#endif  // LAUDIO_MIXER_PROCESSOR_H