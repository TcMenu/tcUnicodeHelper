/*
 * Copyright (c) 2018 https://www.thecoderscorner.com (Dave Cherry).
 * This product is licensed under an Apache license, see the LICENSE file in the top-level directory.
 */

#ifndef TC_UNICODE_FONTHELPER_H
#define TC_UNICODE_FONTHELPER_H

#include <string.h>
#include <inttypes.h>
#include "Utf8TextProcessor.h"
#include "UnicodeFontDefs.h"

#define TCUNICODE_API_VERSION 2

#if !defined(pgm_read_dword) && (defined(__MBED__) || defined(BUILD_FOR_PICO_CMAKE))
#define pgm_read_byte(addr) (*(const unsigned char *)(addr))
#define pgm_read_word(addr) (*(const unsigned short *)(addr))
#define pgm_read_dword(addr) (*(const unsigned long *)(addr))
#define pgm_read_float(addr) (*(const float *)(addr))
#define pgm_read_ptr(addr) (*(addr))
#define memcpy_P memcpy
#endif // pgm_read_byte

/**
 * @file tcUnicodeHelper.h
 * @brief contains a Unicode handler that can process UTF-8 data and print it onto most display types.
 */

#if __has_include ("zio_local_definitions.h")
#    include "zio_local_definitions.h"
#endif

#ifndef TC_COORD_DEFINED
#define TC_COORD_DEFINED

namespace tcgfx {

/** A structure that holds both X and Y direction in a single 32 bit integer. Both x and y are public */
    struct Coord {
        /** default construction sets values to 0 */
        Coord() {
            x = y = 0;
        }

        /**
         * Create a coord based on an X and Y location
         * @param x the x location
         * @param y the y location
         */
        Coord(int x, int y) {
            this->x = x;
            this->y = y;
        }

        Coord(const Coord &other) = default;
        Coord& operator = (const Coord& other) = default;

        int16_t x;
        int16_t y;
    };
}

#endif // TC_COORD_DEFINED

using namespace tcgfx;

/**
 * A plot pipeline takes care of actually drawing the font glyphs in terms of pixels and cursor positions, it allows
 * for independent implementation on many different graphics libraries. There are ready made implementation for U8G2,
 * Adafruit_GFX, tcMenu Drawable and TFT_eSPI. Should you wish to create one for another display, follow one of the
 * example device includes such as `tcUnicodeU8G2.h`.
 */
class TextPlotPipeline {
public:
    virtual ~TextPlotPipeline() = default;
    /**
     * Draw a pixel onto the device at the given coordinates
     * @param x the x position increases left to right
     * @param y the y position increases top to bottom
     * @param color the color in whatever format the device uses
     */
    virtual void drawPixel(uint16_t x, uint16_t y, uint32_t color) = 0;
    /**
     * Set the position that the next text will be printed at, handling of offscreen is minimal, and just stops rendering
     * @param where the coordinate to draw at
     */
    virtual void setCursor(const Coord& where) = 0;
    /**
     * @return the current coordinates for print
     */
    virtual Coord getCursor() = 0;
    /**
     * @return the dimensions of the underlying display object
     */
    virtual Coord getDimensions() = 0;
};

#define TC_UNICODE_CHAR_ERROR 0xffffffff

/**
 * Represents an item that can be drawn using the TcMenu font drawing functions. Regardless of if it is Adafruit
 * or TcUnicode we wrap it in one of these so the drawing code is always the same.
 */
class GlyphWithBitmap {
private:
    const uint8_t *bitmapData = nullptr;
    const UnicodeFontGlyph *glyph = nullptr;
public:
    /**
     * @return the actual bitmap data with offset already applied
     */
    const uint8_t *getBitmapData() const {
        return bitmapData;
    }

    /**
     * @return the glyph instructions for rendering. This structure is always in RAM and not progmem.
     */
    const UnicodeFontGlyph *getGlyph() const {
        return glyph;
    }

    void setBitmapData(const uint8_t *bm) {
        GlyphWithBitmap::bitmapData = bm;
    }

    void setGlyph(const UnicodeFontGlyph *g) {
        GlyphWithBitmap::glyph = g;
    }
};

void handleUtf8Drawing(void *userData, uint32_t ch);

#if __has_include (<Print.h>) || defined(ARDUINO_SAM_DUE)
#include <Print.h>
class UnicodeFontHandler : public Print {
#elif __has_include(<PrintCompat.h>)
#include <PrintCompat.h>
class UnicodeFontHandler : public Print {
#else
class UnicodeFontHandler {
#endif

public:
    enum HandlerMode {
        HANDLER_SIZING_TEXT, HANDLER_DRAWING_TEXT
    };
private:
    tccore::Utf8TextProcessor utf8;
    TextPlotPipeline *plotter;
    union {
        const UnicodeFont *unicodeFont;
        const GFXfont *adaFont;
    };
    bool fontAdafruit = false;
    HandlerMode handlerMode = HANDLER_DRAWING_TEXT;
    uint16_t xExtentCurrent = 0;
    int16_t calculatedBaseline = -1;
    uint32_t drawColor = 0;
public:
    /**
     * Create a UnicodeFontHandler with a given pipeline, the pipeline interfaces with the underlying library and provides
     * the drawing support. It is the minimum possible code to draw text. It also takes an encoding mode which is passed
     * to the underlying UTF-8 encoder, either `ENCMODE_UTF8` or `ENCMODE_EXT_ASCII`. This object will not delete the
     * underlying plotter that you pass in, you must do that yourself if this object is not global in scope. See the examples
     * for more details on usage with libraries.
     *
     * @param plotter the pipeline plotter pointer
     * @param mode the encoding mode
     */
    explicit UnicodeFontHandler(TextPlotPipeline *plotter, tccore::UnicodeEncodingMode mode) : utf8(handleUtf8Drawing, this, mode),
                                                                                               plotter(plotter),
                                                                                               unicodeFont(nullptr) {}
    virtual ~UnicodeFontHandler() = default;

