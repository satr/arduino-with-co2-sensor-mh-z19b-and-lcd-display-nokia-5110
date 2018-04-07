/*
Copyright © 2018, github.com/satr
License: MIT

Arduino with CO2 sensor MH-Z19B (v2: 0-5000 ppm) and the display Nokia 5110
Data are read from the sensor's PWM output.

Arduino | CO2 sensor MH-Z19B
  pin 10 - PWM
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

#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
Adafruit_PCD8544 display = Adafruit_PCD8544(7, 6, 5, 4, 3); //pins description are above
#define pwmPin 10

int prevValue = LOW;
long th, tl, h, l, ppm = 0;

void setup() {
  Serial.begin(9600);
  pinMode(pwmPin, INPUT);
  initDisplay();
  putCo2ValueOnDisplay("---");
  putMessageOnDisplay("Initializing");
  invalidateDisplay();
}


void loop() 
{
  clearDisplay();
  readCo2SensorValueToCo2Response();
  putOnDisplaySensorDataAndMessage(ppm);
  invalidateDisplay();
  
  delay(1000);//timeout between reads - 1 second
}


void readCo2SensorValueToCo2Response() {
  long timeMillisec = millis();
  int pwmValue = digitalRead(pwmPin);

  if (pwmValue == HIGH) {
    if (pwmValue != prevValue) {
      h = timeMillisec;
      tl = h - l;
      prevValue = pwmValue;
    }
    return;
  }
  if (pwmValue != prevValue) {
    l = timeMillisec;
    th = l - h;
    prevValue = pwmValue;
    ppm = 5000 * (th - 2) / (th + tl - 4);
  }
}

void putOnDisplaySensorDataAndMessage(unsigned int ppm) {
  if (ppm == 0) {   
    putMessageOnDisplay("Initializing");
    return;
  }  
  
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
 
