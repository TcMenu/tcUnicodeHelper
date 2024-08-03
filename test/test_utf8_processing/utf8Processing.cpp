// with thanks to https://www.cl.cam.ac.uk/~mgk25/ucs/examples/UTF-8-test.txt for many of the ideas in this test.

#include <Arduino.h>
#include <unity.h>
#include <deque>
#include <Utf8TextProcessor.h>

std::deque<uint32_t> unicodeChars;
const size_t max_size = 10;

void setUp() {
    unicodeChars.clear();
}

void tearDown() {}

void textHandler(void* handler, uint32_t charCode) {
    if (unicodeChars.size() == max_size) {
        unicodeChars.pop_front();
    }
    unicodeChars.push_back(charCode);
}

uint32_t getFromBufferOrError() {
    if (!unicodeChars.empty()) {
        uint32_t charCode = unicodeChars.front();
        unicodeChars.pop_front();
        return charCode;
    } else {
        return TC_UNICODE_CHAR_ERROR;
    }
}

void testUtf8EncoderAscii() {
    tccore::Utf8TextProcessor textProcessor(textHandler, nullptr, tccore::ENCMODE_EXT_ASCII);
    textProcessor.pushChars("Hello");
    // force extended ascii processing!
    textProcessor.pushChar((char)199);
    textProcessor.pushChar((char)200);

    TEST_ASSERT_EQUAL_UINT32(72, getFromBufferOrError());
    TEST_ASSERT_EQUAL_UINT32(101, getFromBufferOrError());
    TEST_ASSERT_EQUAL_UINT32(108, getFromBufferOrError());
    TEST_ASSERT_EQUAL_UINT32(108, getFromBufferOrError());
    TEST_ASSERT_EQUAL_UINT32(111, getFromBufferOrError());
    TEST_ASSERT_EQUAL_UINT32(199U, getFromBufferOrError());
    TEST_ASSERT_EQUAL_UINT32(200U, getFromBufferOrError());
    TEST_ASSERT_FALSE(!unicodeChars.empty());
}

void testUtf8EncoderUnicodeCodesDirect() {
    tccore::Utf8TextProcessor textProcessor(textHandler, nullptr, tccore::ENCMODE_UTF8);

    textProcessor.pushChar((char)0b00100100);
    TEST_ASSERT_EQUAL_UINT32(0x24, getFromBufferOrError());
    TEST_ASSERT_FALSE(!unicodeChars.empty());

    textProcessor.pushChar((char)0b11000010);
    textProcessor.pushChar((char)0b10100011);
    TEST_ASSERT_EQUAL_UINT32(0xA3, getFromBufferOrError());
    TEST_ASSERT_FALSE(!unicodeChars.empty());

    textProcessor.pushChar((char)0b11100010);
    textProcessor.pushChar((char)0b10000010);
    textProcessor.pushChar((char)0b10101100);
    TEST_ASSERT_EQUAL_UINT32(0x20AC, getFromBufferOrError());
    TEST_ASSERT_FALSE(!unicodeChars.empty());
}

void testUtf8EncoderUnicodeBasicCase() {
    tccore::Utf8TextProcessor textProcessor(textHandler, nullptr, tccore::ENCMODE_UTF8);
    textProcessor.pushChars("Hello");

    TEST_ASSERT_EQUAL_UINT32(72, getFromBufferOrError());
    TEST_ASSERT_EQUAL_UINT32(101, getFromBufferOrError());
    TEST_ASSERT_EQUAL_UINT32(108, getFromBufferOrError());
    TEST_ASSERT_EQUAL_UINT32(108, getFromBufferOrError());
    TEST_ASSERT_EQUAL_UINT32(111, getFromBufferOrError());
    TEST_ASSERT_FALSE(!unicodeChars.empty());
}

