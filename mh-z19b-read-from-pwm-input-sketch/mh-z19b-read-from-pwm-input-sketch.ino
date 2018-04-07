#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
/*
pin 7 - Serial clock out CLK (SCLK)
pin 6 - Serial data out DIN
pin 5 - Data/Command select DC (D/C)
pin 4 - LCD chip select CE (CS)
pin 3 - LCD reset RST
*/
Adafruit_PCD8544 display = Adafruit_PCD8544(7, 6, 5, 4, 3);
#define pwmPin 10
#define LedPin 13

int prevValue = LOW;
long th, tl, h, l, ppm;

void setup() {
  Serial.begin(9600);
  pinMode(pwmPin, INPUT);
  pinMode(LedPin, OUTPUT);
  initDisplay();
}

void loop() {
  long timeMillisec = millis();
  int pwmValue = digitalRead(pwmPin);

  //Если обнаружили изменение
  if (pwmValue == HIGH) {
    digitalWrite(LedPin, HIGH);
    if (pwmValue != prevValue) {
      h = timeMillisec;
      tl = h - l;
      prevValue = pwmValue;
    }
  } else {
    digitalWrite(LedPin, LOW);
    if (pwmValue != prevValue) {
      l = timeMillisec;
      th = l - h;
      prevValue = pwmValue;
      ppm = 5000 * (th - 2) / (th + tl - 4);
      Serial.println("PPM = " + String(ppm));
    }
  }
  display.clearDisplay();   // clears the screen and buffer
  displayOutput("PPM = " + String(ppm), 1, 0, 0);
}

void displayOutput(String text, int textSize, int posX, int posY){
  display.setTextSize(textSize);
  display.setTextColor(BLACK);
  display.setCursor(posX, posY);
  display.println(text);
  display.display();
}

void clearDisplay(){
  display.clearDisplay();   // clears the screen and buffer
}

void initDisplay() {
  display.begin();
  // init done

  // you can change the contrast around to adapt the display
  // for the best viewing!
  display.setContrast(50);
  delay(1000);
  display.clearDisplay();   // clears the screen and buffer
}
 
