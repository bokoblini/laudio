#include <gtk/gtk.h>
#include <stdio.h>

#include "audio.h"
#include "led.h"
#include "processor.h"
#include "slider.h"

typedef struct {
  LAudio *l_audio;
  LMultiProcessor *l_processor;
  LAudioSlider slider[2];
} LMixer;

static gboolean slider_right_user_input(GtkRange *slider, GtkScrollType *scroll,
                                        gdouble value, gpointer user_data) {
  LMixer *l_mixer = (LMixer *)user_data;

  fprintf(stderr, "new value right: %f \n", value);

  l_audio_set_volume(l_mixer->l_audio, value, 0);
  return FALSE;
  return FALSE;
}

static gboolean slider_left_user_input(GtkRange *slider, GtkScrollType *scroll,
                                       gdouble value, gpointer user_data) {
  LMixer *l_mixer = (LMixer *)user_data;

  fprintf(stderr, "new value left: %f \n", value);

  l_audio_set_volume(l_mixer->l_audio, value, 1);
  return FALSE;
}

static gboolean proba_piros(gpointer user_data) {
  LAudioLed* led = (LAudioLed*)user_data;
  l_audio_led_alert(led);
  return G_SOURCE_REMOVE;
}

static void activate(GtkApplication *app, gpointer user_data) {
  LMixer *l_mixer = (LMixer *)user_data;

  l_multi_processor_setup(l_mixer->l_processor, 2);
  l_audio_init(l_mixer->l_audio, l_mixer->l_processor);

  l_audio_slider_setup(&l_mixer->slider[0]);
  l_audio_slider_setup(&l_mixer->slider[1]);

  g_signal_connect(l_mixer->slider[0].scale, "change-value",
                   G_CALLBACK(slider_left_user_input), user_data);
  g_signal_connect(l_mixer->slider[1].scale, "change-value",
                   G_CALLBACK(slider_right_user_input), user_data);

  GtkWidget *control_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 32);
  gtk_box_append(GTK_BOX(control_box), l_mixer->slider[0].box);
  gtk_box_append(GTK_BOX(control_box), l_mixer->slider[1].box);

  GtkWidget *center_box = gtk_center_box_new();
  gtk_center_box_set_center_widget(GTK_CENTER_BOX(center_box), control_box);

  GtkWidget *window = gtk_application_window_new(app);
  gtk_window_set_title(GTK_WINDOW(window), "LAudio Supermixer Platinum");
  gtk_window_set_default_size(GTK_WINDOW(window), 480, 720);

  gtk_window_set_child(GTK_WINDOW(window), center_box);
  gtk_window_present(GTK_WINDOW(window));

  g_timeout_add_seconds(3, proba_piros, &l_mixer->slider[0].led);
}

int main(int argc, char **argv) {
  LAudio l_audio;
  LMixer l_mixer;
  l_mixer.l_audio = &l_audio;
  LMultiProcessor l_processor;
  l_mixer.l_processor = &l_processor;
  l_audio.l_processor = &l_processor;

  GtkApplication *app = gtk_application_new(NULL, G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect(app, "activate", G_CALLBACK(activate), &l_mixer);

  int status = g_application_run(G_APPLICATION(app), argc, argv);
  g_object_unref(app);

  l_audio_destruct(&l_audio);
  return status;
}