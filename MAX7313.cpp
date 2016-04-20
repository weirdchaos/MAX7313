#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif
#ifdef __AVR_ATtiny85__
  #include <TinyWireM.h>
  #define Wire TinyWireM
#else
  #include <Wire.h>
#endif

#ifdef __AVR
  #include <avr/pgmspace.h>
#elif defined(ESP8266)
  #include <pgmspace.h>
#endif

//#define DEBUG 1

#include "MAX7313.h"

void WCN_MAX7313::begin(uint8_t addr) {
  i2caddr = addr;

  Wire.begin();

  // Set defaults!
  write8(MAX7313_CONFIG, B00010000);
//  write8(MAX7313_PORTCONF70, 0x00);  // Configure all ports as output
//  write8(MAX7313_PORTCONF158, 0x00); //
  write8(MAX7313_BLINK0P70, 0xFF);   // Turn intensity to 0 (0xF)
  write8(MAX7313_BLINK0P158, 0xFF);  //
  write8(MAX7313_BLINK1P70, 0x00);
  write8(MAX7313_BLINK1P158, 0x00);
  write8(MAX7313_MASTO16INT, 0xFF);
  for (int i = 0x10; i < 0x17; i++){
    write8(i, 0xFF);
  }

}

void WCN_MAX7313::begin(void) {
  begin(MAX7313_ADDRESS);
}

void WCN_MAX7313::pinMode(uint8_t p, uint8_t d) {
  uint8_t iodir, reg;

  if (p < 8) {
    reg = MAX7313_PORTCONF70;
  }
  else {
    reg = MAX7313_PORTCONF158;
    p -= 8;
  }
  iodir = read8(reg);

  // set the pin and direction
  if (d == INPUT) {
    iodir |= 1 << p;
  } else {
    iodir &= ~(1 << p);
  }

  // write the new IODIR
  write8(reg, iodir);
}

void WCN_MAX7313::analogWrite(uint8_t port, uint8_t d) {
  uint8_t intens, blinks, reg, e;

  reg = getRegAddr(port, MAX7313_BLINKPHASE0);

  if (d > 0xFF) d = 0xFF;
  e = map(d, 0x0, 0xFF, 0XF, 0x0);

  blinks = read8(reg); // Set phase 0 bit to 1
  blinks |= 1 << port;
  write8(reg, blinks);

  // read the current GPIO output latches
  intens = readIntensity(port);
//  blinks =

#ifdef DEBUG
  SerialUSB.println(intens, HEX);
#endif

  if ((port % 2) > 0) {

    intens &= 0x0F;
    intens |= e << 4;
  } else {
    intens &= 0xF0;
    intens |= e;
  }

  // write the new GPIO
  writeIntensity(port, intens);
}

void WCN_MAX7313::allOutputOff(void) {
  uint8_t blinks, reg;
  write8(MAX7313_BLINK0P70, 0xFF);
  write8(MAX7313_BLINK0P158, 0xFF);
  for (int i = 0x10; i < 0x17; i++){
    write8(i, 0xFF);
  }
}

void WCN_MAX7313::blink(bool on = true) {
  uint8_t conf;

  conf = read8(MAX7313_CONFIG);

  if (on) {
    conf |= 1;
  } else {
    conf &= 0;
  }

  write8(MAX7313_CONFIG, conf);
}

bool WCN_MAX7313::getPortConf(uint8_t port) {
  uint8_t reg, confreg, conf;

  if (port < 8) {
    reg = MAX7313_PORTCONF70;
  }
   else {
    reg = MAX7313_PORTCONF158;
    port = port - 8;
  }

  confreg = read8(reg);

  if ((confreg & (1 << port)) == (1 << port)) return 1; else return 0;
}

// Helper functions

