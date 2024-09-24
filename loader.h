#ifndef LAUDIO_LOADER_H
#define LAUDIO_LOADER_H

// Allocates enough memory to hold the data of the whole file,
// and stores that pointer to *buf.
// *buf should be freed via free().
int read_txtfft(char* file_name, float** buf, int* len);

#endif  // LAUDIO_LOADER_H
