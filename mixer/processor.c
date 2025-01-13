#include "processor.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pffft.h>
#include <math.h>

#include "../detectors/areadetector/peakdetector.h"
#include "../frames.h"
#include "../windowfunction.h"

void l_audio_processor_setup(LAudioProcessor* processor, int channel_num) {
  processor->buf = (float*)malloc(PROCESSOR_BUF_SIZE * sizeof(float));
  if (!processor->buf) {
    fprintf(stderr, "malloc failed for LAudioProcessor.buf\n");
    exit(EXIT_FAILURE);
  }
  processor->buf_reserved = PROCESSOR_BUF_SIZE;
  processor->begin = 0;
  processor->len = 0;

  frames_setup(&processor->frames);
  processor->frames.frame_len = PROCESSOR_WINDOW_SIZE / 2;

  frames_add(&processor->frames);

  processor->samples_since_detect = 0;

  processor->channel_num = channel_num;

  processor->pffft_setup = pffft_new_setup(PROCESSOR_WINDOW_SIZE, PFFFT_REAL);

  window_function_setup(&processor->wf, PROCESSOR_WINDOW_SIZE);

  processor->pffft_input_buf =
      (float*)pffft_aligned_malloc(sizeof(float) * PROCESSOR_WINDOW_SIZE);
  if (!processor->pffft_input_buf) {
    fprintf(stderr, "pffft_aligned_malloc failed to allocate memory\n");
    exit(1);
  }

  processor->pffft_output_buf =
      (float*)pffft_aligned_malloc(sizeof(float) * PROCESSOR_WINDOW_SIZE);
  if (!processor->pffft_output_buf) {
    fprintf(stderr, "pffft_aligned_malloc failed to allocate memory\n");
    exit(1);
  }

  processor->pffft_work_buf =
      (float*)pffft_aligned_malloc(sizeof(float) * PROCESSOR_WINDOW_SIZE * 2);
  if (!processor->pffft_work_buf) {
    fprintf(stderr, "pffft_aligned_malloc failed to allocate memory\n");
    exit(1);
  }

  processor->abs_buf_w =
      (float*)malloc(PROCESSOR_WINDOW_SIZE / 2 * sizeof(float));
  if (!processor->abs_buf_w) {
    fprintf(stderr, "malloc failed to allocate memory\n");
    exit(1);
  }
}

int l_audio_processor_copy(LAudioProcessor* processor, float* destination) {
  if (processor->len < PROCESSOR_WINDOW_SIZE) {
    return 0;
  }

  int n = processor->begin + processor->len;

  if (n < processor->buf_reserved) {
    memcpy(destination, processor->buf + n - PROCESSOR_WINDOW_SIZE,
           PROCESSOR_WINDOW_SIZE * sizeof(float));
    return 1;
  }

  n = n % processor->buf_reserved;
  if (n >= PROCESSOR_WINDOW_SIZE) {
    memcpy(destination, processor->buf + n - PROCESSOR_WINDOW_SIZE,
           PROCESSOR_WINDOW_SIZE * sizeof(float));
    return 1;
  }

  int lb = PROCESSOR_WINDOW_SIZE - n;
  memcpy(destination, processor->buf + processor->buf_reserved - lb,
         lb * sizeof(float));
  memcpy(destination + lb, processor->buf, n * sizeof(float));
  return 1;
}

void l_audio_processor_feed(LAudioProcessor* processor, const float* data,
                            int nsamples, int stride) {
  processor->samples_since_detect += nsamples;

  int buf_place = (processor->begin + processor->len) % processor->buf_reserved;
  for (int i = 0; i < nsamples; ++i) {
    processor->buf[buf_place] = data[i * stride];
    ++buf_place;
    if (buf_place >= processor->buf_reserved) {
      buf_place -= processor->buf_reserved;
    }
  }

  if (processor->len + nsamples >= processor->buf_reserved) {
    processor->begin = buf_place;
    processor->len = processor->buf_reserved;
  } else {
    processor->len = processor->len + nsamples;
  }

  if (processor->samples_since_detect > PROCESSOR_DETECT_THRESHOLD) {
    l_audio_processor_detect(processor);
    processor->samples_since_detect = 0;
  }
}

void l_audio_processor_detect(LAudioProcessor* processor) {
  if (!l_audio_processor_copy(processor, processor->pffft_input_buf)) {
    return;
  }

  window_function_apply_hann(&processor->wf, processor->pffft_input_buf,
                             PROCESSOR_WINDOW_SIZE);
  pffft_transform_ordered(processor->pffft_setup, processor->pffft_input_buf,
                          processor->pffft_output_buf,
                          processor->pffft_work_buf, PFFFT_FORWARD);
  for (int i = 0; i < PROCESSOR_WINDOW_SIZE / 2; ++i) {
    processor->frames.data[0][i] =
        log(1.0 + sqrt(pow(processor->pffft_output_buf[i * 2], 2.0) +
                       pow(processor->pffft_output_buf[i * 2 + 1], 2.0)));
  }

  PeakDetectorInput pdi;
  pdi.frames = &processor->frames;
  pdi.height = 4;
  pdi.frame_num = 0;
  pdi.channel = processor->channel_num;
  pdi.from_coefficient = 3 * 64;

  int has_peak = peakdetector_detect(&pdi);

  // fprintf(stderr, "looking for peak: %d\n", has_peak);
  if (has_peak) {
    static int n = 0;
    // fprintf(stderr, "!!!PEAK!!! %d %d\n", processor->channel_num, n++);
    processor->feedback_signal(processor->feedback_signal_data);
  }
}

void l_multi_processor_setup(LMultiProcessor* processor, int num_channels) {
  processor->ps =
      (LAudioProcessor*)malloc(sizeof(LAudioProcessor) * num_channels);
  if (!processor->ps) {
    fprintf(stderr, "malloc for LMultiProcessor setup");
    exit(1);
  }
  processor->num_ps = num_channels;

  for (int i = 0; i < num_channels; ++i) {
    l_audio_processor_setup(processor->ps + i, i);
  }
}
void l_multi_processor_feed(LMultiProcessor* processor, const float* data,
                            int nsamples) {
  for (int i = 0; i < processor->num_ps; ++i) {
    l_audio_processor_feed(processor->ps + i, data + i, nsamples,
                           processor->num_ps);
  }
}
