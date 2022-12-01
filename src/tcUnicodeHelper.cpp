/*
 * Copyright (c) 2018 https://www.thecoderscorner.com (Dave Cherry).
 * This product is licensed under an Apache license, see the LICENSE file in the top-level directory.
 */

#include "tcUnicodeHelper.h"

Coord UnicodeFontHandler::textExtents(const char *text, int *baseline, bool progMem) {
    handlerMode = HANDLER_SIZING_TEXT;
    xExtentCurrent = 0;
    utf8.reset();
    if(progMem) {
        uint8_t c;
        while ((c = pgm_read_byte(text++))) utf8.pushChar((char)c);
    } else {
        utf8.pushChars(text);
    }
    handlerMode = HANDLER_DRAWING_TEXT;

    if(baseline) {
        *baseline = getBaseline();
    }
    return Coord((int)xExtentCurrent, getYAdvance());
}

#define internal_max(a, b)  ((a) < (b) ? (b) : (a));

void UnicodeFontHandler::writeUnicode(uint32_t unicodeText) {
    // make sure it's printable.
    auto dims = plotter->getDimensions();
    auto posn = plotter->getCursor();
    if (posn.x > (int32_t) dims.x) return;

    GlyphWithBitmap gb;
    if(!findCharInFont(unicodeText, gb)) return;
    uint8_t w = gb.getGlyph()->width, h = gb.getGlyph()->height;
    int8_t xo = gb.getGlyph()->xOffset, yo = gb.getGlyph()->yOffset;
    uint8_t xx, yy, bits = 0, bit = 0;

    uint16_t xDim = dims.x;
    uint16_t yDim = dims.y;
    uint16_t x = posn.x;
    uint16_t y = posn.y;

    plotter->setColor(drawColor);

    const uint8_t* bitmap = gb.getBitmapData();
    int bo = 0;
    for (yy = 0; yy < h; yy++) {
        auto locY = internal_max(0, y + yo + yy);
        bool yOK = (locY < yDim);
        for (xx = 0; xx < w; xx++) {
            if (!(bit++ & 7)) {
                bits = bitmap[bo++];
            }
            if (bits & 0x80) {
                int locX = internal_max(0, int(x +xo + xx));
                if (locX < xDim && yOK) {
                    plotter->drawPixel(locX, locY);
                }
            }
            bits <<= 1;
        }
    }
    plotter->setCursor(Coord(posn.x + gb.getGlyph()->xAdvance, posn.y));
}

Coord UnicodeFontHandler::textExtent(uint32_t theChar) {
    GlyphWithBitmap gb;
    if (!findCharInFont(theChar, gb)) {
        return Coord(0, getYAdvance());
    }
    return Coord(gb.getGlyph()->xAdvance, getYAdvance());
}

const UnicodeFontGlyph *findWithinGlyphs(const UnicodeFontBlock* block, uint32_t ch) {
    size_t start = 0;
    size_t end = block->numberOfPoints - 1;
    bool failed = false;
    while (!failed) {
        if(block->glyphs[start].relativeChar == ch) return &block->glyphs[start];
        if(block->glyphs[end].relativeChar == ch) return &block->glyphs[end];

        size_t middle = ((end - start) / 2) + start;
        uint32_t charNumMiddle = block->glyphs[middle].relativeChar;
        if (charNumMiddle == ch) return &block->glyphs[middle];
        if (ch < charNumMiddle) {
            end = middle;
        } else {
            start = middle;
        }
        if ((end - start) < 2) failed = true;
    }
    return nullptr;
}


UnicodeFontGlyph globalGlyphForFindChar;

bool UnicodeFontHandler::findCharInFont(uint32_t code, GlyphWithBitmap& glyphBitmap) const {
    if(adaFont == nullptr) return false; // no font, then no characters!

    if (fontAdafruit) {
        if (code < adaFont->first || code > adaFont->last) return false;
        uint32_t idx = code - adaFont->first;
        glyphBitmap.setBitmapData(&adaFont->bitmap[adaFont->glyph[idx].bitmapOffset]);
        globalGlyphForFindChar.relativeChar = code;
        globalGlyphForFindChar.height = adaFont->glyph[idx].height;
        globalGlyphForFindChar.width = adaFont->glyph[idx].width;
        globalGlyphForFindChar.xAdvance = adaFont->glyph[idx].xAdvance;
        globalGlyphForFindChar.xOffset = adaFont->glyph[idx].xOffset;
        globalGlyphForFindChar.yOffset = adaFont->glyph[idx].yOffset;
        globalGlyphForFindChar.relativeBmpOffset = 0; // unused
        glyphBitmap.setGlyph(&globalGlyphForFindChar);
        return true;
    } else {
        for (uint16_t i = 0; i < unicodeFont->numberOfBlocks; i++) {
            uint32_t startingNum = unicodeFont->unicodeBlocks[i].startingNum;
            uint32_t endingNum = startingNum + unicodeFont->unicodeBlocks[i].numberOfPoints;
            if (code >= startingNum && code <= endingNum) {
                const UnicodeFontGlyph *glyph = findWithinGlyphs(&unicodeFont->unicodeBlocks[i], code - startingNum);
                if (glyph != nullptr) {
                    glyphBitmap.setGlyph(glyph);
                    glyphBitmap.setBitmapData(&unicodeFont->unicodeBlocks[i].bitmap[glyph->relativeBmpOffset]);
                    return true;
                }
            }
        }
    }

    return false;
}

void UnicodeFontHandler::internalHandleUnicodeFont(uint32_t ch) {
    if (ch == TC_UNICODE_CHAR_ERROR) {
        utf8.reset();
        return;
    }

    switch(handlerMode) {
        case HANDLER_SIZING_TEXT:
            xExtentCurrent += textExtent(ch).x;
            break;
        case HANDLER_DRAWING_TEXT:
            writeUnicode(ch);
            break;
    }
}

size_t UnicodeFontHandler::write(uint8_t data) {
    handlerMode = HANDLER_DRAWING_TEXT;
    utf8.pushChar((char)data);
    return 1;
}

size_t UnicodeFontHandler::print_P(const char *textPgm) {
    uint8_t c;
    size_t count= 0;
    while ((c = pgm_read_byte(textPgm++))) {
        utf8.pushChar((char)c);
        count++;
    }
    return count;
}

int UnicodeFontHandler::getBaseline() {
    auto current = "(|jy";
    uint16_t height = 0;
    int bl = 0;
    while (*current) {
        GlyphWithBitmap gb;
        if(findCharInFont(*current, gb)) {
            if (gb.getGlyph()->height > height) height = gb.getGlyph()->height;
            bl = gb.getGlyph()->height + gb.getGlyph()->yOffset;
        }
        current++;
    }
    return bl;
}

void handleUtf8Drawing(void *data, uint32_t ch) {
    auto fontHandler = reinterpret_cast<UnicodeFontHandler *>(data);
    fontHandler->internalHandleUnicodeFont(ch);
}
