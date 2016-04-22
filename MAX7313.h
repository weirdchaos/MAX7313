#ifndef MAX7313_H
#define MAX7313_H

#include <Wire.h>

class WCN_MAX7313 {
public:
  void begin(uint8_t addr);
  void begin(void);

  void allOutputOff(void);
  void pinMode(uint8_t p, uint8_t d);
  void analogWrite(uint8_t p, uint8_t d);
  void digitalRead(uint8_t p);
  void blink(bool o);
  void blink(uint8_t p, bool o);
  void blink(uint8_t p, uint8_t d0, uint8_t d1);
  void toggle(uint8_t p);
  bool getPortConf(uint8_t p);
  void globalIntensity(uint8_t i);
  void globalIntensity(bool o);

private:
  uint8_t i2caddr;
  uint8_t read8(uint8_t addr);
  void write8(uint8_t addr, uint8_t data);
  uint8_t getRegAddr(uint8_t p, uint8_t b);
  uint8_t readIntensity(uint8_t p);
  void writeIntensity(uint8_t p, uint8_t i);
};

#define MAX7313_ADDRESS 0x10

// Registers
#define MAX7313_READINP70 0x00
#define MAX7313_READINP158 0x01
#define MAX7313_BLINK0P70 0x02
#define MAX7313_BLINK0P158 0x03
#define MAX7313_PORTCONF70 0x06
#define MAX7313_PORTCONF158 0x07
#define MAX7313_BLINK1P70 0x0A
#define MAX7313_BLINK1P158 0x0B
#define MAX7313_MASTO16INT 0x0E
#define MAX7313_CONFIG 0x0F
#define MAX7313_OUTPUTINT10 0x10
#define MAX7313_OUTPUTINT32 0x11
#define MAX7313_OUTPUTINT54 0x12
#define MAX7313_OUTPUTINT76 0x13
#define MAX7313_OUTPUTINT98 0x14
#define MAX7313_OUTPUTINT1110 0x15
#define MAX7313_OUTPUTINT1312 0x16
#define MAX7313_OUTPUTINT1514 0x17
#define MAX7313_BLINKPHASE0 0
#define MAX7313_BLINKPHASE1 1
#define MAX7313_INTENSITY 2

#endif
