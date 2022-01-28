#ifndef __MCP79510_H__
#define __MCP79510_H__
#include <stdint.h>
#include <stdbool.h>

// # Addresses
// ## RTC/SRAM registers
// ### Timekeeping
#define RTCHSEC   0x00
#define RTCSEC    0x01
#define RTCMIN    0x02
#define RTCHOUR   0x03
#define RTCWKDAY  0x04
#define RTCDATE   0x05
#define RTCYEAR   0x07
#define CONTROL   0x08
#define OSCTRIM   0x09
// ### Alarm 0
#define ALM0SEC   0X0C
#define ALM0MIN   0x0D
#define ALM0HOUR  0x0E
#define ALM0WKDAY 0x0F
#define ALM0DATE  0x10
#define ALM0MTH   0x11
// ### Alarm 1
#define ALM1SEC   0X12
#define ALM1MIN   0x13
#define ALM1HOUR  0x14
#define ALM1WKDAY 0x15
#define ALM1DATE  0x16
#define ALM1MTH   0x17
// ### Power-fail timekeeping
// #### Power-down timestamp
#define PWRDNMIN  0x18
#define PWRDNHOUR 0x19
#define PWRDNDATE 0x1A
#define PWRDNMTH  0x1B
// #### Power-up t imestamp
#define PWRUPMIN  0x1C
#define PWRUPHOUR 0x1D
#define PWRUPDATE 0x1E
#define PWRUPMTH  0x1F
// ### SRAM
#define SRAMSTART 0x20
#define SRAMLEN   64
// ## EEPROM
#define EEPMSTART 0x0
#define EEPMLEN   128
// ## Protected EEPROM
#define EUIADR   0x00
#define USERID    0x08

// ## MFP output selections
#define MFP_1HZ       0b00
#define MFP_4_096KHZ  0b01
#define MFP_8_192KHZ  0b10
#define MFP_32_768KHZ 0b11

// ## Register bits
// ### RTCSEC
#define STARTOSC 7
#define SECTEN2  6
#define SECTEN1  5
#define SECTEN0  4
#define SECONE3  3
#define SECONE2  2
#define SECONE1  1
#define SECONE0  0
// ### RTCMIN
#define MINTEN2  6
#define MINTEN1  5
#define MINTEN0  4
#define MINONE3  3
#define MINONE2  2
#define MINONE1  1
#define MINONE0  0
// ### RTCHOUR
#define TRIMSIGN 7
#define HOUR12   6
#define AMPM     5
#define HRTEN1   5
#define HRTEN0   4
#define HRONE3   3
#define HRONE2   2
#define HRONE1   1
#define HRONE0   0
// ### RTCWKDAY
#define OSCRUN   5
#define PWRFAIL  4
#define VBATEN   3
#define WKDAY2   2
#define WKDAY1   1
#define WKDAY0   0
// ### CONTROL
#define SQWEN    6
#define ALM1EN   5
#define ALM0EN   4
#define EXTOSC   3
#define CRSTRIM  2
#define SQWFS1   1
#define SQWFS0   0
// ### STATUS
#define BLOCKPROTECT1 3
#define BLOCKPROTECT0 2
#define WRITEENLATCH  1
#define WRITEINPROC   0


// # Commands
#define RTC_EEREAD   0b00000011 // Read from EEPROM starting at selected address
#define RTC_EEWRITE  0b00000010 // Write to EEPROM starting at selected address
#define RTC_EEWRDI   0b00000100 // Reset the write enable latch (disable write operations)
#define RTC_EEWREN   0b00000110 // Set the write enable latch (enable write operations)
#define RTC_SRREAD   0b00000101 // Read STATUS register
#define RTC_SRWRITE  0b00000001 // Write STATUS register
#define RTC_READ     0b00010011 // Read data from RTCC/SRAM array beginning at selected address
#define RTC_WRITE    0b00010010 // Write data to RTCC/SRAM array beginning at selected address
#define RTC_UNLOCK   0b00010100 // Unlock the protected EEPROM block for a write operation
#define RTC_IDWRITE  0b00110010 // Write data to the protected EEPROM block beginning at selected address
#define RTC_IDREAD   0b00110011 // Read data from the protected EEPROM block beginning at the selected address
#define RTC_CLRRAM   0b01010100 // Clear all SRAM data to ‘0’

// Enable timekeeping, set to 24h mode, and optionally enable battery
void initRTC(bool enableBat);

// Read a byte from SRAM
uint8_t readSRAM(uint8_t addr);
// Write a byte to SRAM
void writeSRAM(uint8_t addr, uint8_t value);

// Read a byte from EEPROM
uint8_t readEEPROM(uint8_t addr);
// Write a byte to EEPROM
void writeEEPROM(uint8_t addr, uint8_t value);

// Read data from the protected EEPROM
void readProtectedEEPROM(uint8_t startAddr, uint8_t* buffer, uint8_t readLen);

// Read just the hour
uint8_t readHour();
// Read just the minute
uint8_t readMinute();
// Read just the second
uint8_t readSecond();
// Write just the hour
void writeHour(uint8_t hour);
// Write just the minute
void writeMinute(uint8_t minute);
// Write just the second
void writeSecond(uint8_t second);

// Read the entire time string and place the value in the given addresses
void readTime(uint8_t *hour24, uint8_t *minute, uint8_t *second);
// Write the entire time string
void writeTime(uint8_t hour24, uint8_t minute, uint8_t second);

// Set the crystal trimming to this value
// If positiveTrim is true, *add* clock cycles (slow crystal), otherwise remove (fast crystal)
// If coarse is true, add/remove clock cycles every second, otherwise every minute
// Each additional 1 trim equates to 2 clock cycles per period
void setTrim(bool positiveTrim, bool coarse, uint8_t trimVal);

// Enable or disable the multi-function pin to output a square wave at the selected frequency
// See [MFP output selections] for the acceptable values
// Users can select from 1Hz, 4.096KHz, 8.192KHz, or the raw 32.768KHz output
// Note that the MFP will not enable if coarse trimming is enabled.
// Also note that if non-coarse trimming is applied, the 1Hz, 4.096KHz,
//  and 8.192KHz outputs will be scaled appropriately.
void setMFPOutput(bool enableMFP, uint8_t mfpFunc);

// Output if oscillator is running or if it is stopped
bool oscRunning();

// Read the STATUS register
uint8_t readStatus();
// Write to the STATUS register; only BLOCKPROTECT1 and BLOCKPROTECT0 can be written
void writeStatus(uint8_t value);

#endif //__MCP79510_H__
