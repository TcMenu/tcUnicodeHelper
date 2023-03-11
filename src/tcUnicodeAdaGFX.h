/*
 * Copyright (c) 2018 https://www.thecoderscorner.com (Dave Cherry).
 * This product is licensed under an Apache license, see the LICENSE file in the top-level directory.
 */

/**
 * @file tcUnicodeAdaGFX.h
 * @brief Adds tcUnicode support to the Adafruit_GFX library, only include when Adafruit_GFX is on your library path.
 *        To create a pipeline instance simply call `newAdafruitTextPipeline()`
 */

#ifndef TCMENU_UNICODE_ADAGFX_H
#define TCMENU_UNICODE_ADAGFX_H

#include "tcUnicodeHelper.h"
#include <Adafruit_GFX.h>

namespace tcgfx {
    class AdafruitTextPlotPipeline : public TextPlotPipeline {
    private:
        Adafruit_GFX *gfx;
    public:
        explicit AdafruitTextPlotPipeline(Adafruit_GFX *gfx) : gfx(gfx) {
        }

        ~AdafruitTextPlotPipeline() = default;

        void drawPixel(uint16_t x, uint16_t y, uint32_t dc) override { return gfx->drawPixel(x, y, dc); }

        void setCursor(const Coord &where) override { gfx->setCursor(where.x, where.y); }

        Coord getCursor() override { return Coord(gfx->getCursorX(), gfx->getCursorY()); }

        Coord getDimensions() override { return Coord(gfx->width(), gfx->height()); }
    };

    /**
     * Create a plotter pipeline that draws onto adafruit library. Simply provide this as the first parameter to the
     * creation of a TcUnicodeHelper.
     * @param gfx the adafruit graphics pointer
     * @return a new text pipeline for adafruit
     */
    inline AdafruitTextPlotPipeline *newAdafruitTextPipeline(Adafruit_GFX *gfx) {
        return new AdafruitTextPlotPipeline(gfx);
    }
}

#endif //TCMENU_UNICODE_ADAGFX_H
