#ifndef __SEGMENTS_H__
#define __SEGMENTS_H__
#include <stdint.h>
#include <avr/pgmspace.h>

const uint8_t segments[] PROGMEM = {
 0b10111111, 0b11111, //0
 0b10001100, 0b10001, //1
 0b11101111, 0b11110, //2
 0b11101111, 0b11101, //3
 0b11111101, 0b10001, //4
 0b11110111, 0b11101, //5
 0b11110111, 0b11111, //6
 0b10001111, 0b10001, //7
 0b11111111, 0b11111, //8
 0b11111111, 0b10001, //9
};

#endif //__SEGMENTS_H__