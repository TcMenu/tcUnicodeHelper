# Provides TcUnicode support to Adafruit_GFX, U8G2, TFT_eSPI and others.

## Summary

TcUnicode provides Unicode character support to a wide range of display libraries (Adafruit_GFX, U8G2, TFT_eSPI, and also TcMenu). There is no need to use special versions of the libraries to use TcUnicode, and there is even a UI for creating the fonts. 

Dave Cherry / TheCodersCorner.com make this framework available for you to use. It takes me significant effort to keep all my libraries current and working on a wide range of boards. Please consider making at least a one off donation via the sponsor button if you find it useful.

In any fork, please ensure all text up to here is left unaltered.

## Why this library and this format?

This format and library were originally devised as part of the tcMenu project specifically for mbed boards, I thought the format to be very useful, enough to make open to all. It provides a very extensible unicode format that works across many libraries and boards.

This library provides the full implementation of TcUnicode for use in many different environments that are listed below.

* For TcMenu Device Drawable - the designer supports this format natively.
* Most Adafruit_GFX libraries - see the packaged Adafruit example.
* U8G2 library - see the packaged U8G2 example.
* TFT_eSPI - although the library has good Unicode support, this is another option. See the packaged example.

## How does this support work?

Firstly, you can create fonts by generating from a desktop font file directly from "tcMenu Designer" UI on most desktop platforms, and then they are included into your project as a header file.

There is a custom dialog within "TcMenu Designer UI" where you can select the Unicode blocks, and even sub ranges within blocks that you want to support using the UI.

Please raise all questions in the main TcMenu repository discussions.
