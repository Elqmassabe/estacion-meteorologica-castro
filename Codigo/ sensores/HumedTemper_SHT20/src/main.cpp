#include <Arduino.h>
#include "DFRobot_SHT20.h"

DFRobot_SHT20 sht20(&Wire, SHT20_I2C_ADDR);

void setup()
{
  Serial.begin(115200);
  sht20.initSHT20();
  delay(100);
  Serial.println("Sensor init finish!");
  sht20.checkSHT20();
}

void loop()
{
  float humd = sht20.readHumidity();

  Serial.print(" Humidity:");
  Serial.print(humd, 1);  
  Serial.print("%");
  Serial.println();

  delay(1000);
}