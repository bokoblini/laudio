#include "processor.h"

#include "../frames.h"

void l_audio_processor_setup(LAudioProcessor* processor) {
  processor->buf_reserved = PROCESSOR_BUF_SIZE;
  processor->begin = 0;
  processor->len = 0;

  frames_setup(&processor->frames);
  processor->frames.frame_len = PROCESSOR_WINDOW_SIZE;

  frames_add(&processor->frames);
}

void l_audio_processor_feed(LAudioProcessor* processor, float* data,
                            int nsamples, int stride) {
  processor->len += nsamples;
  processor->len = processor->len % processor->buf_reserved;

  int buf_place = processor->begin;
  for (int i = 0; i < nsamples; ++i) {
    ++buf_place;
    if (buf_place >= processor->buf_reserved) {
      buf_place -= processor->buf_reserved;
    }
    processor->buf[buf_place] = data[i * stride];
  }

  processor->begin = buf_place + 1;
  if (buf_place >= processor->buf_reserved) {
    buf_place -= processor->buf_reserved;
  }
}

void l_audio_processor_detect(LAudioProcessor* processor) {
  if (processor->len < PROCESSOR_WINDOW_SIZE) {
    return;
  }

  int n = processor->begin + processor->len - PROCESSOR_WINDOW_SIZE;
  n = n % processor->buf_reserved;

  // TODO: solve with memcpy
  for (int i = 0; i < PROCESSOR_WINDOW_SIZE; ++i) {
    processor->frames.data[0][i] = processor->buf[n];
    ++n;
    n = n % processor->buf_reserved;
  }
}