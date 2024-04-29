#include <gtk/gtk.h>

static void print_slider (GtkRange *slider, gpointer) {
  fprintf(stderr, "slider: %f\n", gtk_range_get_value(slider));
}

static void draw_function(GtkDrawingArea *area, cairo_t *cr, int width,
                          int height, gpointer data) {
  GdkRGBA color;
  color.red = 1.0f;
  color.blue = 0.0f;
  color.green = 0.0f;
  color.alpha = 1.0f;

  cairo_arc(cr, width / 2.0, height / 2.0, MIN(width, height) / 2.0, 0,
            2 * G_PI);

  gdk_cairo_set_source_rgba(cr, &color);

  cairo_fill(cr);
}

static void activate(GtkApplication *app, gpointer user_data) {
  GtkWidget *window = gtk_application_window_new(app);
  gtk_window_set_title(GTK_WINDOW(window), "Hello");
  gtk_window_set_default_size(GTK_WINDOW(window), 200, 200);


  GtkWidget *area = gtk_drawing_area_new();
  gtk_drawing_area_set_content_width(GTK_DRAWING_AREA(area), 100);
  gtk_drawing_area_set_content_height(GTK_DRAWING_AREA(area), 100);
  gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(area), draw_function, NULL,
                                 NULL);

  GtkWidget *slider = gtk_scale_new_with_range (GTK_ORIENTATION_HORIZONTAL, 0.0, 100.0, 1.0);
  g_signal_connect (slider, "value-changed", G_CALLBACK (print_slider), NULL); 
  gtk_range_set_value(GTK_RANGE(slider), 0.0);

  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_box_append(GTK_BOX(box), area);
  gtk_box_append(GTK_BOX(box), slider);

  gtk_window_set_child(GTK_WINDOW(window), box);
  gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char **argv) {
  GtkApplication *app;
  int status;

  app = gtk_application_new("org.gtk.example", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
  status = g_application_run(G_APPLICATION(app), argc, argv);
  g_object_unref(app);

  return status;
}