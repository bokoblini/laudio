#include "processor.h"

#include <string.h>

#include "../frames.h"
#include "../detectors/areadetector/peakdetector.h"

void l_audio_processor_setup(LAudioProcessor* processor) {
  processor->buf_reserved = PROCESSOR_BUF_SIZE;
  processor->begin = 0;
  processor->len = 0;

  frames_setup(&processor->frames);
  processor->frames.frame_len = PROCESSOR_WINDOW_SIZE;

  frames_add(&processor->frames);
}

int l_audio_processor_copy(LAudioProcessor* processor, float* destination) {
  if (processor->len < PROCESSOR_WINDOW_SIZE) {
    return 0;
  }

  int n = processor->begin + processor->len;

  if (n < processor->buf_reserved) {
    memcpy(destination, processor->buf + n - PROCESSOR_WINDOW_SIZE, PROCESSOR_WINDOW_SIZE * sizeof(float));
    return 1;
  }

  n = n % processor->buf_reserved;
  if (n >= PROCESSOR_WINDOW_SIZE) {
    memcpy(destination, processor->buf + n - PROCESSOR_WINDOW_SIZE, PROCESSOR_WINDOW_SIZE * sizeof(float));
    return 1;
  }

  int lb = PROCESSOR_WINDOW_SIZE - n;
  memcpy(destination, processor->buf + processor->buf_reserved - lb, lb * sizeof(float));
  memcpy(destination + lb, processor->buf, n * sizeof(float));
  return 1;
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
  if (!l_audio_processor_copy(processor, processor->frames.data[0])) {
    return;
  }

  PeakDetectorInput pdi;
  pdi.frames = &processor->frames;
  pdi.height = 6.2;
  pdi.frame_num = 0;

  int has_peak = peakdetector_detect(&pdi);

  if (has_peak) {
    fprintf(stderr, "!!!PEAK!!!\n");
  }
}