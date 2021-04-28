#include "album.h"
#include <Wire.h>
#include <Adafruit_PN532.h>

#define PN532_IRQ (3)
#define PN532_RESET (13) // Not connected by default on the NFC Shield

Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);

#include <Adafruit_NeoPixel.h>
#define PIN 8
#define NUMPIXELS 20
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
#define DELAYVAL 50

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET 4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int gostart[32];
byte count = 0;
byte forward[4] = {200, 0, 185, 0};
byte left[4] = {150, 0, 0, 140}; // 150 150
byte right[4] = {0, 140, 165, 0}; // 150 155
byte back[4] = {0, 200, 0, 185};
byte st = 0;

void myTone(byte pin, uint16_t frequency, uint16_t duration)
{ // input parameters: Arduino pin number, frequency in Hz, duration in milliseconds
  unsigned long startTime = millis();
  unsigned long halfPeriod = 1000000L / frequency / 2;
  pinMode(pin, OUTPUT);
  while (millis() - startTime < duration)
  {
    digitalWrite(pin, HIGH);
    delayMicroseconds(halfPeriod);
    digitalWrite(pin, LOW);
    delayMicroseconds(halfPeriod);
  }
  pinMode(pin, INPUT);
}

void motor(byte MA1, byte MA2, byte MB1, byte MB2, int gotime)
{
  analogWrite(5, MA1);
  analogWrite(6, MA2);
  analogWrite(9, MB1);
  analogWrite(10,MB2);
  delay(gotime);
  analogWrite(5, 0);
  analogWrite(6, 0);
  analogWrite(9, 0);
  analogWrite(10, 0);
  delay(500);
}

