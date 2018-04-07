/*
Copyright © 2018, github.com/satr
License: MIT

Arduino with CO2 sensor MH-Z19B (v2: 0-5000 ppm) and the display Nokia 5110
Data are read from the sensor's serial port.

Arduino | CO2 sensor MH-Z19B
  pin 8 - RX
  pin 9 - TX
  GND   - GND
  +5V   - Vin

Arduino | Display Nokia 5110
  pin 7 - Serial clock out: CLK (SCLK)
  pin 6 - Serial data out: DIN
  pin 5 - Data/Command select: DC (D/C)
  pin 4 - LCD chip select: CE (CS)
  pin 3 - LCD reset: RST
  5V    - VCC
  GND   - GND
*/

#include <SoftwareSerial.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
Adafruit_PCD8544 display = Adafruit_PCD8544(7, 6, 5, 4, 3); //pins description are above

SoftwareSerial co2SensorSerial(8, 9); // pins description are above

// CO2 sensor data structures:
byte cmd[9] = {0xFF,0x01,0x86,0x00,0x00,0x00,0x00,0x00,0x79}; 
unsigned char co2SensorResponse[9];
byte crc = 0;


void setup() {
  Serial.begin(9600);
  co2SensorSerial.begin(9600);
  
  initDisplay();
  putCo2ValueOnDisplay("---");
  putMessageOnDisplay("Initializing");
  invalidateDisplay();
}


void loop() 
{
  readCo2SensorValueToCo2Response();
  
  clearDisplay();
  if(validateCo2Response()) {
    putOnDisplaySensorDataAndMessage((256 * (unsigned int) co2SensorResponse[2]) + (unsigned int) co2SensorResponse[3]);
  } else {
    putMessageOnDisplay("CO2 sensor error");
  }
  invalidateDisplay();
  
  delay(1000);//timeout between reads - 1 second
}


bool validateCo2Response() {
  crc = 0;
  for (int i = 1; i < 8; i++) {
    crc += co2SensorResponse[i];
  }
  crc = 256 - crc;
  //crc++;
  bool valid = co2SensorResponse[0] == 0xFF && co2SensorResponse[1] == 0x86 && co2SensorResponse[8] == crc;
  if(!valid) {
    Serial.println("CRC error: " + String(crc) + "/"+ String(co2SensorResponse[8]));
  }
  return valid; 
}
  
void   readCo2SensorValueToCo2Response() {
  co2SensorSerial.write(cmd, 9);
  memset(co2SensorResponse, 0, 9);
  co2SensorSerial.readBytes(co2SensorResponse, 9);
}

void putOnDisplaySensorDataAndMessage(unsigned int ppm) {
  Serial.println(ppm);
  if (ppm <= 300 || ppm > 4900) {
    putCo2ValueOnDisplay("*****");
    putMessageOnDisplay("No valid CO2 data");
    return;
  }
 
  putCo2ValueOnDisplay(String(ppm)); 
  
  if (ppm < 450) {   
    putMessageOnDisplay("Very good");
    return;
  }
  if (ppm < 600) {   
    putMessageOnDisplay("Good");
    return;
  }
  if (ppm < 1000) {   
    putMessageOnDisplay("Acceptable");
    return;
  }
  if (ppm < 2500) {   
    putMessageOnDisplay("Bad");
    return;
  }
  putMessageOnDisplay("Health risk");
}

void putCo2ValueOnDisplay(String ppmValueText) {
  putOnDisplay("CO2: " + String(ppmValueText) + " ppm", 1, 0, 0);
}

void putMessageOnDisplay(String text) {
  putOnDisplay(text, 1, 0, 20);
}

void putOnDisplay(String text, int textSize, int posX, int posY) {
  display.setTextSize(textSize);
  display.setTextColor(BLACK);
  display.setCursor(posX, posY);
  display.print(text);
}

void invalidateDisplay() {
  display.display();
}

void clearDisplay() {
  display.clearDisplay();   // clears the screen and buffer
}

void initDisplay() {
  display.begin();  // init done
  display.setContrast(50); // change the contrast for the best viewing!
  delay(1000);
  clearDisplay();
}
