#ifndef __SDA5708
#define __SDA5708

#include "fonts.h"
#include <Arduino.h>
#include <avr/pgmspace.h>



#define DISPLAY_LENGTH 8
#define BUFFER_SIZE 9

class SDA5708 {
    public:
        SDA5708(int loadPin, int dataPin, int sdclkPin, int resetPin);
        SDA5708(int loadPin, int dataPin, int sdclkPin, int resetPin, uint8_t brightness, uint16_t scrollSpeed, uint16_t scrollDelay, char *delim);

        void clearDisplay();
        void refreshDisplay();

        void setBrightness(uint8_t brightnessToSet);
        uint8_t getBrightness();
        void setText(char *text);
        void updateText(char *text);
        char* getText();
        void setDelimiterChar(char *text);
        char* getDelimiterChar();
        void setScrollSpeed(int speed);
        int getScrollSpeed();
        void setScrollDelay(int delay);
        int getScrollDelay();
        
    private:
        uint8_t loadPin;
        uint8_t dataPin;
        uint8_t sdclkPin;
        uint8_t resetPin;
        uint8_t brightness;
        char *displayText;
        unsigned int textLength;
        char buffer[BUFFER_SIZE];

        uint16_t scrollSpeed = 500;
        uint16_t scrollDelay = 1000;
        uint16_t scrollIndex = 0;
        unsigned long previousScrollMillis = 0;
        char *delimiterChar = "   ";
        uint8_t delimLength = 3;
        
        void shiftCharsToScroll();
        void initSDA5708();
        void sendByte(uint8_t byte);
        void writeDigit(uint8_t sign, uint8_t digit);
        void printToDisplay(char *text);
};


#endif