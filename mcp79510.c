#include <stdint.h>
#include <stdbool.h>
#include "tinyspi.h"
#include "mcp79510.h"

uint8_t commbuf[5];
uint8_t readbuf[3];

// Enable timekeeping, set to 24h mode, and optionally enable battery
void initRTC(bool enableBat) {
  // Read the seconds register to see if we need to enable the oscillator
  commbuf[0] = RTC_READ;
  commbuf[1] = RTCSEC;
  spi_transaction(commbuf, readbuf, 2, 1);
  // and update it if we need to
  if(! (readbuf[0] & (1<<STARTOSC))) {
    commbuf[0] = RTC_WRITE;
    commbuf[1] = RTCSEC;
    commbuf[2] = readbuf[0] | (1<<STARTOSC);
    spi_transaction(commbuf, readbuf, 3, 0);
  }
  // Read the hours register to see if we need to enable 24 hour mode
  commbuf[0]=RTC_READ;
  commbuf[1]=RTCHOUR;
  spi_transaction(commbuf, readbuf, 2, 1);
  // and update it if we need to
  if(readbuf[0] & (1<<HOUR12)) {
    commbuf[0] = RTC_WRITE;
    commbuf[1] = RTCHOUR;
    commbuf[2] = readbuf[0] & (~(1<<HOUR12));
    spi_transaction(commbuf, readbuf, 3, 0);
  }
  if(enableBat) {
    commbuf[0] = RTC_READ;
    commbuf[1] = RTCWKDAY;
    spi_transaction(commbuf, readbuf, 2, 1);
    if( !(readbuf[0] & (1<<VBATEN)) ) {
      commbuf[0] = RTC_WRITE;
      commbuf[1] = RTCWKDAY;
      commbuf[2] = readbuf[0] | (1<<VBATEN);
      spi_transaction(commbuf, readbuf, 3, 0);
    }
  }
}

// Read a byte from SRAM
uint8_t readSRAM(uint8_t addr) {
  commbuf[0] = RTC_READ;
  commbuf[1] = addr;
  spi_transaction(commbuf, readbuf, 2, 1);
  return readbuf[0];
}
// Write a byte to SRAM
void writeSRAM(uint8_t addr, uint8_t value) {
  commbuf[0] = RTC_WRITE;
  commbuf[1] = addr;
  commbuf[2] = value;
  spi_transaction(commbuf, readbuf, 3, 0);
}

// Read a byte from EEPROM
uint8_t readEEPROM(uint8_t addr) {
  commbuf[0] = RTC_EEREAD;
  commbuf[1] = addr;
  spi_transaction(commbuf, readbuf, 2, 1);
  return readbuf[0];
}
// Write a byte to EEPROM
void writeEEPROM(uint8_t addr, uint8_t value) {
  commbuf[0] = RTC_EEWRITE;
  commbuf[1] = addr;
  commbuf[2] = value;
  spi_transaction(commbuf, readbuf, 3, 0);
}

// Read data from the protected EEPROM
void readProtectedEEPROM(uint8_t startAddr, uint8_t* buffer, uint8_t readLen) {
  commbuf[0] = RTC_IDREAD;
  commbuf[1] = startAddr;
  spi_transaction(commbuf, buffer, 2, readLen);
}

// Read just the hour
uint8_t readHour() {
  commbuf[0] = RTC_READ;
  commbuf[1] = RTCHOUR;
  spi_transaction(commbuf, readbuf, 2, 1);
  return readbuf[0];
}
// Read just the minute
uint8_t readMinute() {
  commbuf[0] = RTC_READ;
  commbuf[1] = RTCMIN;
  spi_transaction(commbuf, readbuf, 2, 1);
  return readbuf[0];
}
// Read just the second
uint8_t readSecond() {
  commbuf[0] = RTC_READ;
  commbuf[1] = RTCSEC;
  spi_transaction(commbuf, readbuf, 2, 1);
  return readbuf[0];
}
// Write just the hour
void writeHour(uint8_t hour) {
  commbuf[0] = RTC_WRITE;
  commbuf[1] = RTCHOUR;
  commbuf[2] = hour;
  spi_transaction(commbuf, readbuf, 3, 0);
}
// Write just the minute
void writeMinute(uint8_t minute) {
  commbuf[0] = RTC_WRITE;
  commbuf[1] = RTCMIN;
  commbuf[2] = minute;
  spi_transaction(commbuf, readbuf, 3, 0);
}
// Write just the second
void writeSecond(uint8_t second) {
  commbuf[0] = RTC_WRITE;
  commbuf[1] = RTCSEC;
  commbuf[2] = second;
  spi_transaction(commbuf, readbuf, 3, 0);
}