int free_ram()
{
  extern int __heap_start, *__brkval;
  int v;
  return (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
}

String code_;

void setup()
{
  Serial.begin(115200);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.~Adafruit_SSD1306();
  display.clearDisplay();
  display.drawBitmap(0, 0, myBitmap00, 128, 64, SSD1306_WHITE);
  display.display(); // Show the display buffer on the screen
  display.display();
  nfc.begin();
  nfc.SAMConfig();
  Serial.println("Waiting for an ISO14443A Card ...");
  pixels.begin();
  pixels.setBrightness(30);
  pixels.clear();
  for (byte i = 0; i < NUMPIXELS; i++)
  {
    pixels.setPixelColor(i, pixels.Color(0, 50, 0));
    pixels.show();
    delay(DELAYVAL);
  }
  pixels.show();
  for (byte i = 0; i < 32; i++)
  {
    gostart[i] = 0;
  }

  Serial.println(free_ram());
}

void showoled()
{
  display.clearDisplay();
  byte x = 0;
  byte y = 0;
  for (byte i = 0; i < 32; i++)
  {
    if (gostart[i] == 1)
    {
      display.drawBitmap(x, y, smallmap01, 12, 12, WHITE);
    }
    else if (gostart[i] == 2)
    {
      display.drawBitmap(x, y, smallmap03, 12, 12, WHITE);
    }
    else if (gostart[i] == 3)
    {
      display.drawBitmap(x, y, smallmap04, 12, 12, WHITE);
    }
    else if (gostart[i] == 4)
    {
      display.drawBitmap(x, y, smallmap02, 12, 12, WHITE);
    }
    else if (gostart[i] == 5)
    {
      display.drawBitmap(x, y, smallmap05, 12, 12, WHITE);
    }
    else if (gostart[i] == 6)
    {
      display.drawBitmap(x, y, smallmap06, 12, 12, WHITE);
    }
    else if (gostart[i] == 7)
    {
      display.drawBitmap(x, y, smallmap07, 12, 12, WHITE);
    }
    else if (gostart[i] == 8)
    {
      display.drawBitmap(x, y, smallmap08, 12, 12, WHITE);
    }
    else if (gostart[i] == 9)
    {
      display.drawBitmap(x, y, smallmap09, 12, 12, WHITE);
    }

    display.display();
    x += 16;
    if (x == 128)
    {
      x = 0;
      y += 16;
    }
  }
}

void loop()
{
  while (1)
  {
    uint8_t success;
    uint8_t uid[] = {0, 0, 0, 0, 0, 0, 0};
    uint8_t uidLength;

    success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
    uint8_t data[16]; // Array to store block data during reads
    uint8_t keyuniversal[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 5, 1, keyuniversal);
    success = nfc.mifareclassic_ReadDataBlock(5, data);
    if (success)
    {
      //nfc.PrintHex(uid, uidLength);
      if (uidLength == 4)
      {
        myTone(11, 1000, 100);
        pixels.clear();
        for (byte i = 0; i < NUMPIXELS; i++)
        {
          pixels.setPixelColor(i, pixels.Color(0, 100, 0));
        }
        pixels.show();
        delay(100);
        for (byte i = 0; i < NUMPIXELS; i++)
        {
          pixels.setPixelColor(i, pixels.Color(0, 50, 0));
        }
        pixels.show();
        pixels.show();
        /*
        uint32_t cardid;
        cardid = uid[0];
        cardid <<= 8;
        cardid |= uid[1];
        cardid <<= 8;
        cardid |= uid[2];
        cardid <<= 8;
        cardid |= uid[3];
        Serial.println(String(cardid));
        */

        //nfc.PrintHexChar(data, 16);
        uint32_t cardid;
        cardid = data[0];
        code_ = String(cardid, HEX);
        Serial.println(String(cardid, HEX));

        //start
        if (code_ == "77")
        {
          display.clearDisplay();              // Clear the display buffer
          display.setTextSize(3);              // Normal 1:1 pixel scale
          display.setTextColor(SSD1306_WHITE); // Draw white text
          display.setCursor(43, 23);           // Start at top-left corner
          display.println(F("GO!"));
          display.display(); // Show the display buffer on the screen
          delay(1000);
          display.clearDisplay();
          display.display();

          for (byte i = 0; i < 32; i++)
          {
            display.clearDisplay();
            if (gostart[i] == 1)
            {
              display.drawBitmap(0, 0, myBitmap01, 128, 64, SSD1306_WHITE);
              display.display(); // Show the display buffer on the screen
              pixels.clear();
              for (byte i = 0; i < NUMPIXELS; i++)
              {
                pixels.setPixelColor(i, pixels.Color(50, 50, 0));
              }
              pixels.show();
              pixels.show();
              motor(forward[0],forward[1],forward[2],forward[3], 438);
            }
            else if (gostart[i] == 2)
            {
              display.drawBitmap(0, 0, myBitmap02, 128, 64, SSD1306_WHITE);
              display.display(); // Show the display buffer on the screen
              pixels.clear();
              for (byte i = 0; i < NUMPIXELS; i++)
              {
                pixels.setPixelColor(i, pixels.Color(50, 0, 50));
              }
              pixels.show();
              pixels.show();
              motor(left[0],left[1],left[2],left[3], 294);
            }
            else if (gostart[i] == 3)
            {
              display.drawBitmap(0, 0, myBitmap03, 128, 64, SSD1306_WHITE);
              display.display(); // Show the display buffer on the screen
              pixels.clear();
              for (byte i = 0; i < NUMPIXELS; i++)
              {
                pixels.setPixelColor(i, pixels.Color(0, 50, 50));
              }
              pixels.show();
              pixels.show();
              motor(right[0],right[1],right[2],right[3], 294);
            }
            else if (gostart[i] == 4)
            {
              display.drawBitmap(0, 0, myBitmap08, 128, 64, SSD1306_WHITE);
              display.display(); // Show the display buffer on the screen
              pixels.clear();
              for (byte i = 0; i < NUMPIXELS; i++)
              {
                pixels.setPixelColor(i, pixels.Color(70, 25, 70));
              }
              pixels.show();
              pixels.show();
              motor(back[0], back[1], back[2], back[3], 438);
            }
            else if (gostart[i] == 5)
            {
              display.clearDisplay();              // Clear the display buffer
              display.setTextSize(2);              // Normal 1:1 pixel scale
              display.setTextColor(SSD1306_WHITE); // Draw white text
              display.setCursor(37, 25);           // Start at top-left corner
              display.println(F("Stop!"));
              display.display(); // Show the display buffer on the screen
              pixels.clear();
              for (byte i = 0; i < NUMPIXELS; i++)
              {
                pixels.setPixelColor(i, pixels.Color(50, 50, 50));
              }
              pixels.show();
              pixels.show();

              motor(st, st, st, st, 500);
            }
            else if (gostart[i] == 6)
            {
              display.clearDisplay();              // Clear the display buffer
              display.setTextSize(2);              // Normal 1:1 pixel scale
              display.setTextColor(SSD1306_WHITE); // Draw white text
              display.setCursor(47, 25);           // Start at top-left corner
              display.println(F("Red"));
              display.display(); // Show the display buffer on the screen
              pixels.clear();
              for (byte i = 0; i < NUMPIXELS; i++)
              {
                pixels.setPixelColor(i, pixels.Color(50, 0, 0));
              }
              pixels.show();
              pixels.show();
              delay(1000);
            }
            else if (gostart[i] == 7)
            {
              display.clearDisplay();              // Clear the display buffer
              display.setTextSize(2);              // Normal 1:1 pixel scale
              display.setTextColor(SSD1306_WHITE); // Draw white text
              display.setCursor(35, 25);           // Start at top-left corner
              display.println(F("Green"));
              display.display(); // Show the display buffer on the screen
              pixels.clear();
              for (byte i = 0; i < NUMPIXELS; i++)
              {
                pixels.setPixelColor(i, pixels.Color(0, 50, 0));
              }
              pixels.show();
              pixels.show();
              delay(1000);
            }
            else if (gostart[i] == 8)
            {
              display.clearDisplay();              // Clear the display buffer
              display.setTextSize(2);              // Normal 1:1 pixel scale
              display.setTextColor(SSD1306_WHITE); // Draw white text
              display.setCursor(45, 25);           // Start at top-left corner
              display.println(F("Blue"));
              display.display(); // Show the display buffer on the screen
              pixels.clear();
              for (byte i = 0; i < NUMPIXELS; i++)
              {
                pixels.setPixelColor(i, pixels.Color(0, 0, 50));
              }
              pixels.show();
              pixels.show();
              delay(1000);
            }
            else if (gostart[i] == 9)
            {
              display.clearDisplay();              // Clear the display buffer
              display.setTextSize(2);              // Normal 1:1 pixel scale
              display.setTextColor(SSD1306_WHITE); // Draw white text
              display.setCursor(45, 25);           // Start at top-left corner
              display.println(F("Dark"));
              display.display(); // Show the display buffer on the screen
              pixels.clear();
              for (byte i = 0; i < NUMPIXELS; i++)
              {
                pixels.setPixelColor(i, pixels.Color(0, 0, 0));
              }
              pixels.show();
              pixels.show();
              delay(1000);
            }
          }
          count = 0;
          for (byte i = 0; i < 32; i++)
          {
            gostart[i] = 0;
          }
          pixels.clear();
          for (byte i = 0; i < NUMPIXELS; i++)
          {
            pixels.setPixelColor(i, pixels.Color(0, 50, 0));
          }
          pixels.show();
          pixels.show();
          display.clearDisplay(); // Clear the display buffer
          display.drawBitmap(0, 0, myBitmap06, 128, 64, SSD1306_WHITE);
          display.display(); // Show the display buffer on the screen
          for (int i = 0; i < 3; i++)
          {
            myTone(11, 1000, 100);
            delay(100);
          }
          Serial.println("done");
          delay(1000);
          display.clearDisplay();
          display.drawBitmap(0, 0, myBitmap00, 128, 64, SSD1306_WHITE);
          display.display(); // Show the display buffer on the screen
          display.display();
          continue;
        }
        if (count > 31)
        {
          for (int i = 0; i < 3; i++)
          {
            myTone(11, 1000, 100);
            delay(100);
          }
          continue;
        }
        //forward
        if (code_ == "66")
        {
          display.clearDisplay(); // Clear the display buffer
          display.drawBitmap(0, 0, myBitmap01, 128, 64, SSD1306_WHITE);
          display.display(); // Show the display buffer on the screen
          delay(300);
          gostart[count] = 1;
          count += 1;
        }
        //left
        else if (code_ == "6c")
        {
          display.clearDisplay(); // Clear the display buffer
          display.drawBitmap(0, 0, myBitmap02, 128, 64, SSD1306_WHITE);
          display.display(); // Show the display buffer on the screen
          delay(300);
          gostart[count] = 2;
          count += 1;
        }
        //right
        else if (code_ == "72")
        {
          display.clearDisplay(); // Clear the display buffer
          display.drawBitmap(0, 0, myBitmap03, 128, 64, SSD1306_WHITE);
          display.display(); // Show the display buffer on the screen
          delay(300);
          gostart[count] = 3;
          count += 1;
        }

        //backward
        else if (code_ == "61")
        {
          display.clearDisplay(); // Clear the display buffer
          display.drawBitmap(0, 0, myBitmap08, 128, 64, SSD1306_WHITE);
          display.display(); // Show the display buffer on the screen
          delay(300);
          gostart[count] = 4;
          count += 1;
        }

        //stop
        else if (code_ == "73")
        {
          display.clearDisplay();              // Clear the display buffer
          display.setTextSize(2);              // Normal 1:1 pixel scale
          display.setTextColor(SSD1306_WHITE); // Draw white text
          display.setCursor(37, 25);           // Start at top-left corner
          display.println(F("Stop!"));
          display.display(); // Show the display buffer on the screen
          delay(300);
          gostart[count] = 5;
          count += 1;
        }

        //red
        else if (code_ == "68")
        {
          display.clearDisplay();              // Clear the display buffer
          display.setTextSize(2);              // Normal 1:1 pixel scale
          display.setTextColor(SSD1306_WHITE); // Draw white text
          display.setCursor(47, 25);           // Start at top-left corner
          display.println(F("Red"));
          display.display(); // Show the display buffer on the screen
          delay(300);
          gostart[count] = 6;
          count += 1;
        }
        //green
        else if (code_ == "67")
        {
          display.clearDisplay();              // Clear the display buffer
          display.setTextSize(2);              // Normal 1:1 pixel scale
          display.setTextColor(SSD1306_WHITE); // Draw white text
          display.setCursor(35, 25);           // Start at top-left corner
          display.println(F("Green"));
          display.display(); // Show the display buffer on the screen
          delay(300);
          gostart[count] = 7;
          count += 1;
        }
        //blue
        else if (code_ == "62")
        {
          display.clearDisplay();              // Clear the display buffer
          display.setTextSize(2);              // Normal 1:1 pixel scale
          display.setTextColor(SSD1306_WHITE); // Draw white text
          display.setCursor(45, 25);           // Start at top-left corner
          display.println(F("Blue"));
          display.display(); // Show the display buffer on the screen
          delay(300);
          gostart[count] = 8;
          count += 1;
        }
        //black
        else if (code_ == "64")
        {
          display.clearDisplay();              // Clear the display buffer
          display.setTextSize(2);              // Normal 1:1 pixel scale
          display.setTextColor(SSD1306_WHITE); // Draw white text
          display.setCursor(45, 25);           // Start at top-left corner
          display.println(F("Dark"));
          display.display(); // Show the display buffer on the screen
          delay(300);
          gostart[count] = 9;
          count += 1;
        }

        //clean
        else if (code_ == "63")
        {
          display.clearDisplay(); // Clear the display buffer
          display.drawBitmap(0, 0, myBitmap04, 128, 64, SSD1306_WHITE);
          display.display(); // Show the display buffer on the screen
          delay(300);
          count -= 1;
          if (count < 0)
          {
            count = 0;
          }
          gostart[count] = 0;
        }
        showoled();
        for (byte i = 0; i < 32; i++)
        {
          Serial.print(gostart[i]);
        }
        Serial.println("");
      }
    }
  }
}
