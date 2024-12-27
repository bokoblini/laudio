#ifndef LAUDIO_MIXER_PROCESSOR_H
#define LAUDIO_MIXER_PROCESSOR_H

#define PROCESSOR_BUF_SIZE 4096
#define PROCESSOR_WINDOW_SIZE 2048
#define PROCESSOR_DETECT_THRESHOLD 4410

#include <pffft.h>
#include <stdint.h>

#include "../frames.h"
#include "../windowfunction.h"

typedef struct {
  float* buf;  // [PROCESSOR_BUF_SIZE * sizeof(float)];
  int buf_reserved;

  int begin;
  int len;

  Frames frames;

  uint64_t samples_since_detect;
  int channel_num;

  PFFFT_Setup* pffft_setup;
  float* pffft_input_buf;
  float* pffft_output_buf;
  float* pffft_work_buf;
  float* abs_buf_w;

  WindowFunction wf;
} LAudioProcessor;

void l_audio_processor_setup(LAudioProcessor* processor, int channel_num);

void l_audio_processor_feed(LAudioProcessor* processor, const float* data,
                            int nsamples, int stride);

void l_audio_processor_detect(LAudioProcessor* processor);

typedef struct {
  LAudioProcessor* ps;
  int num_ps;
} LMultiProcessor;

void l_multi_processor_setup(LMultiProcessor* processor, int num_channels);
void l_multi_processor_feed(LMultiProcessor* processor, const float* data,
                            int nsamples);

#endif  // LAUDIO_MIXER_PROCESSOR_H