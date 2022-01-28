#include <avr/io.h>
#include <stdint.h>
#include "tinyspi.h"


void setup_spi() {
  // Set all of B to inputs except SCK, MOSI, and CLOCKCS
  DDRB = SCK | MOSI | CLOCKCS;
  PORTB |= CLOCKCS;
}

uint8_t spi_send(uint8_t byte) {
  uint8_t temp = 0;
  // Clock the bus and wait until the status register indicates the byte transmission is complete
  for(uint8_t i = 0; i < 8; i++) {
    if(byte & (0x80>>i)) {
      PORTB |= MOSI;
    } else {
      PORTB &= ~MOSI;
    }
    PINB |= SCK;
    PINB |= SCK;
    temp <<= 1;
    temp |= (PINB & MISO)>>MISO_PIN;
  }
  return temp;
}
uint8_t spi_recv() {
  uint8_t temp = 0;
  // Clock the bus and wait until the status register indicates the byte transmission is complete
  for(uint8_t i = 0; i < 8; i++) {
    PINB |= SCK;
    PINB |= SCK;
    temp <<= 1;
    temp |= (PINB & MISO)>>MISO_PIN;
  }
  return temp;
}
void spi_transaction(uint8_t* combuf, uint8_t* recbuf, uint8_t send_len, uint8_t recv_len) {
  // Enable the peripheral using the CS line
  PORTB &= ~CLOCKCS;
  // When we send a command, we ignore the received contents
  for(uint8_t i = 0; i < send_len; i++) {
    spi_send(combuf[i]);
  }
  // When we're receiving data, the slave will ignore the sent contents
  for(uint8_t i = 0; i < recv_len; i++) {
    recbuf[i] = spi_recv();
  }
  PORTB |= CLOCKCS;
  // And then disable the USI utility
  USICR &= ~((1<<USIWM1) | (1<<USIWM0));
}
