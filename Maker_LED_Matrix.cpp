#include "Maker_LED_Matrix.h"        //Header file of library.

#include <Wire.h>                 //Arduino Wire library.
#include <Ticker.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
Ticker updateLED = Ticker();
Maker_LED_Matrix *j;
void (*userFunc)(void);


Maker_LED_Matrix::Maker_LED_Matrix(uint8_t n) : Adafruit_GFX(16*n, 9) {
	displayXSize = 16*n;
	_n = n;
}
//--------------------------------------LOW LEVEL FUNCTIONS FOR DISPLAYING A MESSAGE--------------------------------------

void Maker_LED_Matrix::writeMessage() {
	//This part of code scrolls message on display (also sends data to display).
	if (_position < _scroll) {
		_position = displayXSize;                                   //If text has come to end, bring it back to start.
		if(_repeats && _repeats != -1) _repeats--;        //If it still has some repeats to do, decrement variable that keeps track of that.
		_messageRepeats++;                                //And also, increment variable that keeps track of how many times message has been repeated on display.
	}

	if(!_repeats) {                                      //Repeats are not equal zero? Keep screen updated!
		updateLED.detach();
		return;
	}
	j->setOffset(0);
	j->fillRect(0, 0, displayXSize, 9, _backBrightness);     //Delete everything from screen, using filled rect. that has same color (Brightness) sa background color.
	j->setOffset(_position);            //Set cursor at the start of the display.
	j->setCursor(0,0);
	j->print(msgBuffer);                   //Send text to the left side of display (becouse display is made form 2 seperate LED matrix drivers that are not chained together).
	//Wire.begin(4,5);
	j->display();    //Display data that is written into buffer.
	_position -= _step;                         //Increment counter for scrolling.
}

void Maker_LED_Matrix::writePicture() {
	if (_position < _scroll) {
		_position = displayXSize;                                   //If text has come to end, bring it back to start.
		if(_repeats && _repeats != -1) _repeats--;        //If it still has some repeats to do, decrement variable that keeps track of that.
		_messageRepeats++;                                //And also, increment variable that keeps track of how many times message has been repeated on display.
	}

	if(!_repeats) {                                      //Repeats are not equal zero? Keep screen updated!
		updateLED.detach();
		return;
	}
	j->setOffset(0);
	j->fillRect(0, 0, displayXSize, 9, _backBrightness);     //Delete everything from screen, using filled rect. that has same color (Brightness) sa background color.
	j->setOffset(_position);            //Set cursor at the start of the display.
	j->drawBitmap(0, 0, _pic, _picSizeX, _picSizeY, _brightness, _backBrightness);
	//Wire.begin(4,5);
	j->display();    //Display data that is written into buffer.
	_position -= _step;                         //Increment counter for scrolling.
}

void Maker_LED_Matrix::writeGreyscale() {
	if (_position < _scroll) {
		_position = displayXSize;                                   //If text has come to end, bring it back to start.
		if(_repeats && _repeats != -1) _repeats--;        //If it still has some repeats to do, decrement variable that keeps track of that.
		_messageRepeats++;                                //And also, increment variable that keeps track of how many times message has been repeated on display.
	}

	if(!_repeats) {                                      //Repeats are not equal zero? Keep screen updated!
		updateLED.detach();
		return;
	}
	j->setOffset(0);
	j->fillRect(0, 0, displayXSize, 9, _backBrightness);     //Delete everything from screen, using filled rect. that has same color (Brightness) sa background color.
	j->setOffset(_position);            //Set cursor at the start of the display.
	j->drawGrayscaleBitmap(0, 0, (uint8_t*)_pic, _picSizeX, _picSizeY, _brightness);
	//Wire.begin(4,5);
	j->display();    //Display data that is written into buffer.
	_position -= _step;                         //Increment counter for scrolling.
}

void Maker_LED_Matrix::writeScroll() {
	if (_position < _scroll) {
		_position = displayXSize;                                   //If text has come to end, bring it back to start.
		if(_repeats && _repeats != -1) _repeats--;        //If it still has some repeats to do, decrement variable that keeps track of that.
		_messageRepeats++;                                //And also, increment variable that keeps track of how many times message has been repeated on display.
	}

	if(!_repeats) {                                      //Repeats are not equal zero? Keep screen updated!
		updateLED.detach();
		return;
	}
	
	j->setOffset(0);
	j->fillRect(0, 0, displayXSize, 9, _backBrightness);     //Delete everything from screen, using filled rect. that has same color (Brightness) sa background color.
	j->setOffset(_position); 
	userFunc();
	//Wire.begin(4,5);
	j->display();
	_position -= _step;
}
//--------------------------------------FUNCTIONS FOR INIT--------------------------------------

