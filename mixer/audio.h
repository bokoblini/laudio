
#ifndef LAUDIO_MIXER_AUDIO_H
#define LAUDIO_MIXER_AUDIO_H

#include <pulse/pulseaudio.h>
#include <pulse/glib-mainloop.h>

typedef struct {
  pa_context* ctx;
  pa_glib_mainloop *mainloop;
} LAudio;

void l_audio_init(LAudio* l_audio);
void l_audio_destruct(LAudio *l_audio);

#endif  // LAUDIO_MIXER_AUDIO_H