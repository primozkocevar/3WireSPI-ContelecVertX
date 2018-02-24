// Test derrived class for base class SparkFunIMU
#ifndef __ENCODER_H__
#define __ENCODER_H__
#include "Arduino.h"
#include "Wire.h"

// Define SPI pins
//  D53 -> SDI/SDO
//  D52 -> SCLK
//  D51 -> CSPORT_ENCODER1
//  D50 -> CSPORT_ENCODER2
//  D49 -> CSPORT_ENCODER3
//  D48 -> CSPORT_ENCODER4
#define CSPORT_ENCODER2 PORTB
#define CSBIT_ENCODER2  3
#define CSPORT_ENCODER1 PORTB
#define CSBIT_ENCODER1  2
#define CSPORT_ENCODER3 PORTL
#define CSBIT_ENCODER3  0
#define CSPORT_ENCODER4 PORTL
#define CSBIT_ENCODER4  1
#define CLKPORT    PORTB
#define CLKBIT     1
#define DATAPORTI  PINB
#define DATAPORTO  PORTB
#define DATABIT    0
//The Port B Data Direction Register - read/write :
#define DIR_REG    DDRB
#define DIR_REG_L    DDRL
// End SPI pin definitions

typedef enum
{
  ENCODER1,
  ENCODER2,
  ENCODER3,
  ENCODER4
} CHIP_t;

typedef enum
{
  INIT,
  VALID_ANGLE,
  ERROR_ENCODER,
  INVALID_PATTERN
} STATUS_t;

class Encoder
{
  public:

    Encoder();
    float readEncoder(STATUS_t&,CHIP_t&);

  protected:

    // Hardware abstraction function
    uint16_t SPI_Read(CHIP_t);
   
};

#endif