int Maker_LED_Matrix::begin(Maker_LED_Matrix *g) {
	_brightness = 32;                             //Set default levels for font brightness, background brightness, delay between scrolls and scroll step.
	_backBrightness = 2;
	_pause = 150;
	_step = 1;
	j = g;
	Wire.begin(4, 5);
	twi_setClock(400000);
	
	for(int i = 0; i<_n; i++) {
		//shutdown
		writeData(_adr[i], ISSI_BANK_FUNCTIONREG, ISSI_REG_SHUTDOWN, 0x00);
		delay(10);
		// out of shutdown
		writeData(_adr[i], ISSI_BANK_FUNCTIONREG, ISSI_REG_SHUTDOWN, 0x01);
		// picture mode
		writeData(_adr[i], ISSI_BANK_FUNCTIONREG, ISSI_REG_CONFIG, ISSI_REG_CONFIG_PICTUREMODE);
		//displayFrame(_frame);
		writeData(_adr[i], ISSI_BANK_FUNCTIONREG, ISSI_REG_PICTUREFRAME, 0);
		//audioSync(false);
		writeData(_adr[i], ISSI_BANK_FUNCTIONREG, ISSI_REG_AUDIOSYNC, 0x0);
	}
	clearDisplay();
	display();
	
	for(int k = 0; k<_n; k++) {
		for (uint8_t f=0; f<8; f++) {
			for (uint8_t i=0; i<=0x11; i++)
			writeData(_adr[k], f, i, 0xff);     // each 8 LEDs on
		}
	}
	setTextWrap(false);         //Turn of text wrapin, we do not need that on this screen.
	setTextColor(_brightness, _backBrightness);
	setRotation(0);
}

void Maker_LED_Matrix::brightness(uint8_t _fontLight, uint8_t _backingLight) {
	_brightness = _fontLight;                               //If user want to change brightness of screen, they can use this function.
	_backBrightness = _backingLight;
	setTextColor(_brightness, _backBrightness);
}

//--------------------------------------FUNCTION FOR WRITING MESSAGE--------------------------------------
void Maker_LED_Matrix::message(char* msg, int _ms, int _stp, int _rep) {
	if(_ms < 75 ) _ms = 75;
	if(_stp < 1 ) _stp = 1;
	if(_rep < -1) _rep = -1;//It doesn't make any sense to make delay less than 10 ms, especially negative one and step smaller than 1, so if that happens, set preset values.
	if(strlen(msg) > _BUFFERSIZE) return;   //If size of string is much bigger of size of buffer, do not do anything more, it will overflow.
	_step = _stp;                                   //Save what user want for delay and step for scrolling.
	_pause = _ms;
	_repeats = _rep;
	_dispMode = 1;

	updateLED.detach();                                 //Remove timer, just in case.
	_messageRepeats = 0;                //If there is a new message, reset counter!
	_position = displayXSize;                     //Calling this function means that user whats to write some new message, so bring scroll counter to start and clear buffer for message.
	
	memset(msgBuffer, 0, _BUFFERSIZE);
	memcpy(msgBuffer, msg, strlen(msg));    //Copy new message to the buffer.
	clearDisplay();
	_maxPx = 0;
	print(msgBuffer);
	_scroll = -1*(_maxPx);


	if(_repeats) {                                        //If number of repeats is zero, that means that text is not scrolling, do not update screen.
		updateLED.attach_ms(_pause, &Maker_LED_Matrix::writeMessage);          //Setup timer with new parameters for new message.
	}
	if(!_repeats) {                                     //If repeats are equal to zero, that means that we do not want to scroll it, we just want to print message with picture.
		_repeats = 1;
		_position = 0;                                    //In that case, set position to start of the screen.
		writeMessage();
	}
}

