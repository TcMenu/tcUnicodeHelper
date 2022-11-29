/*
 * Copyright (c) 2018 https://www.thecoderscorner.com (Dave Cherry).
 * This product is licensed under an Apache license, see the LICENSE file in the top-level directory.
 */

#ifndef TCMENU_UNICODE_FONT_DEFN
#define TCMENU_UNICODE_FONT_DEFN

#include <inttypes.h>

#ifndef _GFXFONT_H_
#define _GFXFONT_H_

//
// This section is copied from Adafruit_GFX, it provides support for using Adafruit GFX fonts with this library.
//
// Font structures for newer Adafruit_GFX (1.1 and later).
// Example fonts are included in 'Fonts' directory.
// To use a font in your Arduino sketch, #include the corresponding .h
// file and pass address of GFXfont struct to setFont().
//

typedef struct {
    uint16_t bitmapOffset; ///< Pointer into GFXfont->bitmap
    uint8_t width;         ///< Bitmap dimensions in pixels
    uint8_t height;        ///< Bitmap dimensions in pixels
    uint8_t xAdvance;      ///< Distance to advance cursor (x axis)
    int8_t xOffset;        ///< X dist from cursor pos to UL corner
    int8_t yOffset;        ///< Y dist from cursor pos to UL corner
} GFXglyph;

/// Data stored for FONT AS A WHOLE
typedef struct {
    uint8_t *bitmap;  ///< Glyph bitmaps, concatenated
    GFXglyph *glyph;  ///< Glyph array
    uint16_t first;   ///< ASCII extents (first char)
    uint16_t last;    ///< ASCII extents (last char)
    uint8_t yAdvance; ///< Newline distance (y axis)
} GFXfont;

#endif // GFXFont include

/**
 * Each glyph in the font is represented by this struct. This describes how to render each character.
 */
typedef struct {
    /** The character number in the array */
    uint32_t relativeChar: 15;
    /** offset into the bitmap array */
    uint32_t relativeBmpOffset:17;
    /** width of the bitmap in pixels */
    uint8_t width;
    /** height of the bitmap in pixels */
    uint8_t height;
    /** how far to advance in the x dimension */
    uint8_t xAdvance;
    /** the x offset from the UL corner */
    int8_t xOffset;
    /** the y offset from the UL corner, usually negative */
    int8_t yOffset;
} UnicodeFontGlyph;

/**
 * Each unicode block range has it's own characters, this allows us to be more efficient with memory overall.
 * The char nums in a glyph are actually calculated by startingId + charNum
 */
typedef struct {
    /** The starting point for this block, all glyph entries are an offset from this */
    uint32_t startingNum;
    /** the array of bitmaps for each letter */
    const uint8_t *bitmap;
    /** The glyphs within this block */
    const UnicodeFontGlyph *glyphs;
    /** The number of points in this block */
    uint16_t numberOfPoints;
} UnicodeFontBlock;

/**
 * This represents the whole font, and is passed to an TcUnicode renderer requiring a font. It links the full
 * rendering instructions including bitmaps in one place.
 */
typedef struct {
    /** the array of unicode glyphs */
    const UnicodeFontBlock *unicodeBlocks;
    /** the number of items */
    uint16_t numberOfBlocks;
    /** the height of each line */
    uint8_t yAdvance;
} UnicodeFont;

#endif // TCMENU_UNICODE_FONT_DEFN
