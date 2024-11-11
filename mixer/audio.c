#include "audio.h"

#include <pulse/glib-mainloop.h>
#include <pulse/pulseaudio.h>
#include <stdio.h>

static void context_state_callback(pa_context* c, void* user_data) {
  fprintf(stderr, "abrakadabra\n");
}

void l_audio_init(LAudio *l_audio) {
  l_audio->mainloop = pa_glib_mainloop_new(NULL);

  l_audio->ctx = pa_context_new(pa_glib_mainloop_get_api(l_audio->mainloop), NULL);
  g_assert(l_audio->ctx);

  int r = pa_context_connect(l_audio->ctx, NULL,
                             PA_CONTEXT_NOAUTOSPAWN | PA_CONTEXT_NOFAIL, NULL);
  g_assert(r == 0);

  pa_context_set_state_callback(l_audio->ctx, context_state_callback, l_audio);
}

void l_audio_destruct(LAudio *l_audio) {
  pa_context_unref(l_audio->ctx);
  pa_glib_mainloop_free(l_audio->mainloop);
}