    /**
     * Plotter pipelines allow the rendering of fonts to be customized to a greater extent, for example a transformation
     * pipeline could be added as a kind of passthrough.
     * @return  the current plotter
     */
    TextPlotPipeline *getTextPlotPipeline() { return plotter; }

    /**
     * Set a new pipeline as the means of plotting onto the display
     * @param newPipeline the new pipeline
     */
    void setTextPlotPipeline(TextPlotPipeline *newPipeline) { plotter = newPipeline; }

    /**
    * Sets the font to be a TcUnicode font
    * @param font a tcUnicode font
    */
    void setFont(const UnicodeFont *font) {
        unicodeFont = font;
        fontAdafruit = false;
        calculatedBaseline = -1;
    }

    /**
    * sets the font to be an Adafruit font
    * @param font an adafruit font
    */
    void setFont(const GFXfont *font) {
        adaFont = font;
        fontAdafruit = true;
        calculatedBaseline = -1;
    }

    /**
     * Set the cursor position where the next text will be drawn, depending on the library this may also change the
     * cursor for the library.
     * @param x the x position
     * @param y the y position
     */
    void setCursor(int16_t x, int16_t y) { plotter->setCursor(Coord(x, y)); }

    /**
     * Set the cursor position where the next text will be drawn, depending on the library this may also change the
     * cursor for the library.
     * @param where the new position
     */
    void setCursor(const Coord& where) { plotter->setCursor(where); }

    /**
    * Set the drawing color
    * @param color the new color for text drawing
    */
    void setDrawColor(uint32_t color) { this->drawColor = color; }

    /**
    * Prints a unicode character using the current font
    * @param unicodeChar the character to print.
    */
    void writeUnicode(uint32_t unicodeText);

    /**
    * Get the extents of the text provided in UTF8, optionally the string can be in program memory by providing the
    * third parameter as true.
    * @param text the text to get the length of, in UTF8
    * @param baseline the pointer to int for the baseline (amount below text), can be nullptr.
    * @param progMem optional, defaults to false, set to true for progMem.
    * @return the x and y extent of the text
    */
    Coord textExtents(const char *text, int *baseline, bool progMem = false);

    /**
    * Get the extents of the text provided in UTF8 from program memory.
    * @param text the UTF8 text in program memory to get the length of
    * @param baseline the pointer to int for the baseline (amount below text), can be nullptr.
    * @return the x and y extent of the text
    */
    Coord textExtents_P(const char *text, int *baseline) { return textExtents(text, baseline, true); }

#if !defined(__MBED__) && !defined(BUILD_FOR_PICO_CMAKE)

    /**
    * Get the extents of the text provided in UTF8 as a flash based string using the F() macro
    * @param fh the string provided using _F()
    * @param baseline the pointer to int for the baseline (amount below text), can be nullptr.
    * @return the X and Y extent of the text.
    */
    Coord textExtents(const __FlashStringHelper *fh, int *baseline) {
        return textExtents((const char *) fh, baseline, true);
    }

#endif

    /**
    * Get the extent of a single unicode character
    * @param theChar the unicode character to get the extent of
    * @return the x and y extent of the character
    */
    Coord textExtent(uint32_t theChar);

    /**
    * An extension to the print interface that allows printing from program memory somewhat easier
    * @param textPgm the text in program memory
    * @return the number of chars written
    */
    size_t print_P(const char *textPgm);

    /**
    * Get the absolute baseline of the font, this is the total height including any parts considered below
    * the yheight.
    * @return the baseline
    */
    int getBaseline();

    /**
    * Can be used to implement the print interface, the UTF8 implementation for this library is completely
    * asynchronous and keeps internal state, so it will wait until enough characters arrive to actually print
    * something.
    * @param data a byte of data in utf8
    */
    size_t write(uint8_t data) override;

    /**
     * Finds a character in the current font, if the character exists it will return true, and the referenced value
     * type (GlyphWithBitmap) will be filled in. Note that the returned glyph is always accessible without progmem
     * functions on any board. Whereas the bitmap will be in constant memory, so could we require progmem functions.
     * @param ch the character to find.
     * @param glyphBitmap a reference to a structure holding the Glyph and bitmap pointer. Only valid when true returned
     * @return true if successful, otherwise false.
     */
    bool findCharInFont(uint32_t ch, GlyphWithBitmap &glyphBitmap) const;
    /**
     * @return the total Y advance to move down a line. Call get baseline to get the amount below the baseline.
     */
    int getYAdvance() const {
        if(adaFont == nullptr) return 0;
        return pgm_read_byte((fontAdafruit ? &adaFont->yAdvance : &unicodeFont->yAdvance));
    }

    /**
     * Internal function called by the utf8 async callback
     * @param ch the unicode char
     */
    void internalHandleUnicodeFont(uint32_t ch);
};

using namespace tccore;

#endif //TC_UNICODE_FONTHELPER_H
