#include "Arduino.h"
#include "Wire.h"
#include "encoder.h"

Encoder VertX;

void setup() {
  Serial.begin(115200);
  }

void loop() {
    float value(0);
    STATUS_t Status = INIT;
    CHIP_t encoderSelect = ENCODER2;
    value = VertX.readEncoder(Status,encoderSelect);
    
    if(Status==VALID_ANGLE)
    {
    Serial.print("\nAngle (deg) = ");
    Serial.println(value, 4);
    }
    else
    {
      Serial.println("Error !");
    }
    delay(10);
  }
