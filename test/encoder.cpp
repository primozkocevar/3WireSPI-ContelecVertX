#include "Arduino.h"
#include "encoder.h"
#include "stdint.h"


Encoder::Encoder()
{
    // Setup pins for SPI
    // CS & CLK must be outputs DDRxn = 1
    bitSet(DIR_REG, CSBIT_ENCODER1);
    bitSet(DIR_REG, CSBIT_ENCODER2);
    bitSet(DIR_REG_L, CSBIT_ENCODER3);
    bitSet(DIR_REG_L, CSBIT_ENCODER4);
    bitSet(DIR_REG, CLKBIT);
    // Deselect SPI chips
    bitSet(CSPORT_ENCODER1, CSBIT_ENCODER1);
    bitSet(CSPORT_ENCODER2, CSBIT_ENCODER2);  
    bitSet(CSPORT_ENCODER3, CSBIT_ENCODER3);
    bitSet(CSPORT_ENCODER4, CSBIT_ENCODER4); 
    //bitSet(CSPORT_ENCODERn, CSBIT_ENCODERn); and so on...
    // Clock polarity (CPOL) = 1
    bitClear(CLKPORT, CLKBIT);
}


float Encoder::readEncoder(STATUS_t& Status,CHIP_t& encoderSelect)
{  
  uint16_t angle;
  float anglef;

  // add defferent 
  angle = SPI_Read(encoderSelect);

  //Check for errors:
  switch (angle&0x0003)
  {
  case 0x01: // Valid angle
    angle = angle&0xFFFC;
    angle >>= 2;
    anglef=float(angle);
    anglef=360*anglef/16384;
    Status=VALID_ANGLE;
    // a simple solution for a problem that it sometimes throws values between 358-360 randomly
    //if((int)anglef!=358 && (int)anglef!=359 && (int)anglef!=360){
    return anglef;
    //}
    break;
  case 0x02: // Error code
    Status=ERROR_ENCODER;
    return 0;
    break;
  case 0x03: // Invalid pattern
    Status=INVALID_PATTERN;
    return 0;
    break;
  case 0x00: // Invalid pattern
    Status=INVALID_PATTERN;
    return 0;
    break;
  }
}

// This function uses bit manibulation for higher speed & smaller code
uint16_t Encoder::SPI_Read(CHIP_t chip)
{
  uint8_t counter;
  uint8_t dataL;
  uint8_t dataH;
  uint16_t data;
  uint8_t AA=0xaa;
  uint8_t FF=0xff;
  bool flag=1;
  bool flag2=0;

  
 
  noInterrupts();
 // Set data pin to output
  bitSet(DIR_REG, DATABIT);
  
  // Select the chip & deselect the other
  switch (chip)
  {
  case ENCODER1:
  // we clear the one we want to enable and set all others, just in case to unable them if they are enabled.
    bitClear(CSPORT_ENCODER1, CSBIT_ENCODER1);
    bitSet(CSPORT_ENCODER2, CSBIT_ENCODER2);
    bitSet(CSPORT_ENCODER3, CSBIT_ENCODER3);
    bitSet(CSPORT_ENCODER4, CSBIT_ENCODER4);
    break;
  case ENCODER2:
    bitClear(CSPORT_ENCODER2, CSBIT_ENCODER2);
    bitSet(CSPORT_ENCODER1, CSBIT_ENCODER1);
    bitSet(CSPORT_ENCODER3, CSBIT_ENCODER3);
    bitSet(CSPORT_ENCODER4, CSBIT_ENCODER4);
    break;
  case ENCODER3:
    bitClear(CSPORT_ENCODER3, CSBIT_ENCODER3);
    bitSet(CSPORT_ENCODER1, CSBIT_ENCODER1);
    bitSet(CSPORT_ENCODER2, CSBIT_ENCODER2);
    bitSet(CSPORT_ENCODER4, CSBIT_ENCODER4);
    break;
  case ENCODER4:
    bitClear(CSPORT_ENCODER4, CSBIT_ENCODER4);
    bitSet(CSPORT_ENCODER1, CSBIT_ENCODER1);
    bitSet(CSPORT_ENCODER2, CSBIT_ENCODER2);
    bitSet(CSPORT_ENCODER3, CSBIT_ENCODER3);
    break;
  }
  delayMicroseconds(10);

  for(counter = 8; counter; counter--)
  {
    delayMicroseconds(5);
    flag2=!flag2;
    bitWrite(DATAPORTO, DATABIT, flag2);
    delayMicroseconds(5);
    // Data is setup, so drop clock edge
    bitSet(CLKPORT, CLKBIT);
    delayMicroseconds(10);
    bitClear(CLKPORT, CLKBIT);
  }
  delayMicroseconds(50);
  for(counter = 8; counter; counter--)
  {
    delayMicroseconds(5);
    bitWrite(DATAPORTO, DATABIT, flag);
    delayMicroseconds(5);
    // Data is setup, so drop clock edge
    bitSet(CLKPORT, CLKBIT);
    delayMicroseconds(10);
    bitClear(CLKPORT, CLKBIT);
  }
  
  // Switch data pin to input (0 = INPUT)
  delayMicroseconds(50);
  bitClear(DIR_REG, DATABIT);
  
  // Shift in register data from address
  for(counter = 8; counter; counter--)
  {
    // Shift dataH to the left.  Remains 0 after first shift
    dataH <<= 1;
    delayMicroseconds(10);
    bitSet(CLKPORT, CLKBIT);
    // Sample on rising egde
    delayMicroseconds(10);
    bitClear(CLKPORT, CLKBIT);
    if (bitRead(DATAPORTI, DATABIT))
    {
      dataH |= 0x01;
    }
  }
delayMicroseconds(50);
  for(counter = 8; counter; counter--)
  {
    // Shift dataL to the left.  Remains 0 after first shift
    dataL <<= 1;
    delayMicroseconds(10);
    bitSet(CLKPORT, CLKBIT);
    // Sample on rising egde
    delayMicroseconds(10);
    bitClear(CLKPORT, CLKBIT);
    if (bitRead(DATAPORTI, DATABIT))
    {
      dataL |= 0x01;
    }
  }

    
  // Unselect chip
  switch (chip)
  {
  case ENCODER1:
    // unselect it by setting it 
    bitSet(CSPORT_ENCODER1, CSBIT_ENCODER1);
    break;
  case ENCODER2:
    bitSet(CSPORT_ENCODER2, CSBIT_ENCODER2);
    break;
  case ENCODER3:
    bitSet(CSPORT_ENCODER3, CSBIT_ENCODER3);
    break;
  case ENCODER4:
    bitSet(CSPORT_ENCODER4, CSBIT_ENCODER4);
    break;
  }
delayMicroseconds(300);
  data=(int16_t)( (dataH << 8) | dataL );
  interrupts();

  return(data);
}
