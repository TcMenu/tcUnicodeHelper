/*
 * Copyright (c) 2018 https://www.thecoderscorner.com (Dave Cherry).
 * This product is licensed under an Apache license, see the LICENSE file in the top-level directory.
 */

#ifndef TCMENU_UTF8TEXTPROCESSOR_H
#define TCMENU_UTF8TEXTPROCESSOR_H

/* the value that defines an error in character conversion, usual procedure is to call reset() on the text processor */
#define TC_UNICODE_CHAR_ERROR 0xffffffff

// This defines controls if this is used in the context of multiplatform IoAbstraction (false) or straight Arduino (true)
// as this is a very useful class, we also provide it outside the regular context for other users.
#define USED_IN_ARDUINO_ONLY true

#if USED_IN_ARDUINO_ONLY == true
#include <Arduino.h>
#else
#include <PlatformDetermination.h>
#endif // USED_IN_ARDUINO_ONLY

namespace tccore {

    /**
     * Tells the UTF8 decoder which mode it is in, either extended ASCII or UTF8
     */
    enum UnicodeEncodingMode { ENCMODE_UTF8, ENCMODE_EXT_ASCII };

    /**
     * This callback will be called for every converted UTF8 or ASCII sequence (depending on the mode of the handler).
     * You'll get the void pointer you provided during construction, and the characeter that was converted.
     */
    typedef void (*UnicodeCharacterHandler)(void* callbackData, uint32_t convertedChar);

    /**
     * A completely asynchronous implementation of a UTF8 encoder. Can work with the Print interface as it expects no
     * end to the stream, and requires only a very simple change in the write function.
     */
    class Utf8TextProcessor {
    public:
        enum DecoderState {
            WAITING_BYTE_0, WAITING_BYTE_1, WAITING_BYTE_2, WAITING_BYTE_3, UTF_CHAR_FOUND
        };
    private:
        DecoderState decoderState = WAITING_BYTE_0;
        uint32_t currentUtfChar = 0U;
        int extraCharsNeeded = 0;
        UnicodeCharacterHandler handler;
        void* userData;
        const UnicodeEncodingMode encodingMode;
    public:
        explicit Utf8TextProcessor(UnicodeCharacterHandler handler, void* userData, UnicodeEncodingMode mode)
                : handler(handler), userData(userData), encodingMode(mode) {}
        void reset();

        void pushChar(char ch);
        void pushChars(const char *str);

    private:
        void error(char lastCode);
        bool couldSequenceBeSmaller() const;

        void processChar0(char data);
    };

}



#endif //TCMENU_UTF8TEXTPROCESSOR_H
