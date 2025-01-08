#include "led.h"

const int AREA_SIZE = 10;

void draw_function(GtkDrawingArea *area, cairo_t *cr, int width, int height,
                   gpointer user_data) {
  LAudioLed *led = (LAudioLed *)user_data;
  GdkRGBA color;

  if (led->state) {
    color.red = 1.0f;
    color.blue = 0.0f;
    color.green = 0.0f;
    color.alpha = 1.0f;
  } else {
    color.red = 0.0f;
    color.blue = 0.0f;
    color.green = 1.0f;
    color.alpha = 1.0f;
  }

  cairo_arc(cr, width / 2.0, height / 2.0, MIN(width, height) / 2.0, 0,
            2 * G_PI);

  gdk_cairo_set_source_rgba(cr, &color);

  cairo_fill(cr);
}

void l_audio_led_setup(LAudioLed *led) {
  led->drawing_area = gtk_drawing_area_new();
  gtk_drawing_area_set_content_width(GTK_DRAWING_AREA(led->drawing_area),
                                     AREA_SIZE);
  gtk_drawing_area_set_content_height(GTK_DRAWING_AREA(led->drawing_area),
                                      AREA_SIZE);
  gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(led->drawing_area),
                                 draw_function, led, NULL);
}

void l_audio_led_alert(LAudioLed *led) {
  led->state = 1;
  gtk_widget_queue_draw(led->drawing_area);
}