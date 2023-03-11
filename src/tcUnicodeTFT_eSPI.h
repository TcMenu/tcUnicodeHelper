/*
 * Copyright (c) 2018 https://www.thecoderscorner.com (Dave Cherry).
 * This product is licensed under an Apache license, see the LICENSE file in the top-level directory.
 */

/**
 * @file tcUnicodeAdaGFX.h
 * @brief Adds tcUnicode support to the Adafruit_GFX library, only include when Adafruit_GFX is on your library path.
 *        To create a pipeline instance simply call `newTFT_eSPITextPipeline()`
 */

#ifndef TCMENU_UNICODE_ADAGFX_H
#define TCMENU_UNICODE_ADAGFX_H

#include "tcUnicodeHelper.h"
#include <TFT_eSPI.h>

namespace tcgfx {

    class TftSpiTextPlotPipeline : public TextPlotPipeline {
    private:
        TFT_eSPI* tft;
        Coord cursor;
    public:
        TftSpiTextPlotPipeline(TFT_eSPI* tft) : tft(tft) {}
        ~TftSpiTextPlotPipeline()=default;
        void drawPixel(uint16_t x, uint16_t y, uint32_t dc) override { return tft->drawPixel(x, y, dc); }
        Coord getDimensions() override { return Coord(tft->width(), tft->height());}
        void setCursor(const Coord& where) override { cursor = where; }
        Coord getCursor() override { return cursor; }
    };

    /**
     * Create a plotter pipeline that draws onto TFT_eSPI library. Simply provide this as the first parameter to the
     * creation of a TcUnicodeHelper.
     * @param gfx the graphics pointer
     * @return a new text pipeline for TFT_eSPI
     */
    inline TftSpiTextPlotPipeline *newTFT_eSPITextPipeline(TFT_eSPI *gfx) {
        return new TftSpiTextPlotPipeline(gfx);
    }
}

#endif //TCMENU_UNICODE_ADAGFX_H