void testUtf8EncoderUnicodeMulti() {
    tccore::Utf8TextProcessor textProcessor(textHandler, nullptr, tccore::ENCMODE_UTF8);
    textProcessor.pushChars("Світ"); // \xD0\xA1\xD0\xB2\xD1\x96\xD1\x82

    TEST_ASSERT_EQUAL_UINT32(0x0421, getFromBufferOrError());
    TEST_ASSERT_EQUAL_UINT32(0x0432, getFromBufferOrError());
    TEST_ASSERT_EQUAL_UINT32(0x0456, getFromBufferOrError());
    TEST_ASSERT_EQUAL_UINT32(0x0442, getFromBufferOrError());
    TEST_ASSERT_FALSE(!unicodeChars.empty());
}

void testUtf8EncoderUnicodeOverlongNull() {
    tccore::Utf8TextProcessor textProcessor(textHandler, nullptr, tccore::ENCMODE_UTF8);
    textProcessor.pushChar((char)0xc0);
    textProcessor.pushChar((char)0x80);
    TEST_ASSERT_EQUAL_UINT32(TC_UNICODE_CHAR_ERROR, getFromBufferOrError());
    TEST_ASSERT_FALSE(!unicodeChars.empty());

    textProcessor.reset();
    textProcessor.pushChar((char)0xe0);
    textProcessor.pushChar((char)0x80);
    textProcessor.pushChar((char)0x80);
    TEST_ASSERT_EQUAL_UINT32(TC_UNICODE_CHAR_ERROR, getFromBufferOrError());
    TEST_ASSERT_FALSE(!unicodeChars.empty());

    textProcessor.reset();
    textProcessor.pushChar((char)0xf0);
    textProcessor.pushChar((char)0x80);
    textProcessor.pushChar((char)0x80);
    textProcessor.pushChar((char)0x80);
    TEST_ASSERT_EQUAL_UINT32(TC_UNICODE_CHAR_ERROR, getFromBufferOrError());
    TEST_ASSERT_FALSE(!unicodeChars.empty());
}

void testBrokenSequenceContinueAsAscii() {
    tccore::Utf8TextProcessor textProcessor(textHandler, nullptr, tccore::ENCMODE_UTF8);
    textProcessor.pushChar((char)0xc3);
    textProcessor.pushChar((char)'H');
    textProcessor.pushChar((char)'I');
    TEST_ASSERT_EQUAL_UINT32(TC_UNICODE_CHAR_ERROR, getFromBufferOrError());
    TEST_ASSERT_EQUAL_UINT32('H', getFromBufferOrError());
    TEST_ASSERT_EQUAL_UINT32('I', getFromBufferOrError());
    TEST_ASSERT_FALSE(!unicodeChars.empty());

    textProcessor.reset();
    textProcessor.pushChar((char)0xe1);
    textProcessor.pushChar((char)0x84);
    textProcessor.pushChar((char)'H');
    textProcessor.pushChar((char)'I');
    TEST_ASSERT_EQUAL_UINT32(TC_UNICODE_CHAR_ERROR, getFromBufferOrError());
    TEST_ASSERT_EQUAL_UINT32('H', getFromBufferOrError());
    TEST_ASSERT_EQUAL_UINT32('I', getFromBufferOrError());
    TEST_ASSERT_FALSE(!unicodeChars.empty());

    textProcessor.reset();
    textProcessor.pushChar((char)0xf1);
    textProcessor.pushChar((char)0x82);
    textProcessor.pushChar((char)0x81);
    textProcessor.pushChar((char)'A');
    TEST_ASSERT_EQUAL_UINT32(TC_UNICODE_CHAR_ERROR, getFromBufferOrError());
    TEST_ASSERT_EQUAL_UINT32('A', getFromBufferOrError());
    TEST_ASSERT_FALSE(!unicodeChars.empty());

    textProcessor.reset();
    textProcessor.pushChar((char)0xf1);
    textProcessor.pushChars("С"); // Cyrillic letter
    TEST_ASSERT_EQUAL_UINT32(TC_UNICODE_CHAR_ERROR, getFromBufferOrError());
    TEST_ASSERT_EQUAL_UINT32(0x0421, getFromBufferOrError());

    textProcessor.reset();
    textProcessor.pushChar((char)0xf1);
    textProcessor.pushChar((char)0x81);
    textProcessor.pushChars("С"); // Cyrillic letter
    TEST_ASSERT_EQUAL_UINT32(TC_UNICODE_CHAR_ERROR, getFromBufferOrError());
    TEST_ASSERT_EQUAL_UINT32(0x0421, getFromBufferOrError());
}

