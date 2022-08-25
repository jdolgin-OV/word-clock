# word-clock
Large-scale LED word clock with additional features such as scrolling message, rainbow color, etc. 

Materials:
ESP32 Wroom Module Breakout
4x at42qt1010 capacitive touch breakouts 
1x DS3231 RTC breakout
576 WS2812 LEDs at 60 LEDs/m

Libraries:

Adafruit_GFX.h https://learn.adafruit.com/adafruit-gfx-graphics-library?gclid=CjwKCAjwiJqWBhBdEiwAtESPaNnGOo8SvMXSlS327Ipo8hFoow_a29YUybYNr93avlGfNRe0t7xqWhoCr6IQAvD_BwE

FastLED.h 

FastLED_NeoMAtrix.h https://github.com/marcmerlin/FastLED_NeoMatrix

Wifi.h

AsyncTCP.h https://github.com/me-no-dev/AsyncTCP

ESPAsyncWebServer.h https://github.com/me-no-dev/ESPAsyncWebServer


Files:

colorPallet.h - adapted from FastLED example "ColorPallet", used to show random LED animations on the clock 

scroll_text.h - custom scrolling LED functions, as well as a show image function

wifi_setup.h - adapted from scott marleys ESP32 Graphic equalizer https://github.com/s-marley/ESP32-INMP441-Matrix-VU/blob/main/INMP441Matrix/web_server.h

word_clock.h - all necessary word clock functions

Word_Clock.ino - Arduino file to brig everything together

See Word Clock PDF for images of build and user guide.

