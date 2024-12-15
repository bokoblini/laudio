#include "audio.h"

#include <pulse/glib-mainloop.h>
#include <pulse/pulseaudio.h>
#include <stdio.h>
#include <string.h>

const char* serviceid = "Laudio streamdumper";

void print_audio_data(float* data, size_t len) {
  static int64_t n = 0;
  for (size_t i = 0; i < len; i += 2) {
    printf("%ld %f\n", n++, data[i]);
  }
  fflush(stdout);
}

static void stream_read_callback(pa_stream* s, size_t, void*) {
  pa_context* context = pa_stream_get_context(s);
  const void* p;

  size_t l;
  if (pa_stream_peek(s, &p, &l) < 0) {
    fprintf(stderr, "stream peek error: %s\n",
            pa_strerror(pa_context_errno(context)));
    return;
  }

  // fprintf(stderr, "frame size: %lu\n", l);

  // print_audio_data((float*)p, l / sizeof(float));

  pa_stream_drop(s);
}

static void stream_state_callback(pa_stream* s, void*) {}

void l_audio_set_volume(LAudio* l_audio, double volume, int channel) {
  pa_cvolume c_volume;
  pa_cvolume_init(&c_volume);
  pa_cvolume_set(&c_volume, channel, pa_sw_volume_from_linear(volume / 100.0));
  pa_context_set_source_volume_by_index(l_audio->ctx, l_audio->source_index,
                                        &c_volume, NULL, NULL);
  fprintf(stderr, "kakukk\n");
}

static void source_info_callback(pa_context* context, pa_source_info* info,
                                 int eol, void* user_data) {
  LAudio* l_audio = (LAudio*)user_data;
  if (!info) {
    return;
  }
  fprintf(stderr, "source: %d; %s; %s; %s\n", info->index, info->name,
          info->description, info->driver);
  //  fprintf(stderr, "  properties: %s\n",
  //  pa_proplist_to_string(info->proplist));
  for (int i = 0; i < info->volume.channels; ++i) {
    fprintf(stderr, "  volume %d: %d\n", i, info->volume.values[i]);
  }
  if (strcmp(info->description, "Built-in Audio Analog Stereo") == 0) {
    l_audio->source_index = info->index;
  }
}

static void create_stream(pa_context* context, const char* name,
                          const char* description, const pa_sample_spec* ss,
                          const pa_channel_map* cmap) {
  pa_sample_spec nss;
  nss.format = PA_SAMPLE_FLOAT32;
  nss.rate = ss->rate;
  nss.channels = ss->channels;
  fprintf(stderr, "Sample rate: %d\nChannels: %d\n", nss.rate, nss.channels);

  // struct pa_channel_map mcmap;
  // pa_channel_map_init_stereo(&mcmap);
  char buf[1024];
  pa_channel_map_snprint(buf, 1024, cmap);
  fprintf(stderr, "Channelmap: %s\n", buf);

  pa_stream* stream = pa_stream_new(context, serviceid, &nss, cmap);
  pa_stream_set_state_callback(stream, stream_state_callback, NULL);
  pa_stream_set_read_callback(stream, stream_read_callback, NULL);
  pa_stream_connect_record(stream, name, NULL, (enum pa_stream_flags)0);
}

static void context_get_source_info_callback(pa_context* context,
                                             const pa_source_info* si,
                                             int is_last, void*) {
  if (!si) return;

  create_stream(context, si->name, si->description, &si->sample_spec,
                &si->channel_map);
}

static void context_get_server_info_callback(pa_context* c,
                                             const pa_server_info* si, void*) {
  fprintf(stderr, "xzy\n");
  if (!si) {
    fprintf(stderr, "Failed to get server information\n");
    return;
  }
  if (!si->default_source_name) {
    fprintf(stderr, "No default source set.\n");
    return;
  }

  pa_operation_unref(pa_context_get_source_info_by_name(
      c, si->default_source_name, context_get_source_info_callback, NULL));
}

static void context_state_callback(pa_context* c, void* user_data) {
  LAudio* l_audio = (LAudio*)user_data;
  fprintf(stderr, "%d\n", pa_context_get_state(c));
  if (pa_context_get_state(c) == PA_CONTEXT_READY) {
    pa_operation_unref(
        pa_context_get_server_info(c, context_get_server_info_callback, NULL));
    pa_context_get_source_info_list(
        c, (pa_source_info_cb_t)source_info_callback, l_audio);
  } else {
    fprintf(stderr, "pa not ready\n");
  }
}

void l_audio_init(LAudio* l_audio) {
  l_audio->mainloop = pa_glib_mainloop_new(NULL);

  l_audio->ctx =
      pa_context_new(pa_glib_mainloop_get_api(l_audio->mainloop), NULL);
  g_assert(l_audio->ctx);

  int r = pa_context_connect(l_audio->ctx, NULL,
                             PA_CONTEXT_NOAUTOSPAWN | PA_CONTEXT_NOFAIL, NULL);
  g_assert(r == 0);

  fprintf(stderr, "context state: %d\n", pa_context_get_state(l_audio->ctx));
  fprintf(stderr, "context connect: %d\n", r);

  pa_context_set_state_callback(l_audio->ctx, context_state_callback, l_audio);

  fprintf(stderr, "context state: %d\n", pa_context_get_state(l_audio->ctx));
}

void l_audio_destruct(LAudio* l_audio) {
  pa_context_unref(l_audio->ctx);
  pa_glib_mainloop_free(l_audio->mainloop);
}