void testinvalidBytesNotProcessed() {
    tccore::Utf8TextProcessor textProcessor(textHandler, nullptr, tccore::ENCMODE_UTF8);
    textProcessor.pushChar((char)0xff);
    TEST_ASSERT_EQUAL_UINT32(TC_UNICODE_CHAR_ERROR, getFromBufferOrError());
    TEST_ASSERT_FALSE(!unicodeChars.empty());

    textProcessor.reset();
    textProcessor.pushChar((char)0xfe);
    TEST_ASSERT_EQUAL_UINT32(TC_UNICODE_CHAR_ERROR, getFromBufferOrError());
    TEST_ASSERT_FALSE(!unicodeChars.empty());
}

void testUtf8EncoderUnicodeOverlongSlash() {
    tccore::Utf8TextProcessor textProcessor(textHandler, nullptr, tccore::ENCMODE_UTF8);
    textProcessor.pushChar((char)0xc0);
    textProcessor.pushChar((char)0xAF);
    TEST_ASSERT_EQUAL_UINT32(TC_UNICODE_CHAR_ERROR, getFromBufferOrError());
    TEST_ASSERT_FALSE(!unicodeChars.empty());

    textProcessor.reset();
    textProcessor.pushChar((char)0xe0);
    textProcessor.pushChar((char)0x80);
    textProcessor.pushChar((char)0xAF);
    TEST_ASSERT_EQUAL_UINT32(TC_UNICODE_CHAR_ERROR, getFromBufferOrError());
    TEST_ASSERT_FALSE(!unicodeChars.empty());

    textProcessor.reset();
    textProcessor.pushChar((char)0xf0);
    textProcessor.pushChar((char)0x80);
    textProcessor.pushChar((char)0x80);
    textProcessor.pushChar((char)0xAF);
    TEST_ASSERT_EQUAL_UINT32(TC_UNICODE_CHAR_ERROR, getFromBufferOrError());
    TEST_ASSERT_FALSE(!unicodeChars.empty());
}

void testUtf8EncoderReallyLargeCodes() {
    tccore::Utf8TextProcessor textProcessor(textHandler, nullptr, tccore::ENCMODE_UTF8);
    textProcessor.pushChars("ﬓﬔﬕﬖ");
    // force extended ascii processing!

    TEST_ASSERT_EQUAL_UINT32(0xFB13, getFromBufferOrError());
    TEST_ASSERT_EQUAL_UINT32(0xFB14, getFromBufferOrError());
    TEST_ASSERT_EQUAL_UINT32(0xFB15, getFromBufferOrError());
    TEST_ASSERT_EQUAL_UINT32(0xFB16, getFromBufferOrError());
    TEST_ASSERT_FALSE(!unicodeChars.empty());
}

void setup() {
    UNITY_BEGIN();
    RUN_TEST(testUtf8EncoderAscii);
    RUN_TEST(testUtf8EncoderUnicodeCodesDirect);
    RUN_TEST(testUtf8EncoderUnicodeBasicCase);
    RUN_TEST(testUtf8EncoderUnicodeMulti);
    RUN_TEST(testUtf8EncoderUnicodeOverlongNull);
    RUN_TEST(testBrokenSequenceContinueAsAscii);
    RUN_TEST(testinvalidBytesNotProcessed);
    RUN_TEST(testUtf8EncoderUnicodeOverlongSlash);
    RUN_TEST(testUtf8EncoderReallyLargeCodes);
    UNITY_END();
}

void loop() {}