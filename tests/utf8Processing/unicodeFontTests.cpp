
#include <testing/SimpleTest.h>
#include <Fonts/OpenSansCyrillicLatin18.h>
#include <tcUnicodeHelper.h>
#include <SimpleCollections.h>
#include <SCCircularBuffer.h>


class UnitTestPlotter : public TextPlotPipeline {
private:
    GenericCircularBuffer<Coord> pixelsDrawn;
    uint32_t col = 0;
    Coord where = {0,0};
public:
    UnitTestPlotter(): pixelsDrawn(32) {}

    void drawPixel(uint16_t x, uint16_t y) override {
        pixelsDrawn.put(Coord(x, y));
    }

    void setColor(uint32_t color) override {
        col = color;
    }

    void setCursor(const Coord &p) override {
        where = p;
    }

    Coord getCursor() override {
        return where;
    }

    Coord getDimensions() override {
        return Coord(320, 200);
    }

    void init() {
        while(pixelsDrawn.available()) pixelsDrawn.get();
        where = {0,0};
    }
} unitTestPlotter;

test(testTextExtents) {
    unitTestPlotter.init();
    UnicodeFontHandler handler(&unitTestPlotter, ENCMODE_UTF8);

    int bl;
    handler.setFont(OpenSansCyrillicLatin18);
    handler.setDrawColor(20);
    Coord coord = handler.textExtents("Abc", &bl, false);
    assertEquals((int32_t)21, coord.x);
    assertEquals((int32_t)17, coord.y);
}

test(testGetGlyph) {
    unitTestPlotter.init();
    UnicodeFontHandler handler(&unitTestPlotter, ENCMODE_UTF8);
    handler.setFont(OpenSansCyrillicLatin18);
    handler.setDrawColor(20);

    GlyphWithBitmap glyphWithBitmap;
    assertTrue(handler.findCharInFont(65, glyphWithBitmap));
    assertTrue(glyphWithBitmap.getGlyph()->relativeChar == 65);
}