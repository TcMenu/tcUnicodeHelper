#include <Adafruit_GFX.h>
#include "tcUnicodeHelper.h"
#include <Adafruit_ILI9341.h>
#include <SPI.h>
#include <Wire.h>
#include <Fonts/FreeSans12pt7b.h>
#include "Fonts/OpenSansCyrillicLatin18.h"

#define TFT_CS   20  // Chip select control pin
#define TFT_DC   18  // Data Command control pin
#define TFT_RST  19  // Reset pin (could connect to Arduino RESET pin)

//
// Here we create an adafruit display, but the font handler works equally well with U8G2, and TFT_eSPI displays.
// It also works with TcMenu drawable interface and the designer can generate suitable themes.
//
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

//
// Create an object that can draw fonts onto the display, this shows adafruit but
// if using U8G2 or TFT_eSPI you'd simply pass the pointer to your display object instead.
//
UnicodeFontHandler fontHandler(&tft, ENCMODE_UTF8);

int yTextSize = 0;
int yOpenSansSize = 0;
const char helloText[] PROGMEM = "hello world";
const char helloUkraine[] PROGMEM = "Привіт Світ";

void setup() {
    // we use a non-standard SPI setup on our pico
    SPI.setSCK(2);
    SPI.setTX(3);
    SPI.setRX(4);

    // start up serial
    Serial.begin(115200);
    Serial.print(F("Graphics test"));

    // initialise the display
    tft.begin();
    tft.fillScreen(ILI9341_BLACK);

    //
    // Get the size of the Unicode open sans font that we are using, it is returned as a Coord object that has x and y.
    // the baseline, indicates the amount of space below the drawing point that's needed.
    //
    int baseLine = 0;
    fontHandler.setFont(OpenSansCyrillicLatin18);
    Coord openSansSize = fontHandler.textExtents(helloText, &baseLine);
    yOpenSansSize = (int)openSansSize.y - baseLine;

    //
    // Now get the size of the Adafruit_GFX font that we are using, same as above basically.
    //
    fontHandler.setFont(&FreeSans12pt7b);
    Coord textSize = fontHandler.textExtents(helloText, &baseLine);
    yTextSize = (int)textSize.y - baseLine;

    //
    // Now we print some text on the first available line, just as with adafruit, the font is drawn with reference to
    // the baseline, so we need to add the Y size of the font without the baseline. This is using a regular adafruit
    // graphics font.
    //
    fontHandler.setCursor(0, yTextSize);
    fontHandler.setFont(&FreeSans12pt7b);
    fontHandler.setDrawColor(ILI9341_WHITE);
    fontHandler.printf_P(helloText);

    //
    // Now we print some text just below that that is in unicode, this text is in cyrillic.
    //
    fontHandler.setCursor(0, 30 + yOpenSansSize);
    fontHandler.setFont(OpenSansCyrillicLatin18);
    fontHandler.print(helloUkraine);

    //
    // Now we print the X and Y sizes of the adafruit font
    //
    fontHandler.setCursor(0, yTextSize + 60);
    fontHandler.setFont(&FreeSans12pt7b);
    fontHandler.print((int)textSize.x);
    fontHandler.setCursor(80, yTextSize + 60);
    fontHandler.print((int)textSize.y);
}

void loop() {
    int whereY = 100;
    delay(250);
    tft.fillRect(0, whereY, tft.width(), 30, ILI9341_BLACK);

    tft.setCursor(0, yTextSize + whereY);
    fontHandler.setFont(&FreeSans12pt7b);
    fontHandler.setDrawColor(ILI9341_CYAN);
    fontHandler.print(millis());

    tft.setCursor(80, yOpenSansSize + whereY);
    fontHandler.setFont(OpenSansCyrillicLatin18);
    fontHandler.setDrawColor(ILI9341_GREEN);
    fontHandler.print((float)millis() / 1000.0F);
}