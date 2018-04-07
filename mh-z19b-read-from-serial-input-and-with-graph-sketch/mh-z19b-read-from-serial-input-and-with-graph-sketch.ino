/*
Copyright © 2018, github.com/satr
License: MIT

Arduino with CO2 sensor MH-Z19B (v2: 0-5000 ppm) and the display Nokia 5110
Data are read from the sensor's serial port.
Graph displays a change of the CO2 level on time.

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

const unsigned int MIN_VALUE = 300;
const unsigned int VERY_GOOD_VALUE = 450;
const unsigned int GOOD_VALUE = 600;
const unsigned int ACCEPTABLE_VALUE = 1000;
const unsigned int MAX_VALUE = 2500;

unsigned int goodValueGraphValue = 0;
unsigned int acceptableValueGraphValue = 0;
const byte GRAPH_WIDTH = 84;
const byte GRAPH_HEIGHT = 28;
const unsigned int MAX_GRAPH_VALUE = 1500;
const unsigned int GRAPH_STEP = (MAX_GRAPH_VALUE - MIN_VALUE) / GRAPH_HEIGHT;
unsigned int graphData[GRAPH_WIDTH];
#define BACKCOLOR 0x0000 // Black
#define PIXELCOLOR 0xFFFF  // White
int displayHeight = 48;
byte graphSpeed = 1;
int graphDrawingCounter = 0;

void setup() {
  Serial.begin(9600);
  co2SensorSerial.begin(9600);
  
  initDisplay();
  putCo2ValueOnDisplay("---");
  putMessageOnDisplay("Initializing");
  invalidateDisplay();
  initGraphStructures();
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
  
  drawGraph();
  invalidateDisplay();
  
  delay(2000);//timeout between reads - 2 seconds
}

void initGraphStructures() {
  for(byte i = 0; i < GRAPH_WIDTH; i++) {
    graphData[i] = getGraphValue(MIN_VALUE);
  }
  goodValueGraphValue = getGraphValue(GOOD_VALUE);
  acceptableValueGraphValue = getGraphValue(ACCEPTABLE_VALUE);
}

int getGraphValue(unsigned int value) {
    if(value < MIN_VALUE) {
      return 0;
    }
    if(value > MAX_GRAPH_VALUE) {
      value = MAX_GRAPH_VALUE;
    }
    return (value - MIN_VALUE) / GRAPH_STEP;
}


void drawGraph() {
  bool dashState = false;
  for(int i = 0; i < GRAPH_WIDTH; i++) {
    byte x = GRAPH_WIDTH - 1 - i;
    display.drawLine(x, displayHeight - 1, x, displayHeight - 1 - graphData[i], PIXELCOLOR);
    display.drawPixel(x, displayHeight - 1 - goodValueGraphValue, dashState ? PIXELCOLOR : BACKCOLOR);
    display.drawPixel(x, displayHeight - 1 - acceptableValueGraphValue, dashState ? PIXELCOLOR : BACKCOLOR);
    dashState = !dashState;
  }
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
  
void readCo2SensorValueToCo2Response() {
  co2SensorSerial.write(cmd, 9);
  memset(co2SensorResponse, 0, 9);
  co2SensorSerial.readBytes(co2SensorResponse, 9);
}

void addValueToGraph(unsigned int value) {
  for(int i = GRAPH_WIDTH - 1; i > 0; i--) {
    graphData[i] = graphData[i - 1];//shift values
  }
  graphData[0] = getGraphValue(value);
}
  
void putOnDisplaySensorDataAndMessage(unsigned int ppm) {
  Serial.println(ppm);
  addValueToGraph(ppm);
  
  if (ppm <= MIN_VALUE || ppm > 4900) {
    putCo2ValueOnDisplay("*****");
    putMessageOnDisplay("No valid CO2 data");
    return;
  }

  putCo2ValueOnDisplay(String(ppm)); 
  
  if (ppm < VERY_GOOD_VALUE) {   
    putMessageOnDisplay("Very good");
    return;
  }
  if (ppm < GOOD_VALUE) {   
    putMessageOnDisplay("Good");
    return;
  }
  if (ppm < ACCEPTABLE_VALUE) {   
    putMessageOnDisplay("Acceptable");
    return;
  }
  if (ppm < MAX_VALUE) {   
    putMessageOnDisplay("Bad");
    return;
  }
  putMessageOnDisplay("Health risk");
}

void putCo2ValueOnDisplay(String ppmValueText) {
  putOnDisplay("CO2: " + String(ppmValueText) + " ppm", 1, 0, 0);
}

void putMessageOnDisplay(String text) {
  putOnDisplay(text, 1, 0, 10);
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
  displayHeight = display.height();
  display.setContrast(50); // change the contrast for the best viewing!
  delay(1000);
  clearDisplay();
}
