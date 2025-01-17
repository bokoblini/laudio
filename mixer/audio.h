
#ifndef LAUDIO_MIXER_AUDIO_H
#define LAUDIO_MIXER_AUDIO_H

#include <stdint.h>

#include <pulse/pulseaudio.h>
#include <pulse/glib-mainloop.h>

#include "processor.h"

typedef struct {
  pa_context* ctx;
  pa_glib_mainloop *mainloop;
  uint32_t source_index;
  LMultiProcessor* l_processor;
  pa_cvolume cvolume;
} LAudio;

void l_audio_init(LAudio* l_audio, LMultiProcessor* l_processor);
void l_audio_destruct(LAudio *l_audio);

void l_audio_set_volume(LAudio *l_audio, double volume, int channel);

#endif  // LAUDIO_MIXER_AUDIO_H