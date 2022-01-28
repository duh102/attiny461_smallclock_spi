#include <avr/io.h>
#include <stdint.h>
#include "tinyspi.h"


void setup_spi() {
  USICR &= ~(1<<USICS0);

  // Set all of B to inputs except SCK, MOSI, and CLOCKCS
  DDRB = SCK | MOSI | CLOCKCS;
  PORTB |= CLOCKCS;
}

uint8_t inline spi_send(uint8_t byte) {
  USIDR = byte;
  // Reset the status bits so we can track the progress 
  USISR = (1<<USIOIF);
  // Clock the bus and wait until the status register indicates the byte transmission is complete
  while( !(USISR & (1<<USIOIF)) ) {
    USICR |= (1<<USITC);
  }
  // During the transmission we'll get the data from the slave clocked into the buffer, so we can just return it
  return USIDR;
}
void spi_transaction(uint8_t* combuf, uint8_t* recbuf, uint8_t send_len, uint8_t recv_len) {
  // Configure the USI utility to SPI mode 0
  USICR &= ~((1<<USISIE) | (1<<USIOIE) | (1<<USIWM1));
  USICR |= (1<<USIWM0) | (1<<USICS1) | (1<<USICLK);
  // Enable the peripheral using the CS line
  PORTB &= ~CLOCKCS;
  // When we send a command, we ignore the received contents
  for(uint8_t i = 0; i < send_len; i++) {
    spi_send(combuf[i]);
  }
  // When we're receiving data, the slave will ignore the sent contents
  for(uint8_t i = 0; i < recv_len; i++) {
    recbuf[i] = spi_send(0);
  }
  PORTB |= CLOCKCS;
  // And then disable the USI utility
  USICR &= ~((1<<USIWM1) | (1<<USIWM0));
}
