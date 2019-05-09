/*
   This is example code on how to use Maker LED Matrix Dispaly library on e-radionica Maker LED Matrix Display Board with ESP8266 and two or four IL31LF3731 drivers.
   It shows how read data from I2C sensor (in this case is BMP180) and display that data on display.

   IMPORTANT!
   We are here using different I2C pins than ones connected to easyC connector. I2C pins that are connected to easyC are same pins for LED drivers.
   You can use them only if you have to, otherwise use different pins.
*/

#include "Maker_LED_Matrix.h"   //Include our Maker LED Matrix Library
#include "Adafruit_BMP085.h"    //Then, include Adafruit library for BMP180 Sensor (you can find it here: https://github.com/adafruit/Adafruit-BMP085-Library)

//Then we make an object to a class. We call this object matrix, but you can name it whatever you want. But remember to use that name instead od matrix in rest of the code!
//If you're using Maker LED Matrix 2 (with resolution of 32x9) than use this
//Maker_LED_Matrix matrix(MAKER_LED_MATRIX_2);

//...otherwise use this
Maker_LED_Matrix matrix(MAKER_LED_MATRIX_4);

Adafruit_BMP085 bmp;

double temp;      //Variable for storing temperature
double pressure;  //Variable for storing absolute pressure data
char text[150];   //Char array for storing text (message) that we want to display (max 149 characters long)

void setup() {
  matrix.begin(&matrix);    //Init our library. As argument we are sending address of out library object. It's necessary to do that, otherwise, we are unable to refresh LED Matrix.
  
  //This is very important. On this board I2C is shared between LED drivers and our sensor (or any other I2C device that you connect).
  //If we want to comunicate to our connected device, we have to switch I2C to pins where our sensor is connected (in our case SDA is connected to pin 13 and SCL to pin 12).
  Wire.begin(13,12);
  if (!bmp.begin()) {   //Check if ESP8266 can find BMP180, if not, write a message and stop the program.
    matrix.message("Could not find BMP180 sensor, check wiring!", 100, 1, -1);
  while (1) {
    delay(10);
    }
  }
}

void loop() {
  //Also, very important! Before each reading data from any I2C device, we have to stop scrolling message, in order to ESP8266 stops sending data to LED drivers. Now, when I2C bus is free, we can read data.
  matrix.stopScroll();
  Wire.begin(13,12);
  temp = bmp.readTemperature();         //Read temperature from BMP180 in Celsius.
  pressure = bmp.readPressure()/100.0;  //Read absolute air pressure in Pa (we are dividing by 100 to get pressure in hPa)
  matrix.resumeScroll();                //After reading data from sensor, continue with scroll.

  //Put whole message in one string using sprintf function.
  sprintf(text, "Temperature %d.%02dC Pressure %d.%02d hPa", int(temp), abs(int(temp*100)%100), int(pressure), abs(int(pressure*100)%100));

  //Send new message to display.
  matrix.message(text, 100, 2, -1);

  //Wait to display whole message to read new data.
  while(matrix.repeatCount() != 1) {
    delay(1);
  }
}
