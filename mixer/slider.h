#ifndef LAUDIO_SLIDER_H
#define LAUDIO_SLIDER_H

#include <gtk/gtk.h>

#include "led.h"

typedef struct {
  GtkWidget* scale;
  LAudioLed led;
  GtkWidget* box;
} LAudioSlider;

void l_audio_slider_setup(LAudioSlider* slider);

#endif  // LAUDIO_SLIDER_H