#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include "ws2812.h"
#include "segments.h"
#include "colormap.h"
#include <stdbool.h>
#include <avr/pgmspace.h>
#include "tinyspi.h"
//#include "mcp79510.h"

#define MFP (1<<PB6)
#define HH  (1<<PA0)
#define MM  (1<<PA1)

#define MAX_LED 78
#define HT_START 0
#define HO_START 13
#define MT_START 26
#define MO_START 39
#define ST_START 52
#define SO_START 65
// How many 10ms increments should we count until we activate the function the button is indicating again
#define BUTTONDOWN_RESET 20
// How many steps in innerstate equates to one step of state
#define STATE_STEPS 6
uint8_t innerstate = 0;
// The state of the rainbow
uint8_t state = 0;
// reserving a byte for loop variant
uint8_t curLed;
// we need 78 LEDs in 8-bit chunks
uint8_t led_enabled[10];

uint8_t buttonDown = 0;
uint8_t hour = 0;
uint8_t hour12 = 0;
uint8_t minute = 0;
uint8_t second = 0;
bool checkButton = false;
bool updateDigits = false;
bool updateColors = false;

uint8_t buttonState;
uint8_t temp, temp2, temp3;

void updateDigit(uint8_t startLED, uint8_t value);

/*ISR(PCINT_vect) {
  checkButton=true;
}*/

/*ISR(INT0_vect) {
  second++;
  updateDigits = true;
}*/

void updateDigit(uint8_t startLED, uint8_t value) {
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

  // Enable SPI
  setup_spi();

  // Enable interrupts on INT0 and the pin change interrupt
  // PCIE1 enables pin change interrupts on PA0-PA7 and PB4-PB7 (HH is on PA0 and MM is on PA1)
  // INT0 enables the external interrupt on PB6 (where the MFP is)
  //GIMSK = (1<<INT0) | (1<<PCIE1);
  //GIMSK = (1<<INT0);
  // Set INT0 to trigger on rising edge
  //MCUCR |= (1<<ISC00) | (1<<ISC01);
  // Enable pin change interrupt on HH and MM
  //PCMSK0 = (1<<PCINT0) | (1<<PCINT1);

  uint8_t buf[] = {0b00110011,0x02,3,4,5,6};
  spi_transaction(buf, buf, 2, 6);

  /*
  // Make sure the RTC is enabled and the battery is enabled
  initRTC(true);
  // Enable MFP output at 1Hz
  setMFPOutput(true, MFP_1HZ);
  // Retrieve the data
  readTime(&hour, &minute, &second);
  /*updateDigits = oscRunning();
  if(!updateDigits) {
  }*/
  //updateDigits = true;

  // Enable the interrupts
  //sei();

  while(1) {
    updateDigit(HT_START, (buf[0+state]&0b11110000)>>4);
    updateDigit(HO_START, buf[0+state]&0b1111);
    updateDigit(MT_START, (buf[1+state]&0b11110000)>>4);
    updateDigit(MO_START, buf[1+state]&0b1111);
    updateDigit(ST_START, (buf[2+state]&0b11110000)>>4);
    updateDigit(SO_START, buf[2+state]&0b1111);
    updateColors = true;
    state = (state + 3) % 6;
    /*
    if(!(checkButton || updateDigits || updateColors)) {
      innerstate = (innerstate+1) % STATE_STEPS;
      if(innerstate == 0) {
        state+=1;
        updateColors = true;
      }
      _delay_ms(50);
      continue;
    }

    if(checkButton) {
      buttonState = (~PINA) & (HH | MM);
      if(!buttonState) {
        checkButton=false;
        updateDigits = true;
        buttonDown = 0;
        readTime(&hour, &minute, &second);
      } else {
        if(buttonDown == 0) {
          buttonDown = BUTTONDOWN_RESET;
          if(buttonState&MM) {
            minute = (minute + 1) % 60;
            second = 0;
          }
          if(buttonState&HH) {
            hour = (hour + 1) % 24;
          }
          writeTime(hour, minute, second);
          updateDigits = true;
        }
        buttonDown--;
        _delay_ms(10);
      }
    }
  
    if(updateDigits) {
      updateDigits = false;
      
      if( second >= 60 ) {
        second %= 60;
        minute++;
        if(minute >= 60) {
          minute %= 60;
          hour = (hour + 1) % 24;
          hour12 = (hour == 0 || hour == 12)? 12 : hour % 12;
        }
        // re-read the time every 30 minutes, just to make sure we're not losing too many seconds during the LED burst
        if(minute == 0 || minute == 30) {
          readTime(&hour, &minute, &second);
        }
      }

      updateDigit(HT_START, hour12/10);
      updateDigit(HO_START, hour12%10);
      updateDigit(MT_START, minute/10);
      updateDigit(MO_START, minute%10);
      updateDigit(ST_START, second/10);
      updateDigit(SO_START, second%10);
      updateColors = true;
    }
    */
    if(updateColors) {
      updateColors = false;
      cli();
      for(curLed = 0; curLed < MAX_LED; curLed++) {
        temp = ((curLed/4)+state)%COLORMAP_LEN;
        temp2 = pgm_read_byte(&colormap[temp][1]);
        temp3 = pgm_read_byte(&colormap[temp][2]);
        temp = pgm_read_byte(&colormap[temp][0]);
        if( led_enabled[curLed/8] & (1<<(curLed%8)) ) {
          ws2812_set_single(temp, temp2, temp3);
        } else {
          ws2812_set_single(0, 0, 0);
        }
      }
      //sei();
    }
    _delay_ms(1000);
  }
}
