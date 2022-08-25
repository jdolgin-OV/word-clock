#ifndef _color_H
#define _color_H

//adapted version of FastLED example color_pallet.ino

#include "wifi_setup.h"

//Include LED libraries
#include <Adafruit_GFX.h>
#include <FastLED.h>
#include <FastLED_NeoMatrix.h>

//Define Matrix width and height
#define mw 48
#define mh 12

//Various LED definitions
#define UPDATES_PER_SECOND 60
#define LED_PIN     14
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB
#define NUM_LEDS 48 * 12
#define MAX_POWER_MILLIAMPS 7000
CRGB leds[NUM_LEDS];            // Create LED Object
//LED data
#define LEDS_PER_COL 4

//Matrix Definition, define starting LED corner, whether LEDS are horz rows or vert col
FastLED_NeoMatrix *matrix = new FastLED_NeoMatrix(leds, mw, mh,
    NEO_MATRIX_BOTTOM     + NEO_MATRIX_RIGHT +
    NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG );


CRGBPalette16 currentPalette = RainbowColors_p;
TBlendType    currentBlending = LINEARBLEND;

extern CRGBPalette16 myRedWhiteBluePalette;
extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;

void FillLEDsFromPaletteColors( uint8_t colorIndex)
{
  for ( int i = 0; i < NUM_LEDS; ++i) {
    leds[i] = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
    colorIndex += 3;
  }
}

void SetupTotallyRandomPalette()
{
  for ( int i = 0; i < 16; ++i) {
    currentPalette[i] = CHSV( random8(), 255, brightness);
  }
}

void SetupBlackAndWhiteStripedPalette()
{
  // 'black out' all 16 palette entries...
  fill_solid( currentPalette, 16, CRGB::Black);
  // and set every fourth one to white.
  currentPalette[0] = CRGB::White;
  currentPalette[4] = CRGB::White;
  currentPalette[8] = CRGB::White;
  currentPalette[12] = CRGB::White;

}

void ChangePalettePeriodically()
{
  uint8_t secondHand = (millis() / 1000) % 60;
  static uint8_t lastSecond = 99;

  if ( lastSecond != secondHand) {
    lastSecond = secondHand;
    if ( secondHand ==  0)  {
      currentPalette = RainbowStripeColors_p;
      currentBlending = LINEARBLEND;
    }
    if ( secondHand == 10)  {
      currentPalette = RainbowStripeColors_p;
      currentBlending = NOBLEND;
    }
    if ( secondHand == 15)  {
      currentPalette = RainbowStripeColors_p;
      currentBlending = NOBLEND;
    }
    if ( secondHand == 20)  {
      currentPalette = RainbowStripeColors_p;
      currentBlending = LINEARBLEND;
    }
    if ( secondHand == 25)  {
      currentPalette = CloudColors_p;
      currentBlending = LINEARBLEND;
    }
    if ( secondHand == 30)  {
      SetupBlackAndWhiteStripedPalette();
      currentBlending = NOBLEND;
    }
    if ( secondHand == 35)  {
      SetupBlackAndWhiteStripedPalette();
      currentBlending = LINEARBLEND;
    }
    if ( secondHand == 40)  {
      currentPalette = PartyColors_p;
      currentBlending = NOBLEND;
    }
    if ( secondHand == 45)  {
      currentPalette = PartyColors_p;
      currentBlending = LINEARBLEND;
    }
    if ( secondHand == 50)  {
      currentPalette = myRedWhiteBluePalette_p;
      currentBlending = NOBLEND;
    }
    if ( secondHand == 55)  {
      currentPalette = myRedWhiteBluePalette_p;
      currentBlending = LINEARBLEND;
    }
  }
}



const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM =
{
  CRGB::Red,
  CRGB::Gray, // 'white' is too bright compared to red and blue
  CRGB::Red,
  CRGB::Black,

  CRGB::Red,
  CRGB::Gray,
  CRGB::Red,
  CRGB::Black,

  CRGB::Red,
  CRGB::Red,
  CRGB::Gray,
  CRGB::Gray,
  CRGB::Red,
  CRGB::Red,
  CRGB::Black,
  CRGB::Black
};

#endif
