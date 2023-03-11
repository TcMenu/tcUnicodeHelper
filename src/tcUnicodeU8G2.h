/*
 * Copyright (c) 2018 https://www.thecoderscorner.com (Dave Cherry).
 * This product is licensed under an Apache license, see the LICENSE file in the top-level directory.
 */

/**
 * @file tcUnicodeU8G2.h
 * @brief Adds tcUnicode support to the U8G2 library, only include when U8G2 is on your library path. To create a
 *        U8G2 pipeline instance simply call `newU8G2TextPipeline()`
 */

#ifndef TCMENU_UNICODE_U8G2_H
#define TCMENU_UNICODE_U8G2_H

#include "tcUnicodeHelper.h"
#include <U8g2lib.h>

namespace tcgfx {

#define UNICODE_U8G2_AVAILABLE

    class U8g2TextPlotPipeline : public TextPlotPipeline {
    private:
        U8G2 *u8g2;
        Coord cursor;
    public:
        explicit U8g2TextPlotPipeline(U8G2 *gfx) : u8g2(gfx) {}

        ~U8g2TextPlotPipeline() = default;

        void drawPixel(uint16_t x, uint16_t y, uint32_t color) override {
            u8g2->setColorIndex(color);
            u8g2->drawPixel(x, y);
        }

        Coord getDimensions() override { return Coord(u8g2->getWidth(), u8g2->getHeight()); }

        void setCursor(const Coord &where) override { cursor = where; }

        Coord getCursor() override { return cursor; }
    };

    /**
     * Create a plotter pipeline that draws onto U8G2 library. Simply provide this as the first parameter to the
     * creation of a TcUnicodeHelper.
     * @param gfx the graphics pointer
     * @return a new text pipeline for U8G2
     */
    inline U8g2TextPlotPipeline* newU8G2TextPipeline(U8G2 *graphics) {
        return new U8g2TextPlotPipeline(graphics);
    }
}
#endif //TCMENU_UNICODE_U8G2_H
