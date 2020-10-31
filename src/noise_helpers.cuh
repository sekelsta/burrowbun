#ifndef NOISE_HELPERS_CUH
#define NOISE_HELPERS_CUH

__device__
void adjustLocation(int x, int y, int width, int height, double scale, double *x_out, double *y_out, double *z_out);

#endif
