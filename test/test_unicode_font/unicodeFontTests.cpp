
#include <Arduino.h>
#include <unity.h>
#include <deque>
#include <Fonts/OpenSansCyrillicLatin18.h>
#include <Fonts/RobotoMedium24.h>
#include <tcUnicodeHelper.h>
#include <IoLogging.h>

class UnitTestPlotter : public TextPlotPipeline {
private:
    std::deque<Coord> pixelsDrawn;
    static const size_t max_size = 32;
    uint32_t col = 0;
    Coord where = {0,0};
public:
    UnitTestPlotter() = default;
    ~UnitTestPlotter() = default;

    void drawPixel(uint16_t x, uint16_t y, uint32_t color) override {
        if (pixelsDrawn.size() == max_size) {
            pixelsDrawn.pop_front();
        }
        pixelsDrawn.push_back(Coord(x, y));
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
        pixelsDrawn.clear();
        where = {0,0};
    }
} unitTestPlotter;

UnicodeFontHandler* handler = nullptr;

void setUp() {
    unitTestPlotter.init();
    handler = new UnicodeFontHandler(&unitTestPlotter, ENCMODE_UTF8);
    handler->setFont(OpenSansCyrillicLatin18);
    handler->setDrawColor(20);
}

void tearDown() {
    delete handler;
}

bool checkGlyph(uint32_t code, uint32_t bmpOffset, int width, int height, int xAdvance, int xOffset, int yOffset) {
    GlyphWithBitmap glyphWithBitmap;
    if(handler->findCharInFont(code, glyphWithBitmap)) {
        const UnicodeFontGlyph *glyph = glyphWithBitmap.getGlyph();
        bool success = true;
        if(bmpOffset != glyph->relativeBmpOffset) {
            serlogF4(SER_DEBUG, "Bmp offset out ", code, bmpOffset, glyph->relativeBmpOffset);
            success = false;
        }
        if(width != glyph->width || height != glyph->height) {
            serlogF4(SER_DEBUG, "Bmp width out ", code, width, glyph->width);
            serlogF3(SER_DEBUG, "Bmp height ", height, glyph->height);
            success = false;
        }
        if(xOffset != glyph->xOffset || yOffset != glyph->yOffset) {
            serlogF4(SER_DEBUG, "Bmp xoffs out ", code, xOffset, glyph->xOffset);
            serlogF3(SER_DEBUG, "Bmp yoffs ", yOffset, glyph->yOffset);
            success = false;
        }
        if(xAdvance != glyph->xAdvance) {
            serlogF4(SER_DEBUG, "Bmp xadv out ", code, xAdvance, glyph->xAdvance);
            success = false;
        }

        return success;
    } else {
        serlogF2(SER_DEBUG, "Glyph not found ", code);
        return false;
    }
}

void test_TextExtents() {
    int bl;
    handler->setFont(OpenSansCyrillicLatin18);
    Coord coord = handler->textExtents("Abc", &bl, false);
    TEST_ASSERT_EQUAL_INT16(31, coord.x);
    TEST_ASSERT_EQUAL_INT16(24, coord.y);

    handler->setFont(RobotoMedium24pt);
    coord = handler->textExtents("Abc", &bl, false);
    TEST_ASSERT_EQUAL_INT16(43, coord.x);
    TEST_ASSERT_EQUAL_INT16(28, coord.y);
}

void test_GetGlyphOnEachRange() {
    TEST_ASSERT_TRUE(checkGlyph(65, 320, 11, 13, 11, 0, -18));
    TEST_ASSERT_TRUE(checkGlyph(55 + 128, 198, 2, 2, 5, 1, -12));
    TEST_ASSERT_TRUE(checkGlyph(17 + 1024, 285, 8, 13, 11, 2, -18));
}

void test_ReadingEveryGlyphInRange() {
    GlyphWithBitmap glyphWithBitmap;

    // test all known characters work
    for (int i = 32; i < 127; i++) {
        serlogF2(SER_DEBUG, "Test character = ", i);
        TEST_ASSERT_TRUE(handler->findCharInFont(i, glyphWithBitmap));
        TEST_ASSERT_NOT_NULL(glyphWithBitmap.getGlyph());
        TEST_ASSERT_NOT_NULL(glyphWithBitmap.getBitmapData());
    }

    // test a few that should fail
    TEST_ASSERT_FALSE(handler->findCharInFont(0, glyphWithBitmap));
    TEST_ASSERT_FALSE(handler->findCharInFont(5, glyphWithBitmap));
    TEST_ASSERT_FALSE(handler->findCharInFont(10, glyphWithBitmap));
    TEST_ASSERT_FALSE(handler->findCharInFont(0xFFFF, glyphWithBitmap));
}

void test_AdafruitFont() {
    GlyphWithBitmap glyphWithBitmap;
    handler->setFont(RobotoMedium24pt);

    // test a few that should fail
    TEST_ASSERT_TRUE(checkGlyph(32, 0, 0, 0, 6, 0, -28));
    TEST_ASSERT_TRUE(checkGlyph(48, 243, 12, 17, 14, 1, -23));
    TEST_ASSERT_TRUE(checkGlyph(65, 611, 16, 17, 16, 0, -23));
    TEST_ASSERT_TRUE(checkGlyph(126, 1990, 14, 5, 16, 1, -16));
    TEST_ASSERT_FALSE(handler->findCharInFont(5, glyphWithBitmap));
    TEST_ASSERT_FALSE(handler->findCharInFont(127, glyphWithBitmap));
    TEST_ASSERT_FALSE(handler->findCharInFont(31, glyphWithBitmap));
}

void setup() {
    UNITY_BEGIN();
    RUN_TEST(test_TextExtents);
    RUN_TEST(test_GetGlyphOnEachRange);
    RUN_TEST(test_ReadingEveryGlyphInRange);
    RUN_TEST(test_AdafruitFont);
    UNITY_END();
}

void loop() {}