void Maker_LED_Matrix::picture(const uint8_t* p, uint8_t sizex, uint8_t sizey, int _ms, int _stp, int _rep) {
	if(_ms < 75 ) _ms = 75;
	if(_stp < 1 ) _stp = 1;
	if(_rep < -1) _rep = -1;//It doesn't make any sense to make delay less than 10 ms, especially negative one and step smaller than 1, so if that happens, set preset values.
	if(p==NULL) return;
	_pic = p;
	_picSizeX = sizex;
	_picSizeY = sizey;
	_step = _stp;                                   //Save what user want for delay and step for scrolling.
	_pause = _ms;
	_repeats = _rep;
	_dispMode = 2;

	updateLED.detach();                                 //Remove timer, just in case.
	_messageRepeats = 0;                //If there is a new message, reset counter!
	_position = displayXSize;                     //Calling this function means that user whats to write some new picure, so bring scroll counter to start and clear buffer for message.

	clearDisplay();
	_maxPx = 0;
	drawBitmap(0, 0, _pic, _picSizeX, _picSizeY, _brightness, _backBrightness);
	_scroll = -1*(_maxPx);


	if(_repeats) {                                        //If number of repeats is zero, that means that text is not scrolling, do not update screen.
		updateLED.attach_ms(_pause, &Maker_LED_Matrix::writePicture);          //Setup timer with new parameters for new message.
	}
	if(!_repeats) {                                     //If repeats are equal to zero, that means that we do not want to scroll it, we just want to print message with picture.
		_repeats = 1;
		_position = 0;                                    //In that case, set position to start of the screen.
		writePicture();
	}
}

void Maker_LED_Matrix::picture8Bit(const uint8_t* p, int sizex, int sizey, int _ms, int _stp, int _rep) {
	if(_ms < 75 ) _ms = 75;
	if(_stp < 1 ) _stp = 1;
	if(_rep < -1) _rep = -1;//It doesn't make any sense to make delay less than 10 ms, especially negative one and step smaller than 1, so if that happens, set preset values.
	if(p==NULL) return;
	_pic = p;
	_picSizeX = sizex;
	_picSizeY = sizey;
	_step = _stp;                                   //Save what user want for delay and step for scrolling.
	_pause = _ms;
	_repeats = _rep;
	_dispMode = 3;

	updateLED.detach();                                 //Remove timer, just in case.
	_messageRepeats = 0;                //If there is a new message, reset counter!
	_position = displayXSize;                     //Calling this function means that user whats to write some new picure, so bring scroll counter to start and clear buffer for message.

	clearDisplay();
	_maxPx = 0;
	drawGrayscaleBitmap(0, 0, (uint8_t*)_pic, _picSizeX, _picSizeY, _brightness);
	_scroll = -1*(_maxPx);


	if(_repeats) {                                        //If number of repeats is zero, that means that text is not scrolling, do not update screen.
		updateLED.attach_ms(_pause, &Maker_LED_Matrix::writeGreyscale);          //Setup timer with new parameters for new message.
	}
	if(!_repeats) {                                     //If repeats are equal to zero, that means that we do not want to scroll it, we just want to print message with picture.
		_repeats = 1;
		_position = 0;                                    //In that case, set position to start of the screen.
		writeGreyscale();
	}
}

void Maker_LED_Matrix::scrollAll(void (*p)(void), int _ms, int _stp, int _rep) {
	if(_ms < 75) _ms = 75;                    //It doesn't make any sense to make delay less than 75 ms, especially negative one and step smaller than 1, so if that happens, set preset values.
	if(_stp < 1 ) _stp = 1;
	if(_rep < -1) _rep = -1;
	if(p == NULL) return;
	
	updateLED.detach();
	userFunc = p;
	_step = _stp;
	_pause = _ms;
	_repeats = _rep;
	_messageRepeats = 0;
	_dispMode = 4;
	_position = displayXSize;
	
	clearDisplay();
	_maxPx = 0;
	p(); //Calculate max scroll
	_scroll = (_maxPx) * (-1);
	if(_repeats) {
		updateLED.attach_ms(_pause, &Maker_LED_Matrix::writeScroll);
	}
	
	if(!_repeats) {
		_repeats = 1;
		_position = 0;
		writeScroll();
	}
}

void Maker_LED_Matrix::stopScroll() {        //Function that stops message from scrolling (just remove timmer).
	updateLED.detach();
}

void Maker_LED_Matrix::resumeScroll() {         //Function that resume scrolling message on the screen.         
	if(_dispMode == 1) updateLED.attach_ms(_pause, &Maker_LED_Matrix::writeMessage);
	if(_dispMode == 2) updateLED.attach_ms(_pause, &Maker_LED_Matrix::writePicture);
	if(_dispMode == 3) updateLED.attach_ms(_pause, &Maker_LED_Matrix::writeGreyscale);
	if(_dispMode == 4) updateLED.attach_ms(_pause, &Maker_LED_Matrix::writeScroll);
}

