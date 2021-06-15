/*
  This library should be used with arduino IDE on e-radionica Social Display Module.
  
  Used librarys:
  Adafruit IS31FL3731 Library: https://github.com/adafruit/Adafruit_IS31FL3731 (modified)
  Adafruit GFX Library: https://github.com/adafruit/Adafruit-GFX-Library (also modified)
  Ticker Library for timer: https://github.com/esp8266/Arduino/tree/master/libraries/Ticker
*/

#ifndef _MAKER_LED_MATRIX_H_
#define _MAKER_LED_MATRIX_H_
#include "Adafruit_GFX.h"         //Adafruit GFX library (modified).

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define _BUFFERSIZE 800                        //Size of buffer for displying messages.
#define _GAMMA 2.2
#define MAKER_LED_MATRIX_2 2
#define MAKER_LED_MATRIX_4 4
#define IS31FL3731_SDA 4
#define IS31FL3731_SCL 5

#define ISSI_REG_CONFIG  0x00
#define ISSI_REG_CONFIG_PICTUREMODE 0x00
#define ISSI_REG_CONFIG_AUTOPLAYMODE 0x08
#define ISSI_REG_CONFIG_AUDIOPLAYMODE 0x18
#define ISSI_CONF_PICTUREMODE 0x00
#define ISSI_CONF_AUTOFRAMEMODE 0x04
#define ISSI_CONF_AUDIOMODE 0x08
#define ISSI_REG_PICTUREFRAME  0x01
#define ISSI_REG_SHUTDOWN 0x0A
#define ISSI_REG_AUDIOSYNC 0x06
#define ISSI_COMMANDREGISTER 0xFD
#define ISSI_BANK_FUNCTIONREG 0x0B

#ifndef _swap_int16_t
#define _swap_int16_t(a, b) { int16_t t = a; a = b; b = t; }
#endif

static uint8_t _brightness;                    //Variables for font and background brightness and also for delay between shifts and shift (scroll) step.
static uint8_t _backBrightness;
static uint8_t _pause;
static uint8_t _step;
static uint8_t displayXSize;
static uint8_t _n;
static char msgBuffer[_BUFFERSIZE];                    //Buffer for user defined message, max size of 799 characters.
static int _msgSize;                           //Variable that holds a size of user message.
static int32_t _scroll;                            //Variable for scrolling text on display (defines maximal scroll steps for the whole message to be scrolled).
static int32_t _position;                          //Variable that moves position of text on screen.
static const uint8_t* _pic;
static uint8_t _picSizeX, _picSizeY;
static uint8_t wlanSuccess = 0;                //Variable that holds information of WiFi connectivity.
static int _repeats = 0;                       //Variable that keep number that says how many more times message have to be displayed.
static uint16_t _messageRepeats = 0;           //Variable that keeps track how many times message has been displayed (repeated).
static const uint8_t _adr[4] = {0x74, 0x77, 0x75, 0x76};

class Maker_LED_Matrix : public Adafruit_GFX {
      public:
            Maker_LED_Matrix(uint8_t n);
            void begin(Maker_LED_Matrix *g);                 
			void drawPixel(int16_t x, int16_t y, uint16_t color);
			void display();
            void brightness(uint8_t _fontLight, uint8_t _backingLight);      //Sets up font brightness and background brightness.
            void message(char* msg, int _ms, int _stp, int _rep); //Writes user defined message/text on display.
			void picture(const uint8_t* p, uint8_t sizex, uint8_t sizey, int _ms, int _stp, int _rep);
			void picture8Bit(const uint8_t* p, int sizex, int sizey, int _ms, int _stp, int _rep);
			void scrollAll(void (*p)(void), int _ms, int _stp, int _rep);
            void stopScroll();                                    //Calling this function scrolling will stop.
            void resumeScroll();                                  //Calling this function scrolling will continue.
            void deleteScroll();                                  //Function removes everything from screen and clears buffer.
            int repeatCount();                                    //Function return how much times message has been displayed
			int16_t getPosition();
			int16_t getXSize();
			
            void drawPixel(int, int, int);
			uint8_t getPixel(int16_t x, int16_t y);
			void clearDisplay();
			void writeData(uint8_t _adr, uint8_t b, uint8_t reg, uint8_t data);
			void drawGrayscaleBitmap(int16_t x0, int16_t y0, uint8_t *p, uint16_t xSize, uint16_t ySize, uint8_t bright);
			void shutdown();
			void powerUp();
			
            int wifiNetwork(const char* _ssid, const char* _pass);                            //Funcion connects to WLAN network.
            int webPage(char* url, int _ms, int _stp, int _rep);         //Function opens web page and displays text on LED matrix.
            int webPageText(char* url, char* webText, int _n);               //Function opens a web and saves data into a string without displayig on matrix.
            int _dispMode;
			union d {
				uint8_t cl[576];
				uint8_t full [9][64];
				uint8_t send [9][4][16];
			}d;
			static void writeMessage();
			static void writeScroll();
			static void writePicture();
			static void writeGreyscale();
			int16_t _maxPx;
	  private:
};
#endif
