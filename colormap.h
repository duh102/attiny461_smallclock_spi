#ifndef __COLORMAP_H__
#define __COLORMAP_H__
#include <stdint.h>
#include <avr/pgmspace.h>

#define COLORMAP_LEN 36
const uint8_t colormap[COLORMAP_LEN][3] PROGMEM = {
  {6, 0, 0},
  {6, 2, 0},
  {6, 3, 0},
  {6, 4, 0},
  {6, 4, 0},
  {6, 5, 0},
  {6, 6, 0},
  {5, 6, 0},
  {4, 6, 0},
  {4, 6, 0},
  {3, 6, 0},
  {2, 6, 0},
  {0, 6, 0},
  {0, 6, 2},
  {0, 6, 3},
  {0, 6, 4},
  {0, 6, 4},
  {0, 6, 5},
  {0, 6, 6},
  {0, 5, 6},
  {0, 4, 6},
  {0, 4, 6},
  {0, 3, 6},
  {0, 2, 6},
  {0, 0, 6},
  {2, 0, 6},
  {3, 0, 6},
  {4, 0, 6},
  {4, 0, 6},
  {5, 0, 6},
  {6, 0, 6},
  {6, 0, 5},
  {6, 0, 4},
  {6, 0, 4},
  {6, 0, 3},
  {6, 0, 2},
};

#endif // __COLORMAP_H__