void Maker_LED_Matrix::deleteScroll() {                //Function removes message form display and memory.
	updateLED.detach();                                //Remove timmer (do not refresh display anymore).
	if(_dispMode == 1) {           //If it's mode 1 od 3 that means that we need to delete some text.
		for (int i = 0; i < 500; i++) {                  //Clean up buffer.
			msgBuffer[i] = 0;
		}
	}
	if(_dispMode == 2 || _dispMode == 3) {
		_pic = NULL;
		_picSizeX = 0;
		_picSizeY = 0;
	}
	if(_dispMode == 4) userFunc = NULL;
	fillRect(0, 0, displayXSize, 9, _backBrightness);    //Delete everything from screen by writing filled rect on screen that has same intensity as background.
}

int Maker_LED_Matrix::repeatCount() {                  //Function retrun how many times message has been displayed and repeated. Useful when user want to know that message is displayed till the end.
	return _messageRepeats;  
}

int16_t Maker_LED_Matrix::getPosition() {
	return (_position*(-1));
}

int16_t Maker_LED_Matrix::getXSize() {
	return (_scroll*(-1));
}
//---------------------------ADAFRUIT GFX----------------------------------
void Maker_LED_Matrix::drawPixel(int16_t x, int16_t y, uint16_t color) {
	if(x > _maxPx) _maxPx = x;
	if ((x < 0) || (x >= displayXSize)) return;
	if ((y < 0) || (y >= 9)) return;
	if (color > 255) color = 255; // PWM 8bit max
	// check rotation, move pixel around if necessary
	switch (getRotation()) {
	case 1:
		_swap_int16_t(x, y);
		x = 16 - x - 1;
		break;
	case 2:
		x = 16 - x - 1;
		y = 9 - y - 1;
		break;
	case 3:
		_swap_int16_t(x, y);
		y = 9 - y - 1;
		break;
	}
	d.full[y][x] = color;
	return;
}

uint8_t Maker_LED_Matrix::getPixel(int16_t x, int16_t y) {
	return d.full[y][x];
}

void Maker_LED_Matrix::display() {
	Wire.begin(IS31FL3731_SDA, IS31FL3731_SCL);
	for(int k = 0; k<_n; k++) {
		Wire.beginTransmission(_adr[k]);
		Wire.write((byte)ISSI_COMMANDREGISTER);
		Wire.write(0);
		Wire.endTransmission(0);
		
		Wire.beginTransmission(_adr[k]);
		Wire.write((byte)0x24);
		int i, j, n = 0, z;
		for(i = 0; i<5; i++) {
			for(j = 0;j<16;j++) {
				Wire.write((byte)d.send[i][k][j]);
			}
		}
		Wire.endTransmission(0);
		
		Wire.beginTransmission(_adr[k]);
		Wire.write((byte)0x24+80);
		for(i = 5; i<9; i++) {
			for(j = 0;j<16;j++) {
				Wire.write((byte)d.send[i][k][j]);
			}
		}
		Wire.endTransmission();
	}
}

void Maker_LED_Matrix::clearDisplay() {
	memset(d.cl, 0, 576);
	setCursor(0,0);
	setOffset(0);
}

void Maker_LED_Matrix::drawGrayscaleBitmap(int16_t x0, int16_t y0, uint8_t *p, uint16_t xSize, uint16_t ySize, uint8_t bright) {
	int endPosX = xSize + x0;    //Calculate end position for x cooridinate (needed for x position offest).
	int endPosY = ySize + y0;

	int n = 0;                                         //Variable for indexing picture array
	for (int j = y0; j < endPosY; j++) {                  //First select row, then write data into seleced row
		for (int i = x0; i < endPosX; i++) {
			writePixel(i, j + y0, (uint8_t)(bright * pow(p[n] / 255.0, _GAMMA)));
			n++;                   //Increment variable for array indexing
		}
	}
}

//---------------------------------LOW LEVEL CONTROL (do not change anything, only if you know what are you doing)---------------
void Maker_LED_Matrix::writeData(uint8_t _adr, uint8_t b, uint8_t reg, uint8_t data) {
	Wire.begin(IS31FL3731_SDA, IS31FL3731_SCL);
	Wire.beginTransmission(_adr);
	Wire.write((byte)ISSI_COMMANDREGISTER);
	Wire.write(b);
	Wire.endTransmission();

	Wire.beginTransmission(_adr);
	Wire.write((byte)reg);
	Wire.write((byte)data);
	Wire.endTransmission();
}

