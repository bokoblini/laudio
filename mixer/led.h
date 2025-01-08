#ifndef LAUDIO_LED_H
#define LAUDIO_LED_H

#include <gtk/gtk.h>

typedef struct {
  GtkWidget* drawing_area;
  int state;  // 1: alerting, 0: not
} LAudioLed;

void l_audio_led_setup(LAudioLed *led);

void l_audio_led_alert(LAudioLed *led);

#endif  // LAUDIO_LED_H