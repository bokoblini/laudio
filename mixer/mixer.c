#include <gtk/gtk.h>
#include <stdio.h>

#include "audio.h"

typedef struct {
  LAudio *l_audio;
} LMixer;

static gboolean slider_right_user_input(GtkRange *slider, GtkScrollType *scroll, gdouble value, gpointer user_data) {
  fprintf(stderr, "new value right: %f \n", value);

  return FALSE;
}

static gboolean slider_left_user_input(GtkRange *slider, GtkScrollType *scroll, gdouble value, gpointer user_data) {
  LMixer *l_mixer = (LMixer*)user_data;

  fprintf(stderr, "new value left: %f \n", value);

  l_audio_set_volume(l_mixer->l_audio, value);
  return FALSE;
}

static void activate(GtkApplication *app, gpointer user_data) {
  LMixer *l_mixer = (LMixer *)user_data;

  l_audio_init(l_mixer->l_audio);

  GtkWidget *slider_left =
      gtk_scale_new_with_range(GTK_ORIENTATION_VERTICAL, 0.0, 100.0, 1.0);
  gtk_range_set_value(GTK_RANGE(slider_left), 0.0);
  gtk_range_set_inverted(GTK_RANGE(slider_left), TRUE);
  g_signal_connect(slider_left, "change-value", G_CALLBACK(slider_left_user_input), user_data);

  GtkWidget *slider_right =
      gtk_scale_new_with_range(GTK_ORIENTATION_VERTICAL, 0.0, 100.0, 1.0);
  gtk_range_set_value(GTK_RANGE(slider_right), 0.0);
  gtk_range_set_inverted(GTK_RANGE(slider_right), TRUE);
  g_signal_connect(slider_right, "change-value", G_CALLBACK(slider_right_user_input), user_data);

  GtkWidget *control_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 32);
  gtk_box_append(GTK_BOX(control_box), slider_left);
  gtk_box_append(GTK_BOX(control_box), slider_right);

  GtkWidget *center_box = gtk_center_box_new();
  gtk_center_box_set_center_widget(GTK_CENTER_BOX(center_box), control_box);

  GtkWidget *window = gtk_application_window_new(app);
  gtk_window_set_title(GTK_WINDOW(window), "LAudio Supermixer Platinum");
  gtk_window_set_default_size(GTK_WINDOW(window), 480, 720);

  gtk_window_set_child(GTK_WINDOW(window), center_box);
  gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char **argv) {
  LAudio l_audio;
  LMixer l_mixer;
  l_mixer.l_audio = &l_audio;

  GtkApplication *app = gtk_application_new(NULL, G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect(app, "activate", G_CALLBACK(activate), &l_mixer);

  int status = g_application_run(G_APPLICATION(app), argc, argv);
  g_object_unref(app);

  l_audio_destruct(&l_audio);
  return status;
}