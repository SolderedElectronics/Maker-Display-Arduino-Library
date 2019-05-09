/*
   This is example code on how to use Maker LED Matrix Dispaly library on e-radionica Maker LED Display Board with ESP8266 and two or four IL31LF3731 drivers.
   It shows how read data from I2C sensor that is connected to easyC connector (in this case APDS9960) and display detected gesture.

   IMPORTANT!
   Here we are using I2C pins same ones as on easyC connector and these I2C pins are aslo connected to LED drivers.
   If You have any problems with sensor, try to put 47uF bypass capacitor between 3.3VDC and GND.

   Also if you still having trouble with reading data from sensor or even sensor init, use unused pins (GPIO 12, 13, 14, 16) of ESP8266 MCU for I2C communication (see other sensor example ih example folder of this library!).
*/

#include "Maker_LED_Matrix.h"   //Include our Maker LED Matrix Library
#include <SparkFun_APDS9960.h>  //Include APDS9960 library. You can find it here: https://github.com/sparkfun/SparkFun_APDS-9960_Sensor_Arduino_Library).

//Then we make an object to a class. We call this object matrix, but you can name it whatever you want. But remember to use that name instead od matrix in rest of the code!
//If you're using Maker LED Matrix 2 (with resolution of 32x9) than use this
//Maker_LED_Matrix matrix(MAKER_LED_MATRIX_2);

//...otherwise use this
Maker_LED_Matrix matrix(MAKER_LED_MATRIX_4);

SparkFun_APDS9960 apds;

void setup() {
  matrix.begin(&matrix);    //Init our library. As argument we are sending address of out library object. It's necessary to do that, otherwise, we are unable to refresh LED Matrix.
  
  //This is very important. On this board I2C is shared between LED drivers and our sensor (or any other I2C device that you connect).
  if (!apds.init()) {   //Check if ESP8266 can find BMP180, if not, write a message and stop the program.
    matrix.message("Could not find APDS9960 sensor, check wiring!", 100, 1, -1);
  while (1) {
    delay(10);
    }
  }

  //Init sensor gesture engine, without interrupt.
  if ( apds.enableGestureSensor(false) ) {
    matrix.message("Make a gesture! :)", 100, 2, -1);
  } else {
    matrix.message("Sensor gesture init fail. :(", 100, 2, -1);    //If it fails, print a message to screen and stop program.
    do {
      delay(1);
    } while (true);
  }
}

void loop() {
  //Also, very important! Before each reading data from any I2C device, we have to stop scrolling message, in order to ESP8266 stops sending data to LED drivers.
  //Now, when I2C bus is free, we can communicate with sensor.
  matrix.stopScroll();
  if ( apds.isGestureAvailable() ) {  //Read if there is any gesture detected.
    switch ( apds.readGesture() ) {   //If there is some vaild gesture, read what kind of gesture is and print out message to matrix.
      case DIR_UP:
        matrix.message("^^ Gesture UP ^^", 100, 3, -1);
        break;
      case DIR_DOWN:
        matrix.message("vv Gesture DOWN vv", 100, 3, -1);
        break;
      case DIR_LEFT:
        matrix.message("<< Gesture LEFT <<", 100, 3, -1);
        break;
      case DIR_RIGHT:
        matrix.message(">> Gesture RIGHT >>", 100, 3, -1);
        break;
      case DIR_NEAR:
        matrix.message(")) Gesture NEAR ((", 100, 3, -1);
        break;
      case DIR_FAR:
        matrix.message("(( Gesture FAR ))", 100, 3, -1);
        break;
      default:
        matrix.message("?? Unknown gesture ??", 100, 3, -1);
    }
  }
  matrix.resumeScroll();    //Continue with scrolling after we read all data from sensor.
  delay(100);               //Delay beetwen reads to avoid false reading and ESP8266 watchdog.
}