// Read the entire time string and place the value in the given addresses
void readTime(uint8_t *hour24, uint8_t *minute, uint8_t *second) {
  commbuf[0] = RTC_READ;
  commbuf[1] = RTCSEC;
  spi_transaction(commbuf, readbuf, 2, 3);
  *second = 10*(readbuf[0]&(1<<SECTEN2 | 1<<SECTEN1 | 1<<SECTEN0)) + (readbuf[0]&(1<<SECONE3 | 1<<SECONE2 | 1<<SECONE1 | 1<<SECONE0));
  *minute = 10*(readbuf[1]&(1<<MINTEN2 | 1<<MINTEN1 | 1<<MINTEN0)) + (readbuf[1]&(1<<MINONE3 | 1<<MINONE2 | 1<<MINONE1 | 1<<MINONE0));
  *hour24 = 10*(readbuf[2]&(1<<HRTEN1 | 1<<HRTEN0)) + (readbuf[2]&(1<<HRONE3 | 1<<HRONE2 | 1<<HRONE1 | 1<<MINONE0));
}
// Write the entire time string
void writeTime(uint8_t hour24, uint8_t minute, uint8_t second) {
  commbuf[0] = RTC_READ;
  commbuf[1] = RTCSEC;
  spi_transaction(commbuf, readbuf, 2, 3);
  commbuf[0] = RTC_WRITE;
  commbuf[2] = (readbuf[0] & (1<<STARTOSC)) | (((second/10)<<4) | ((second%10) & 0b1111));
  commbuf[3] = ((minute/10)<<4) | ((minute%10) & 0b1111);
  commbuf[4] = (readbuf[2] & ((1<<TRIMSIGN) | (1<<HOUR12))) | (((hour24/10)<<4) | (hour24%10 & 0b1111));
  spi_transaction(commbuf, readbuf, 5, 0);
}

// Set the crystal trimming to this value
// If positiveTrim is true, *add* clock cycles (slow crystal), otherwise remove (fast crystal)
// If coarse is true, add/remove clock cycles every second, otherwise every minute
// Each additional 1 trim equates to 2 clock cycles per period
void setTrim(bool positiveTrim, bool coarse, uint8_t trimVal) {
}

// Enable or disable the multi-function pin to output a square wave at the selected frequency
// See [MFP output selections] for the acceptable values
// Users can select from 1Hz, 4.096KHz, 8.192KHz, or the raw 32.768KHz output
// Note that the MFP will not enable if coarse trimming is enabled.
// Also note that if non-coarse trimming is applied, the 1Hz, 4.096KHz,
//  and 8.192KHz outputs will be scaled appropriately.
void setMFPOutput(bool enableMFP, uint8_t mfpFunc) {
  commbuf[0] = RTC_WRITE;
  commbuf[1] = CONTROL;
  commbuf[2] = (enableMFP?(1<<SQWEN):0) | (mfpFunc & 0b11);
  spi_transaction(commbuf, readbuf, 3, 0);
}

// Output if oscillator is running or if it is stopped
bool oscRunning() {
  commbuf[0] = RTC_READ;
  commbuf[1] = RTCWKDAY;
  spi_transaction(commbuf, readbuf, 2, 1);
  return readbuf[0]&(1<<OSCRUN);
}

// Read the STATUS register
uint8_t readStatus() {
  commbuf[0] = RTC_SRREAD;
  spi_transaction(commbuf, readbuf, 1, 1);
  return readbuf[0];
}
// Write to the STATUS register; only BLOCKPROTECT1 and BLOCKPROTECT0 can be written
void writeStatus(uint8_t value) {
  commbuf[0] = RTC_SRWRITE;
  commbuf[1] = value;
  spi_transaction(commbuf, readbuf, 2, 0);
}
