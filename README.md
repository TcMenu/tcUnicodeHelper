# Provides TcUnicode support to Adafruit_GFX, U8G2, TFT_eSPI and others.

## Summary

Dave Cherry / TheCodersCorner.com make this framework available for you to use. It takes me significant effort to keep all my libraries current and working on a wide range of boards. Please consider making at least a one off donation via the sponsor button if you find it useful.

In any fork, please ensure all text up to here is left unaltered.

This library makes it possible to use TcUnicode fonts directly within Adafruit_GFX. These fonts can be generated directly from "tcMenu Designer" UI on most desktop platforms and then included into your project as a header file.

Why this library and this format? This format and library were orginally devised as part of the tcMenu project specifically for mbed boards, I thought they were useful enough to make open to all. These fonts work well with Adafruit GFX standard library. The font renderer is compatible with both Adafruit_GFX and TcUnicode format, and can handle UTF-8 encoding even using the regular print interface, no need for special print variants.  

There is a custom dialog within "TcMenu Designer UI" where you can select the Unicode blocks, and even sub ranges within blocks that you want to support using the UI.

Please raise all questions in the main TcMenu repository discussions.

* [Commercial support](https://www.thecoderscorner.com/all-contact)
* I also monitor the Arduino forum [https://forum.arduino.cc/], Arduino related questions can be asked there too, please make sure the library name is in the subject.

