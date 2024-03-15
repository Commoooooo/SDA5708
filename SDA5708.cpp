#include "SDA5708.h"
#include <Arduino.h>
#include <string.h>

SDA5708::SDA5708(int loadPin, int dataPin, int sdclkPin, int resetPin) {
    this->loadPin = loadPin;
    this->dataPin = dataPin;
    this->sdclkPin = sdclkPin;
    this->resetPin = resetPin;
	pinMode(dataPin, OUTPUT);      
  	pinMode(loadPin, OUTPUT);      
  	pinMode(sdclkPin, OUTPUT);      
  	pinMode(resetPin, OUTPUT);    
    initSDA5708();
	memset(buffer, ' ', BUFFER_SIZE);
    buffer[BUFFER_SIZE - 1] = '\0';
}

SDA5708::SDA5708(int loadPin, int dataPin, int sdclkPin, int resetPin, uint8_t brightness, uint16_t scrollSpeed, uint16_t scrollDelay, char *delim){
	this->loadPin = loadPin;
    this->dataPin = dataPin;
    this->sdclkPin = sdclkPin;
    this->resetPin = resetPin;
	this->brightness = brightness;
	this->scrollSpeed = scrollSpeed;
	this->scrollDelay = scrollDelay;
	this->delimiterChar = delim;
	this->delimLength = strlen(delim);
	pinMode(dataPin, OUTPUT);      
  	pinMode(loadPin, OUTPUT);      
  	pinMode(sdclkPin, OUTPUT);      
  	pinMode(resetPin, OUTPUT);    
    initSDA5708();
	memset(buffer, ' ', BUFFER_SIZE);
    buffer[BUFFER_SIZE - 1] = '\0';
}

void SDA5708::clearDisplay() {
	memset(buffer, ' ', 8);
	refreshDisplay();
}

void SDA5708::refreshDisplay() {
	if (textLength < BUFFER_SIZE) {
		printToDisplay(buffer);
		return;
	}
	if (textLength >= BUFFER_SIZE) {
		//SCROLL
		unsigned long currentMillis = millis();
		if(scrollIndex == 1) {
			if (currentMillis - previousScrollMillis >= scrollDelay) {
				previousScrollMillis = currentMillis;
				shiftCharsToScroll();
			}
		} else {
			if (currentMillis - previousScrollMillis >= scrollSpeed) {
				previousScrollMillis = currentMillis;
				shiftCharsToScroll();
			}
		}
		printToDisplay(buffer);
		return;
	}
}

void SDA5708::shiftCharsToScroll() {
	for (int i = 0; i < BUFFER_SIZE - 1; i++) {
    	int index = (i + scrollIndex) % textLength;
        if (index < textLength) {
            strncpy(buffer + i, displayText + index, 1);
        }
	}
    buffer[BUFFER_SIZE] = '\0'; // Null terminate the string
    scrollIndex++;
    if (scrollIndex >= textLength) {
        scrollIndex = 0;
    }
}

void SDA5708::initSDA5708() {
    // RESET toggeln
    digitalWrite(loadPin, HIGH);
    digitalWrite(resetPin, LOW);
    digitalWrite(resetPin, HIGH);
}

void SDA5708::sendByte(uint8_t byte) {
    uint8_t x;	
	// LOAD auf Low
        digitalWrite(loadPin, LOW);
	// jede Byte-Stelle ans Display senden
	for (x=0; x<=7;x++)	{
		// DATA entsprechend setzen
		if ((byte>>x)&1) {
                  digitalWrite(dataPin, HIGH);
                } else {
                  digitalWrite(dataPin, LOW);
                }
		// SDCLK toggeln
                digitalWrite(sdclkPin, HIGH);
                digitalWrite(sdclkPin, LOW);
	}
	// LOAD auf High
      digitalWrite(loadPin, HIGH);
}

void SDA5708::writeDigit(uint8_t sign, uint8_t digit) {
    uint8_t i;
	// anzeigbares Zeichen? Wenn nein, dann Leerzeichen!
	if ((sign < 0x20) || (sign > 0x7b)) sign = 0x20;
	// Digit im Bereich?
	if (digit > 7) digit = 0;
	// Digit-Adresse zum Display senden
	sendByte(0b10100000|digit);
	// jede Zeile des Fonts adressieren, lesen, ausgeben
	for(i=0; i<7; i++) {
		// Font ist linksbuendig, deshalb 3 Bit nach rechts schieben...
		sendByte(pgm_read_byte(&font[(sign-0x20)*7+i])/8);
	}
}

void SDA5708::printToDisplay(char *text) {
    uint8_t cursor=0;
	char *p=text;
	while (*p) {
		writeDigit(*p, cursor);
		cursor++;
		p++;
	}
}

void SDA5708::setBrightness(uint8_t brightnessToSet) {
    this->brightness = brightnessToSet;
    sendByte(0b11100000|((8-this->brightness)&0b00000111));
}

uint8_t SDA5708::getBrightness() {
    return this->brightness;
}

void SDA5708::setText(char *text) {
	this->displayText = text;
	this->textLength = strlen(this->displayText);
	if (textLength == 0) {
		clearDisplay();
		return;
	}
	if (textLength < 9) {
        	strncpy(buffer, displayText, textLength);
        	for (int i = textLength; i < 8; ++i) {
           		buffer[i] = ' ';
       		}
		return;
    }
	if (textLength >= 9) {
		strcat(displayText, delimiterChar);
		textLength = textLength + delimLength;
		return;
	}
}

void SDA5708::updateText(char* text){
	setText(text);
	refreshDisplay();
}

char* SDA5708::getText() {
	return displayText;
}

void SDA5708::setDelimiterChar(char *text) {
	delimLength = strlen(text);
	strncpy(delimiterChar, text, delimLength);
}

char* SDA5708::getDelimiterChar() {
	return delimiterChar;
}

void SDA5708::setScrollSpeed(int speed) {
	this->scrollSpeed = speed;
}

int SDA5708::getScrollSpeed() {
	return scrollSpeed;
}

void SDA5708::setScrollDelay(int delay) {
	this->scrollDelay = delay;
}

int SDA5708::getScrollDelay() {
	return scrollDelay;
}















