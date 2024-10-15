#include "speedsites.h"

InputData input_data_setup(int interval, double growth, Frames* frames) {
  double speed = growth / (double)interval;

  InputData input_data;
  input_data.interval = interval;
  input_data.growth = growth;
  input_data.frames = frames;
  input_data.speed = speed;

  return input_data;
}

OutputData run_speed_site_detector(OutputData output, InputData input) {
  float* return_list = (float*)malloc(input.frames->frame_len * sizeof(float));

  

  free(input);
  return output;
}

free_output_data(OutputData output) {
  free(output);
}
