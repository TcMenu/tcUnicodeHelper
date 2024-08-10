
#include <Arduino.h>
#include <unity.h>
#include <deque>
#include <Fonts/OpenSansCyrillicLatin18.h>
#include <Fonts/RobotoMedium24.h>
#include <tcUnicodeHelper.h>

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

void checkGlyph(uint32_t code, uint32_t bmpOffset, int width, int height, int xAdvance, int xOffset, int yOffset) {
    printf("Checking glyph %d\n", code);
    GlyphWithBitmap glyphWithBitmap;
    if(handler->findCharInFont(code, glyphWithBitmap)) {
        const UnicodeFontGlyph *glyph = glyphWithBitmap.getGlyph();
        TEST_ASSERT_EQUAL(bmpOffset, glyph->relativeBmpOffset);
        TEST_ASSERT_EQUAL(width, glyph->width);
        TEST_ASSERT_EQUAL(height, glyph->height);
        TEST_ASSERT_EQUAL(xOffset, glyph->xOffset);
        TEST_ASSERT_EQUAL(yOffset, glyph->yOffset);
        TEST_ASSERT_EQUAL(xAdvance, glyph->xAdvance);
    } else {
        TEST_ABORT();
    }
}

void testTextExtents() {
    int bl;
    handler->setFont(OpenSansCyrillicLatin18);
    Coord coord = handler->textExtents("Abc", &bl, false);
    TEST_ASSERT_EQUAL_INT16(41, coord.x);
    TEST_ASSERT_EQUAL_INT16(28, coord.y);

    handler->setFont(RobotoMedium24);
    coord = handler->textExtents("Abc", &bl, false);
    TEST_ASSERT_EQUAL_INT16(45, coord.x);
    TEST_ASSERT_EQUAL_INT16(33, coord.y);
}

void testGetGlyphOnEachRange() {
    handler->setFont(OpenSansCyrillicLatin18);
    checkGlyph(65, 646, 16, 18, 16, 0, -18);
    checkGlyph(257, 42, 11, 17, 14, 1, -17);
    checkGlyph(1041, 561, 12, 18, 15, 2, -18);
}

void testReadingEveryGlyphInRange() {
    GlyphWithBitmap glyphWithBitmap;

    // test all known characters work
    for (int i = 32; i < 126; i++) {
        printf("Test character = %d\n", i);
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

void testAdafruitFont() {
    GlyphWithBitmap glyphWithBitmap;
    handler->setFont(RobotoMedium24);

    // test a few that should fail
    checkGlyph(32, 33, 1, 1, 8, 0, -1);
    checkGlyph(48, 485, 15, 24, 19, 2, -24);
    checkGlyph(65, 1190, 22, 24, 22, 0, -24);
    checkGlyph(126, 3820, 18, 7, 22, 2, -13);
    TEST_ASSERT_FALSE(handler->findCharInFont(5, glyphWithBitmap));
    TEST_ASSERT_FALSE(handler->findCharInFont(127, glyphWithBitmap));
}

#define RUN_TEST_WITH_PRINT(x) printf("test start " #x "\n"); RUN_TEST(x);

void setup() {
    UNITY_BEGIN();
    RUN_TEST_WITH_PRINT(testTextExtents);
    RUN_TEST_WITH_PRINT(testGetGlyphOnEachRange);
    RUN_TEST_WITH_PRINT(testReadingEveryGlyphInRange);
    RUN_TEST_WITH_PRINT(testAdafruitFont);
    UNITY_END();
}

void loop() {}