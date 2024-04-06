#include <pulse/pulseaudio.h>
#include <stdint.h>
#include <stdio.h>

//const char* serviceid = "Laudio streamdumper";
const char* serviceid = "PulseAudio Volume Meter";

void print_audio_data(float* data, size_t len) {
  static int64_t n = 0;
  for (size_t i = 0; i < len; ++i) {
    printf("%ld %f\n", n++, data[i]);
  }
  fflush(stdout);
}

static void stream_read_callback(pa_stream* s, size_t l, void*) {
  fprintf(stderr, "stream_read_callback1\n");
  pa_context* context = pa_stream_get_context(s);
  const void* p;

  fprintf(stderr, "stream_read_callback2\n");

  if (pa_stream_peek(s, &p, &l) < 0) {
    fprintf(stderr, "pa_stream_peek() failed: %s",
            pa_strerror(pa_context_errno(context)));
    return;
  }

  print_audio_data((float*)p, l / sizeof(float));

  pa_stream_drop(s);
}

static void stream_state_callback(pa_stream *s, void *) {
  fprintf(stderr, "stream_state_callback: %d\n", pa_stream_get_state(s));
}

static void create_stream(pa_context* context, const char* name,
                          const char* description, const pa_sample_spec* ss,
                          const pa_channel_map* cmap) {
  pa_sample_spec nss;
  nss.format = PA_SAMPLE_FLOAT32;
  nss.rate = ss->rate;
  nss.channels = ss->channels;

  pa_stream* stream =
      pa_stream_new(context, serviceid, &nss, cmap);
  pa_stream_set_state_callback(stream, stream_state_callback, NULL);
  pa_stream_set_read_callback(stream, stream_read_callback, NULL);
}

static void context_get_source_info_callback(pa_context* context,
                                             const pa_source_info* si,
                                             int is_last, void*) {
  fprintf(stderr, "source_info_callback 1\n");
  if (!si) return;
  fprintf(stderr, "source_info_callback 2\n");
  create_stream(context, si->name, si->description, &si->sample_spec,
                &si->channel_map);
}

static void context_get_server_info_callback(pa_context* c,
                                             const pa_server_info* si, void*) {
  fprintf(stderr, "server_info_callback 1\n");
  if (!si) {
    fprintf(stderr, "Failed to get server information\n");
    return;
  }
  fprintf(stderr, "server_info_callback 2\n");
  if (!si->default_source_name) {
    fprintf(stderr, "No default source set.\n");
    return;
  }
  fprintf(stderr, "server_info_callback 3\n");
  pa_operation_unref(pa_context_get_source_info_by_name(
      c, si->default_source_name, context_get_source_info_callback, NULL));
}

static void context_state_callback(pa_context* c, void*) {
  if (pa_context_get_state(c) == PA_CONTEXT_READY) {
    fprintf(stderr, "context ready\n");
    pa_operation_unref(
        pa_context_get_server_info(c, context_get_server_info_callback, NULL));
  }
}

int main(int argc, char* argv[]) {
  pa_mainloop* m;
  m = pa_mainloop_new();
  pa_context* context =
      pa_context_new(pa_mainloop_get_api(m), serviceid);

  pa_context_set_state_callback(context, context_state_callback, NULL);
  pa_context_connect(context, NULL, PA_CONTEXT_NOAUTOSPAWN, NULL);

  int rc = pa_mainloop_run(m, NULL);
  if (rc < 0) {
    fprintf(stderr, "pulseaudio returned with errror\n");
    return 1;
  }
  return 0;
}