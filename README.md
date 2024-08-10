# TcUnicode support for Adafruit_GFX, U8G2, TFT_eSPI, tcMenu.
[![Build](https://github.com/TcMenu/tcUnicodeHelper/actions/workflows/build.yml/badge.svg)](https://github.com/TcMenu/tcUnicodeHelper/actions/workflows/build.yml)
[![Test](https://github.com/TcMenu/tcUnicodeHelper/actions/workflows/test.yml/badge.svg)](https://github.com/TcMenu/tcUnicodeHelper/actions/workflows/test.yml)
[![License: Apache 2.0](https://img.shields.io/badge/license-Apache--2.0-green.svg)](https://github.com/TcMenu/tcUnicodeHelper/blob/main/LICENSE)
[![GitHub release](https://img.shields.io/github/release/TcMenu/tcUnicodeHelper.svg?maxAge=3600)](https://github.com/TcMenu/tcUnicodeHelper/releases)
[![davetcc](https://img.shields.io/badge/davetcc-dev-blue.svg)](https://github.com/davetcc)
[![JSC TechMinds](https://img.shields.io/badge/JSC-TechMinds-green.svg)](https://www.jsctm.cz)

## Summary

TcUnicode provides Unicode character support to a wide range of display libraries (Adafruit_GFX, U8G2, TFT_eSPI, and also TcMenu). There is no need to use special versions of the libraries to use TcUnicode, and there is even a UI for creating the fonts. 

TcMenu organisation made this framework available for you to use. It takes significant effort to keep all our libraries current and working on a wide range of boards. Please consider making at least a one off donation via the sponsor buttons if you find it useful.

<a href="https://www.buymeacoffee.com/davetcc" target="_blank"><img src="https://cdn.buymeacoffee.com/buttons/v2/default-blue.png" alt="Buy Me A Coffee" style="height: 60px !important;width: 217px !important;" ></a>

In any fork, please ensure all text up to here is left unaltered.

## What is this library and this format?

This library allows you to use Adafruit and TcUnicode fonts with a wide variety of different drawing libraries, without needing any alteration to that library. Importantly, this means you can use Unicode with any Adafruit GFX based library, TcMenu Drawable, U8G2 and TFT_eSPI   

This format and library has now been around for a while and has been battle tested in TcMenu. It provides a very extensible unicode format that works across many libraries and boards. Currently supported on:

* TcMenu Device Drawable - the designer supports this format natively.
* Any Adafruit_GFX compatible library - see the packaged Adafruit example.
* U8G2 library - although the library has good Unicode support, this is another option. See the packaged example.
* TFT_eSPI - although the library has good Unicode support, this is another option. See the packaged example.

## Includes a UTF-8 decoder for mbed and Arduino

This package includes a strict asynchronous UTF8 decoder that needs very little memory to operate with, it is backed by a series of tests that ensure it is quite strict. Given the way it operates it is possible to use it with the Print interface as it can push one char at a time. Safely bailing out if a stream is interrupted midway.

## Creating your own tcUnicode or Adafruit GFX fonts

[TcMenu Designer can create tcUnicode and Adafruit fonts](https://www.thecoderscorner.com/products/arduino-libraries/tc-menu/using-custom-fonts-in-menu/#creating-a-unicode-or-adafruit-font-using-the-designer-ui) built into the [tcMenu Designer application](https://github.com/TcMenu/tcMenu/releases). As of Designer 4.3 the bitmap font generator has been substantially improved and produces very high quality font exports, even at very small sizes and includes a bitmap editor to touch up any erring pixels.

If someone from the U8G2 and TFT_eSPI teams help us, we'll make it possible to generate U8G2 and TFT_eSPI bitmap fonts from there too. 

TcUnicode represents fonts with a wide range of glyphs from different Unicode groups efficiently, if you've got large gaps between ranges, TcUnicode is more efficient than AdaFruit format.

## TextPipelines

The way we've implemented the interface between primitive drawing and the Unicode handler means in future we can provide transformations, for example a rotation transformation. For now, they only provide the direct support for drawing on each display type.

## How does this support work?

Firstly, you can create fonts by generating from a desktop font file directly from "tcMenu Designer" UI on most desktop platforms, and then they are included into your project as a header file.

There is a custom dialog within "TcMenu Designer UI" where you can select the Unicode blocks, and even sub ranges within blocks that you want to support using the UI.

Please raise all questions in the main TcMenu repository discussions.

## Library and Font Licences

The library is source code is [Apache 2 licenced](LICENSE)
The included fonts are separately licenced [Font licence page](src/Fonts/font-licenses.txt)

## Questions and documentation

TheCodersCorner.com invest a lot of time and resources into making this open source product which is used by literally thousands of users. We offer both [commercial support](https://www.thecoderscorner.com/support-services/training-support/) and [C++/Java/Dart consultancy](https://www.thecoderscorner.com/support-services/consultancy/), or if you just want to say thanks, you can also make a donation via [GitHub](https://github.com/TcMenu/tcMenu) (this repository). 

<a href="https://www.buymeacoffee.com/davetcc" target="_blank"><img src="https://cdn.buymeacoffee.com/buttons/v2/default-blue.png" alt="Buy Me A Coffee" style="height: 60px !important;width: 217px !important;" ></a>

* [UTF-8 decoder and Unicode font documentation on TheCodersCorner.com](https://www.thecoderscorner.com/products/arduino-libraries/tc-unicode-helper/)
* [discussions section of the tcMenu repo](https://github.com/TcMenu/tcMenu/discussions) of tcmenu git repo
* [Arduino discussion forum](https://forum.arduino.cc/) where questions can be asked, please tag me using `@davetcc`.
* [Legacy discussion forum probably to be made read only soon](https://www.thecoderscorner.com/jforum/).

