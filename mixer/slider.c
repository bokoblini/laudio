#include "slider.h"

void l_audio_slider_setup(LAudioSlider* slider) {
  slider->scale =
      gtk_scale_new_with_range(GTK_ORIENTATION_VERTICAL, 0.0, 100.0, 1.0);
  gtk_range_set_value(GTK_RANGE(slider->scale), 25.0);
  gtk_range_set_inverted(GTK_RANGE(slider->scale), TRUE);

  l_audio_led_setup(&slider->led);

  slider->box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
  gtk_widget_set_vexpand(slider->box, TRUE);
  gtk_widget_set_vexpand(slider->scale, TRUE);
  gtk_widget_set_margin_top(slider->led.drawing_area, 20);
  gtk_box_append(GTK_BOX(slider->box), slider->led.drawing_area);
  gtk_box_append(GTK_BOX(slider->box), slider->scale);
}
