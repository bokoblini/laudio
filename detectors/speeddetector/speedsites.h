# ifndef LAUDIO_SPEEDSITES_H
# define LAUDIO_SPEEDSITES_H

#include "../../frames.h"

typedef struct {
  char* input_file;
  int interval;
  double growth;
  Frames *frames;
  double speed;
} InputData;

InputData input_data_setup(int interval, double growth, Frames* frames);
char* output(InputData input);



#endif // LAUDIO_SPEEDSITES_H