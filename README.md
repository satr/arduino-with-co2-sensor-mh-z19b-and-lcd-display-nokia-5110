# The Arduino with CO2 sensor MH-Z19B (v2) and LCD display from Nokia 5110.
There are two ways to read data from the sensor:
* from PWM output 
* from serial port

There is a sketches for each of these ways. The one, which uses the serial port - works more stable.

Connect the sensor, when data are read from the sensor's PWM output:
```
Arduino | CO2 sensor MH-Z19B
  pin 10 - PWM
  GND   - GND
  +5V   - Vin
```
Connect the sensor, when data are read from the sensor's serial port.
```
Arduino | CO2 sensor MH-Z19B
  pin 8 - RX
  pin 9 - TX
  GND   - GND
  +5V   - Vin
```
Connect the display
```
Arduino | Display Nokia 5110
  pin 7 - Serial clock out: CLK (SCLK)
  pin 6 - Serial data out: DIN
  pin 5 - Data/Command select: DC (D/C)
  pin 4 - LCD chip select: CE (CS)
  pin 3 - LCD reset: RST
  5V    - VCC
  GND   - GND
```
![](docs/images/arduino-with-co2-sensor-mh-z19b.jpg)
