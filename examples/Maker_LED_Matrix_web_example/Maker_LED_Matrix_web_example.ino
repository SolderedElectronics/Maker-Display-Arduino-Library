/*
   This is example code on how to use Maker LED Matrix Dispaly library on e-radionica Maker LED Matrix Display Board with ESP8266 and two or four IL31LF3731 drivers.
   It shows how to connect to WiFi network and print text/message to display from web.

   This example is made by e-radionica.com
*/

#include "Maker_LED_Matrix.h"       //First, we have to include library to our sketch!
//Then we make an object to a class. We call this object matrix, but you can name it whatever you want. But remember to use that name instead od matrix in rest of the code!
//If you're using Maker LED Matrix 2 (with resolution of 32x9) than use this
//Maker_LED_Matrix matrix(MAKER_LED_MATRIX_2);

//...otherwise use this
Maker_LED_Matrix matrix(MAKER_LED_MATRIX_4);

const char* wlan_ssid = "INSERT_YOUR_SSID_NAME_HERE";             //WLAN name network name
const char* wlan_pass = "INSERT_YOUR_WIFI_PASSWORD_HERE";         //WLAN network password
char scrollText[100];                       //String for text from web

void setup() {
  Serial.begin(9600);                                       //Setting up a serial comunication.

  matrix.begin(&matrix);
  matrix.message("Connecting to WiFi network...", 150, 1, -1);
  
  if (!matrix.wifiNetwork(wlan_ssid, wlan_pass)) {   //Now let's try to connect to our WiFi network.
    Serial.println("Connecting to WiFi failed!\nCheck WLAN SSID name and WLAN Password.");
    matrix.message("Can't connect to WiFi!", 120, 1, -1);
    do {                               //In that case, stop the program forcing it into inf. loop.
      delay(1);
    } while (true);
  } else {
    Serial.print("Connected to ");  Serial.println(wlan_ssid);
  }

  //Let's display something, while we are trying to open a web page.
  matrix.message("Trying to open web page...", 150, 3, 1);

  //While that message is scrolling, try to open a web page and display content from them on display. In this case todays Weather forecast for Croatia. :) [no longer working :(]
  matrix.webPage("http://api.thingspeak.com/apps/thinghttp/send_request?api_key=8X15NO5QU69EOGCJ", 150, 3, 1);

  do {
    delay(250);
  } while (!matrix.repeatCount());

  //And now, let' try to open web page and save it's content to string and then display it. Useful if parsing is needed before displaying.
  //We are saving tommorow Weather forecast for Croatia [no longer working :(]. String is limited to 99 characters.
  matrix.webPageText("http://api.thingspeak.com/apps/thinghttp/send_request?api_key=8B8OCD7Y2P6KXQK2", scrollText, 100);

  //Now let's print a new message on display, without scrolling.
  matrix.message(scrollText, 150, 2, 0);
  delay(4000);

  //And now we scroll it all the time.
  matrix.message(scrollText, 150, 2, -1);
}

void loop() {
  //Nothing...
}
