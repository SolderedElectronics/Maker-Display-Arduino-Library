/*
   This is example code on how to use Maker LED Matrix Dispaly library on e-radionica Maker LED Matrix Display Board with ESP8266 and two or four IL31LF3731 drivers.
   It shows how to print text/message to display, write picture, scroll it, pause it and remove it.
   It also shows how to use function from Adafruit GFX to draw lines, circles, rects, bitmaps, text and scroll all that.
*/

#include "Maker_LED_Matrix.h"       //First, we have to include library to our sketch!
//Then we make an object to a class. We call this object matrix, but you can name it whatever you want. But remember to use that name instead od matrix in rest of the code!
//If you're using Maker LED Matrix 2 (with resolution of 32x9) than use this
//Maker_LED_Matrix matrix(MAKER_LED_MATRIX_2);

//...otherwise use this
Maker_LED_Matrix matrix(MAKER_LED_MATRIX_4);

//Let's make some bitmap picture. To convert your bitmap image to bytes, follow this tutorial https://learn.adafruit.com/steampunk-cameo-necklace/bitmaps-and-code
const uint8_t sine[] PROGMEM = {
  0x03, 0xc0, 0x00, 0x00,
  0x0c, 0x30, 0x00, 0x00,
  0x30, 0x0c, 0x00, 0x00,
  0x40, 0x02, 0x00, 0x00,
  0x80, 0x01, 0x00, 0x00,
  0x00, 0x00, 0xc0, 0x03,
  0x00, 0x00, 0x20, 0x0c,
  0x00, 0x00, 0x1c, 0x70,
  0x00, 0x00, 0x03, 0x80,
};

//And now, let's make 8 bit greyscale bitmap! To convert your picture to array of bytes (like one down under) follow this instructions!
/*For converting greyscale image, use LCD Image Converter.
 * Open program->New Image->Type Image Name and click ok->Image->Import
 * Then select image that you want to convert (note that image should not be bigger than dispaly. If it is, use Paint to resize it.
 * Options->Conversion
 * Preset should be on Grayscale 8, Type Grayscale, Inverse selected, main scan direction Top to bottom
 * Line scan direction Forward. Than click on Image, block size should be 8 bit and byte order to little endian.
 * By clicking Show Preview, you should see image as well as data array. Colors on image should be inversed.
 * Click ok, save it, then File->Convert and save your data array. Open it and copy here.
*/
const uint8_t flame[288] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x93, 0x93, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x93, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x93, 0x69, 0x93, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x93, 0x93, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x93, 0x93, 0x93, 0x93, 0x00, 0x00, 0x00, 0x93, 0x69, 0x69, 0x93, 0x93, 0x93, 0x00, 0x93, 0x93, 0x69, 0x93, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x93, 0x93, 0x93, 0x93, 0x69, 0x93, 0x93, 0x93, 0x93, 0x69, 0x69, 0x69, 0x69, 0x93, 0x93, 0x93, 0x69, 0x69, 0x93, 0x93, 0x93, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x93, 0x93, 0x93, 0x93, 0x69, 0x69, 0x3f, 0x69, 0x69, 0x69, 0x69, 0x3f, 0x3f, 0x69, 0x69, 0x69, 0x69, 0x3f, 0x69, 0x69, 0x93, 0x93, 0x93, 0x93, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x93, 0x93, 0x69, 0x69, 0x3f, 0x3f, 0x0f, 0x0f, 0x3f, 0x3f, 0x3f, 0x0f, 0x3f, 0x3f, 0x3f, 0x0f, 0x3f, 0x3f, 0x69, 0x69, 0x69, 0x93, 0x93, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x93, 0x93, 0x69, 0x69, 0x3f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x3f, 0x69, 0x93, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


void setup() {
  Serial.begin(9600);   //Fire up Serial monitor at 9600 bauds.
  matrix.begin(&matrix);  //Init our library. As argument we are sending address of out library object. It's nessery to do that, otherwise, we are unable to refresh LED Matrix.
  matrix.brightness(25, 1);   //Setup a new brightness. Setup font brigthenss at 25 of 255, and background at 1 of 255 (this makes backlight effect, put zero if you don't wanna that).

  //Finally, let's write something to display. Let's set 75 ms beetwen step and maketwo pixel steps and only one repet of message.
  //If you want to repeat this message all the time, set repeat value to -1 instead of 1. And if you want it to be static (ndoes not scroll) put zero instead 1 for repeats.
  matrix.message("Hello! :) This is Maker LED Matrix Display made by e-radionica.com", 75, 2, 1);
 
  while(matrix.repeatCount() != 1) {  //And now we are waiting end of the message by waiting number of repeats to be exacly one
    delay(100);
  }
  
  matrix.picture(sine, 32, 9, 100, 1, 2); //Message is boring, let's write scrolling picture to display with dimensions of 32x9 pixels, 100 ms pause beetwen steps, one pixel step and only two repeats! :D
  delay(4000);
  matrix.stopScroll();   //Want to pause scroll? No problem! Just call this function.
  delay(2000);
  matrix.resumeScroll();    //After little break, let's continue with scrolling.
  delay(4000);
  matrix.picture(sine, 32, 9, 100, 1, 0);   //And if you want to be static image, without scrolling, just set repets to zero, easy.
  delay(2000);

  //Now, this is important! You can have background color (aka "backlight") but, background color of image would be black, so it's better to turn off background color.
  matrix.brightness(64, 0); //Font brightness also affects brightness of image.
  matrix.picture8Bit(flame, 32, 9, 75, 1, -1);  //Write your 8 bit grayscale image to display.
  
  while(matrix.repeatCount()!=3) {    //Wait until picrure has been displayed 3 times
    delay(100);
  }
  
  matrix.picture8Bit(flame, 32, 9, 75, 1, 0); //And same as before, you can make it static by setting number of repeats to zero
  delay(2000);          //Wait a little.

  matrix.deleteScroll();  //Delete everything that is on display.

  matrix.message("You can even shutdown drivers to save energy........see?", 75, 1, -1);
  delay(24000);
  matrix.shutdown();
  delay(2000);
  matrix.powerUp();
  delay(6000);
  
  matrix.brightness(25, 1);                   //Set a new brightness.

  //Now, this is cool! Make a function with void as return type and no parametes (you can name it whatever you want). Inside this funcion,
  //you can use any function of Adafruit GFX (except drawPixel, instead use writePixel();) and it will scroll!
  matrix.scrollAll(func1, 100, 1, -1);
}

void loop() {
  //And all this is happening in background, so you can do your regular stuff, like writing data to serial. Neat!
  Serial.print("Content has been displayed ");
  Serial.print(matrix.repeatCount(), DEC);
  Serial.println(" times");
  delay(2000);
}

void func1() {
  matrix.writePixel(1, 4, 128);
  matrix.setCursor(3, 0);
  matrix.print("Adafruit GFX functions! :D ");
  matrix.fillRect(160, 1, 5, 4, 15);
  matrix.drawLine(180, 8, 230, 0, 128);
  matrix.setCursor(240, 0);
  matrix.print("...and this is end!");
}
