#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include "ws2812.h"
#include "segments.h"
#include "colormap.h"
#include <stdbool.h>
#include <avr/pgmspace.h>

#define SCK      (1<<PB2)
#define SI       (1<<PB1)
#define SO       (1<<PB0)
#define MAX_LED 78
#define HT_START 0
#define HO_START 13
#define MT_START 26
#define MO_START 39
#define ST_START 52
#define SO_START 65
// The state of the rainbow
uint8_t state = 0;
// reserving a byte for loop variant
uint8_t curLed;
// we need 78 LEDs in 8-bit chunks
uint8_t led_enabled[10];

volatile uint8_t hour = 0;
volatile uint8_t hour12 = 0;
volatile uint8_t minute = 0;
volatile uint8_t second = 0;

uint8_t temp, temp2, temp3;

void inline updateDigit(uint8_t startLED, uint8_t value);

void inline updateDigit(uint8_t startLED, uint8_t value) {
  for(curLed = startLED; curLed < (startLED+13); curLed++) {
    temp = curLed-startLED;
    temp2 = pgm_read_byte(&segments[(value*2)+(temp/8)]);
    if( !( temp2 & (1<<(temp%8)) ) ) {
      led_enabled[curLed/8] &= ~(1<<(curLed%8));
      continue;
    }
    led_enabled[curLed/8] |= 1<<(curLed%8);
  }
}

int main() {
  CLKPR = 1<<CLKPCE;   // allow writes to CLKPR
  CLKPR = 0;   // disable system clock prescaler (run at full 8MHz)

  // Enable the display
  ws2812_init();
  for(curLed = 0; curLed < 10; curLed++) {
    led_enabled[curLed] = 0;
  }

  while(1) {
    _delay_ms(300);

    if(state % 4 == 0) {
      hour = (hour+1) % 24;
      hour12 = hour == 0 || hour == 12? 12 : hour % 12;
      minute = (minute+1) % 60;
      second = (second+1) % 60;
    }
    state+=1;
  
    updateDigit(HT_START, hour12/10);
    updateDigit(HO_START, hour12%10);
    updateDigit(MT_START, minute/10);
    updateDigit(MO_START, minute%10);
    updateDigit(ST_START, second/10);
    updateDigit(SO_START, second%10);

    cli();
    for(curLed = 0; curLed < MAX_LED; curLed++) {
      temp = ((curLed/8)+state)%COLORMAP_LEN;
      temp2 = pgm_read_byte(&colormap[temp][1]);
      temp3 = pgm_read_byte(&colormap[temp][2]);
      temp = pgm_read_byte(&colormap[temp][0]);
      if( led_enabled[curLed/8] & (1<<(curLed%8)) ) {
        ws2812_set_single(temp, temp2, temp3);
      } else {
        ws2812_set_single(0, 0, 0);
      }
    }
    sei();
  }
}
