#include <gtk/gtk.h>
#include <stdio.h>

static void activate(GtkApplication *app, gpointer user_data) {
  GtkWidget *slider_left =
      gtk_scale_new_with_range(GTK_ORIENTATION_VERTICAL, 0.0, 100.0, 1.0);
  gtk_range_set_value(GTK_RANGE(slider_left), 0.0);

  GtkWidget *slider_right =
      gtk_scale_new_with_range(GTK_ORIENTATION_VERTICAL, 0.0, 100.0, 1.0);
  gtk_range_set_value(GTK_RANGE(slider_right), 0.0);

  GtkWidget *control_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 32);
  gtk_box_append(GTK_BOX(control_box), slider_left);
  gtk_box_append(GTK_BOX(control_box), slider_right);

  GtkWidget *center_box = gtk_center_box_new();
  gtk_center_box_set_center_widget(GTK_CENTER_BOX(center_box), control_box);

  GtkWidget *window = gtk_application_window_new(app);
  gtk_window_set_title(GTK_WINDOW(window), "lAudio Supermixer Platinum");

  gtk_window_set_child(GTK_WINDOW(window), center_box);
  gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char **argv) {
  GtkApplication *app = gtk_application_new(NULL, G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

  int status = g_application_run(G_APPLICATION(app), argc, argv);
  g_object_unref(app);

  return status;
}