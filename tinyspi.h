#ifndef __TINYSPI_H__
#define __TINYSPI_H__

#define MISO_PIN PB0
#define SCK      (1<<PB2)
#define MOSI     (1<<PB1)
#define MISO     (1<<MISO_PIN)
#define CLOCKCS  (1<<PB3)

void setup_spi();
uint8_t spi_send(uint8_t byte);
void spi_transaction(uint8_t* combuf, uint8_t* recbuf, uint8_t send_len, uint8_t recv_len);

#endif