void Maker_LED_Matrix::shutdown() {
	for(int i = 0; i<_n; i++) {
		writeData(_adr[i], ISSI_BANK_FUNCTIONREG, ISSI_REG_SHUTDOWN, 0x00);
	}
}

void Maker_LED_Matrix::powerUp() {
	for(int i = 0; i<_n; i++) {
		writeData(_adr[i], ISSI_BANK_FUNCTIONREG, ISSI_REG_SHUTDOWN, 0x01);
	}
}

//--------------------------------------FUNCTIONS FOR WEB--------------------------------------
int Maker_LED_Matrix::wifiNetwork(const char* _ssid, const char* _pass) {
	int retry = 20;                              //Number of retrys for connectin on WLAN network.
	WiFi.mode(WIFI_STA);                         //Setting up WiFi module mode.
	WiFi.begin(_ssid, _pass);                    //Try to connect to desiered network.
	do {                                         //Wait and keep checking if it's connected to network until we are really connected or we run out of retrys
		delay(500);
		retry--;
	} while (WiFi.status() != WL_CONNECTED && retry != 0);
	if(retry) {                                  //If we still have any more retrys left, that means we are successfuly connected to WiFi network.
		wlanSuccess = 1;
		return 1;                                 //Return 1 for successful conection to WiFi.
	}else{
		return 0;                                 //But if we are not connected, return 0 fro fail.
	}
}

int Maker_LED_Matrix::webPage(char* url,  int _ms, int _stp, int _rep) {
	if (!wlanSuccess) return 0;                          //If we are not connected to WiFi, return 0, becouse we can't loar web page if we do not have at least WiFi connection.
	if(_ms < 75 || _stp < 1) return 0;                   //It doesn't make any sense to make delay less than 75 ms, especially negative one and step smaller than 1, so if that happens, return.

	WiFiClient client;
	HTTPClient http;
	if (http.begin(client, url)) {
		int httpCode = http.GET();
		if (httpCode == HTTP_CODE_OK) {
			String payload = http.getString();
			char webText[_BUFFERSIZE];
			payload.toCharArray(webText, payload.length());
			int webTextSize = strlen(webText);                                 //Calculate the lenght of string.
			
			//This part of code finds Croatian letters and change them into ASCII letters. Not really nesscery for this library.
			for (int16_t i = 0; i < webTextSize; i++) {
				if (webText[i] == 196 || webText[i] == 197) memmove(&webText[i], &webText[i + 1], webTextSize - i);
				if (webText[i] == 141 || webText[i] == 135) webText[i] = 'c';
				if (webText[i] == 161) webText[i] = 's';
				if (webText[i] == 190) webText[i] = 'z';
			}
			message(webText, _ms, _stp, _rep);        //Send message to display with desiered delay and step for scrolling
		} else {
			message("Failed to open web", _ms, _stp, -1); 
			return 0;
		}
		http.end();
	}
	return 1;                           //Everything went ok? Return 1 for success!
}

int Maker_LED_Matrix::webPageText(char* url, char* webText, int _n) {  //This function grabs textz form web page and saves it in user defined string that is send as function argument.
	if (!wlanSuccess) return 0;                                                         //Everything else is basicly the same as previous function, with only one exception, it does not write anything on dipslay. Useful if we want to parse something and than display it.

	WiFiClient client;
	HTTPClient http;
	if (http.begin(client, url)) {
		int httpCode = http.GET();
		if (httpCode == HTTP_CODE_OK) {
			String payload = http.getString();
			payload.toCharArray(webText, _n);
			int webTextSize = strlen(webText);                                 //Calculate the lenght of string.
			
			//This part of code finds Croatian letters and change them into ASCII letters. Not really nesscery for this library.
			for (int16_t i = 0; i < _n; i++) {
				if (webText[i] == 196 || webText[i] == 197) memmove(&webText[i], &webText[i + 1], _n - i);
				if (webText[i] == 141 || webText[i] == 135) webText[i] = 'c';
				if (webText[i] == 161) webText[i] = 's';
				if (webText[i] == 190) webText[i] = 'z';
			}
		} else {
			return 0;
		}
		http.end();
	}
	return 1;                           //Everything went ok? Return 1 for success!
}