uint8_t WCN_MAX7313::getRegAddr(uint8_t port, uint8_t phase) {
  uint8_t reg;

  switch (phase) {
    case 0:
      switch (port) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
            reg = MAX7313_BLINK0P70; break;
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
            reg = MAX7313_BLINK0P158; break;
      }
      break;
    case 1:
        switch (port) {
          case 0:
          case 1:
          case 2:
          case 3:
          case 4:
          case 5:
          case 6:
          case 7:
              reg = MAX7313_BLINK1P70; break;
          case 8:
          case 9:
          case 10:
          case 11:
          case 12:
          case 13:
          case 14:
          case 15:
              reg = MAX7313_BLINK1P158; break;
        }
        break;
    case 2:
      switch (port) {
        case 0:
        case 1:
            reg = MAX7313_OUTPUTINT10; break;
        case 2:
        case 3:
            reg = MAX7313_OUTPUTINT32; break;
        case 4:
        case 5:
            reg = MAX7313_OUTPUTINT54; break;
        case 6:
        case 7:
            reg = MAX7313_OUTPUTINT76; break;
        case 8:
        case 9:
            reg = MAX7313_OUTPUTINT98; break;
        case 10:
        case 11:
            reg = MAX7313_OUTPUTINT1110; break;
        case 12:
        case 13:
            reg = MAX7313_OUTPUTINT1312; break;
        case 14:
        case 15:
            reg = MAX7313_OUTPUTINT1514; break;
      }
      break;
  }

  return reg;
}

uint8_t WCN_MAX7313::readIntensity(uint8_t port) {
  // read the current intensity values
  uint8_t intens, reg;

  switch (port) {
    case 0:
    case 1:
        reg = MAX7313_OUTPUTINT10; break;
    case 2:
    case 3:
        reg = MAX7313_OUTPUTINT32; break;
    case 4:
    case 5:
        reg = MAX7313_OUTPUTINT54; break;
    case 6:
    case 7:
        reg = MAX7313_OUTPUTINT76; break;
    case 8:
    case 9:
        reg = MAX7313_OUTPUTINT98; break;
    case 10:
    case 11:
        reg = MAX7313_OUTPUTINT1110; break;
    case 12:
    case 13:
        reg = MAX7313_OUTPUTINT1312; break;
    case 14:
    case 15:
        reg = MAX7313_OUTPUTINT1514; break;
  }

  return read8(reg);
}

void WCN_MAX7313::writeIntensity(uint8_t port, uint8_t intens) {
  uint8_t reg;

  switch (port) {
    case 0:
    case 1:
        reg = MAX7313_OUTPUTINT10; break;
    case 2:
    case 3:
        reg = MAX7313_OUTPUTINT32; break;
    case 4:
    case 5:
        reg = MAX7313_OUTPUTINT54; break;
    case 6:
    case 7:
        reg = MAX7313_OUTPUTINT76; break;
    case 8:
    case 9:
        reg = MAX7313_OUTPUTINT98; break;
    case 10:
    case 11:
        reg = MAX7313_OUTPUTINT1110; break;
    case 12:
    case 13:
        reg = MAX7313_OUTPUTINT1312; break;
    case 14:
    case 15:
        reg = MAX7313_OUTPUTINT1514; break;
  }

  write8(reg, intens);
}

uint8_t WCN_MAX7313::read8(uint8_t addr) {
  Wire.beginTransmission(i2caddr);
#if ARDUINO >= 100
  Wire.write((byte)addr);
#else
  Wire.send(addr);
#endif
  Wire.endTransmission();
  Wire.requestFrom(i2caddr, 1);

#if ARDUINO >= 100
  return Wire.read();
#else
  return Wire.receive();
#endif
}

void WCN_MAX7313::write8(uint8_t addr, uint8_t data) {
  Wire.beginTransmission(i2caddr);
#if ARDUINO >= 100
  Wire.write((byte)addr);
  Wire.write((byte)data);
#else
  Wire.send(addr);
  Wire.send(data);
#endif
  Wire.endTransmission();
}
