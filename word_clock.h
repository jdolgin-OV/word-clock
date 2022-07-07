#ifndef _word_clock_H
#define _word_clock_H

//Include the color pallet animations
#include "colorPallet.h"

//RTC libraries
#include <RTClib.h>
#include <Wire.h>

//Initialize the RTC
RTC_DS3231 rtc;
uint8_t hour, minute, second;
DateTime now;

//LED data
#define LEDS_PER_COL 4

int hue = 0;

//Button/PIR pin definitions
#define PIR_PIN 35
#define HOUR_BUTTON 33
#define MINUTE_BUTTON 25
#define TIME_ZONE 26
#define MODE_BUTTON 27

//Time zones
#define NUM_TIME_ZONES 3
uint8_t time_zone_count = 0;
int8_t time_zone_arr[] = {19, 21, 8};
String time_zone_names[] = {"Toronto", "San Francisco", "London"};
uint8_t time_zone_leds[][2] = {{138, 140}, {136, 137}, {132, 134}};

//Use for automatic sleep timer
unsigned long time_since_last_wake = 0;
unsigned long current_time = millis();
#define SLEEP_TIME 720000

uint8_t it[] = {0, 1};
uint8_t is[] = {3, 4};
uint8_t five[] = {30, 33};
uint8_t ten[] = {38, 40};
uint8_t a[] = {23, 23};
uint8_t quarter[] = {15, 21};
uint8_t twenty[] = {24, 29};
uint8_t half[] = {44, 47};

uint8_t to[] = {56, 57};
uint8_t past[] = {68, 71};
uint8_t minutes[] = {48, 54};

uint8_t one[] = {109, 111};
uint8_t two[] = {84, 86};
uint8_t three[] = {112, 116};
uint8_t four[] = {62, 65};
uint8_t five_h[] = {88, 91};
uint8_t six[] = {117, 119};
uint8_t seven[] = {72, 76};
uint8_t eight[] = {96, 100};
uint8_t nine[] = {92, 95};
uint8_t ten_h[] = {120, 122};
uint8_t eleven[] = {102, 107};
uint8_t twelve[] = {78, 83};

uint8_t oclock[] = {126, 131};
uint8_t in[] = {142, 143};

#define NUM_WIDTH 4
#define NUM_HEIGHT 5

bool nums[10][NUM_WIDTH][NUM_HEIGHT] = {
  {{0, 1, 1, 1, 0}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {0, 1, 1, 1, 0}},
  {{0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {1, 1, 1, 1, 1}, {0, 0, 0, 0, 0}},
  {{1, 0, 1, 1, 1}, {1, 0, 1, 0, 1}, {1, 0, 1, 0, 1}, {1, 1, 1, 0, 1}},
  {{1, 0, 1, 0, 1}, {1, 0, 1, 0, 1}, {1, 0, 1, 0, 1}, {1, 1, 1, 1, 1}},
  {{1, 1, 1, 0, 0}, {0, 0, 1, 0, 0}, {0, 0, 1, 0, 0}, {1, 1, 1, 1, 1}},
  {{1, 1, 1, 0, 1}, {1, 0, 1, 0, 1}, {1, 0, 1, 0, 1}, {1, 0, 1, 1, 1}},
  {{1, 1, 1, 1, 1}, {1, 0, 1, 0, 1}, {1, 0, 1, 0, 1}, {1, 0, 1, 1, 1}},
  {{1, 0, 0, 0, 0}, {1, 0, 0, 0, 0}, {1, 0, 0, 0, 0}, {1, 1, 1, 1, 1}},
  {{1, 1, 1, 1, 1}, {1, 0, 1, 0, 1}, {1, 0, 1, 0, 1}, {1, 1, 1, 1, 1}},
  {{1, 1, 1, 0, 1}, {1, 0, 1, 0, 1}, {1, 0, 1, 0, 1}, {1, 1, 1, 1, 1}},
};

bool dot = false;

bool screen[mh][mw] = {0};
//Show LED Rainbow animation
void rainbow_show()
{
  for (int i = 0; i < NUM_LEDS; i++)
  {
    //Create gradient hue
    leds[i] = CHSV((hue + 5 * i) % 255, 255, brightness);
  }
  FastLED.show();
}

//Interrupt routine to update the clock mode
void update_mode()
{
  //DIY button debounce, only interrupt every 500 ms
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  if (interrupt_time - last_interrupt_time > 500)
  {
    //Iterate mode
    clock_mode = (clock_mode + 1) % NUM_MODES;
    change_mode = true;
    FastLED.clear();
    last_interrupt_time = interrupt_time;
  }
}

//Given an array of LED addresses, light up the LEDs the given color
void set_leds(uint8_t led_address[], CRGB color)
{

  uint16_t start_address = led_address[0] * LEDS_PER_COL;
  uint16_t end_address = led_address[1] * LEDS_PER_COL + LEDS_PER_COL - 1;


  for (uint16_t i = start_address; i <= end_address; i++)
  {
    leds[i] = color;
  }
}

//Get the current time
void get_time()
{
  now = rtc.now();
  hour = now.hour();
  minute = now.minute();
  second = now.second();

  //  Serial.print(hour);
  //  Serial.print("/");
  //  Serial.print(minute);
  //  Serial.print("/");
  //  Serial.println(second);
}

//Take in the hour and minute and display on the clock
void show_time()
{
  CRGB color;
  if (clock_mode == 0)
  {
    color = CRGB(red, green, blue);
  }
  else if (clock_mode == 1)
  {
    color = CHSV(hue, 255, brightness);
  }

  //If minute >= 35 minutes are to the next hour
  if (minute >= 35)
    hour += 1;

  //12 hour time
  hour = hour % 12;

  //Holds starting and ending address
  uint8_t address[2] = {0, 1};

  set_leds(it, color);
  Serial.print("It ");

  set_leds(is, color);
  Serial.print("Is ");

  if ((minute >= 5 && minute < 10) || minute >= 55)
  {
    set_leds(five, color);
    Serial.print("Five ");
  }
  else if ((minute >= 10 && minute < 15) || minute >= 50)
  {
    set_leds(ten, color);
    Serial.print("Ten ");
  }
  else if ((minute >= 15 && minute < 20) || minute >= 45)
  {
    set_leds(a, color);
    Serial.print("A ");

    set_leds(quarter, color);
    Serial.print("Quarter ");
  }
  else if ((minute >= 20 && minute < 25) || minute >= 40)
  {
    set_leds(twenty, color);
    Serial.print("Twenty ");
  }
  else if ((minute >= 25 && minute < 30) || minute >= 35)
  {
    set_leds(twenty, color);
    set_leds(five, color);
    Serial.print("Twenty Five ");
  }
  else if ((minute >= 30))
  {
    set_leds(half, color);
    Serial.print("Half ");
  }

  if (minute < 5)
  {
    set_leds(oclock, color);
    Serial.print("O'Clock ");
  }
  else if (minute >= 35)
  {
    set_leds(to, color);
    Serial.print("To ");
  }
  else
  {
    set_leds(past, color);
    Serial.print("Past ");
  }

  if ((minute >= 5 && minute < 15) || (minute >= 20 && minute < 30) || (minute >= 35 && minute < 45) || minute >= 50)
  {
    set_leds(minutes, color);
    Serial.print("Minutes ");
  }

  switch (hour)
  {
    case 1:
      set_leds(one, color);
      Serial.println("One");
      break;
    case 2:
      set_leds(two, color);
      Serial.println("Two");
      break;
    case 3:
      set_leds(three, color);
      Serial.println("Three");
      break;
    case 4:
      set_leds(four, color);
      Serial.println("Four");
      break;
    case 5:
      set_leds(five_h, color);
      Serial.println("Five");
      break;
    case 6:
      set_leds(six, color);
      Serial.println("Six");
      break;
    case 7:
      set_leds(seven, color);
      Serial.println("Seven");
      break;
    case 8:
      set_leds(eight, color);
      Serial.println("Eight");
      break;
    case 9:
      set_leds(nine, color);
      Serial.println("Nine");
      break;
    case 10:
      set_leds(ten_h, color);
      Serial.println("Ten");
      break;
    case 11:
      set_leds(eleven, color);
      Serial.println("Eleven");
      break;
    default:
      set_leds(twelve, color);
      Serial.println("Twelve");
      break;
  }

  set_leds(in, color);
  Serial.print("in ");

  set_leds(time_zone_leds[time_zone_count], color);
  Serial.println(time_zone_names[time_zone_count]);
}

//monitor and update the buttons
void show_word_time()
{

  //Get the current time
  get_time();

  if (digitalRead(HOUR_BUTTON))
  {
    //Update to the next hour
    rtc.adjust(DateTime(now.year(), now.month(), now.day(), (now.hour() + 1) % 24, now.minute(), 0));
    delay(500);
    FastLED.clear();
  }

  if (digitalRead(MINUTE_BUTTON))
  {
    //Update to the next minute
    rtc.adjust(DateTime(now.year(), now.month(), now.day(), now.hour(), (now.minute() + 1) % 60, 0));
    delay(500);
    FastLED.clear();
  }

  if (digitalRead(TIME_ZONE))
  {
    //Update time zone
    time_zone_count = (time_zone_count + 1) % 3;
    rtc.adjust(DateTime(now.year(), now.month(), now.day(), (now.hour() + time_zone_arr[time_zone_count]) % 24, now.minute(), 0));
    delay(500);
    FastLED.clear();
  }

  FastLED.clear();
  show_time();
  FastLED.show();
}

void show_digital_time()
{
  get_time();

  if (digitalRead(HOUR_BUTTON))
  {
    //Update to the next hour
    rtc.adjust(DateTime(now.year(), now.month(), now.day(), (now.hour() + 1) % 24, now.minute(), 0));
    delay(500);
    FastLED.clear();
  }

  if (digitalRead(MINUTE_BUTTON))
  {
    //Update to the next minute
    rtc.adjust(DateTime(now.year(), now.month(), now.day(), now.hour(), (now.minute() + 1) % 60, 0));
    delay(500);
    FastLED.clear();
  }

  if (digitalRead(TIME_ZONE))
  {
    //Update time zone
    time_zone_count = (time_zone_count + 1) % 3;
    rtc.adjust(DateTime(now.year(), now.month(), now.day(), (now.hour() + time_zone_arr[time_zone_count]) % 24, now.minute(), 0));
    delay(500);
    FastLED.clear();
  }

  uint8_t minute_ones = minute % 10;
  uint8_t minute_tens = minute / 10;
  uint8_t hour_ones = (hour % 12) % 10;
  uint8_t hour_tens = (hour % 12) / 10;

  if(!hour_ones && !hour_tens)
  {
    hour_ones = 2;
    hour_tens = 1;
  }

  EVERY_N_MILLISECONDS(1000)
  {
    dot = !dot;
    CRGB color = CRGB(red, green, blue);
    if(!dot)
    {
      color = CRGB(0, 0, 0);
    }

    uint8_t dot_address[][2] ={{13, 13}, {37, 37}};
    set_leds(dot_address[0], color);
    set_leds(dot_address[1], color);
  }


  for (uint8_t x = 0; x < NUM_WIDTH * LEDS_PER_COL; x++)
  {
    for (uint8_t y = 0; y < NUM_HEIGHT; y++)
    {
      if (nums[minute_ones][x / LEDS_PER_COL][y])
      {
        matrix->drawPixel(x + ((NUM_WIDTH + 1) * LEDS_PER_COL), y + (mh - NUM_HEIGHT), CRGB(red, green, blue));
      }
      else
      {
        matrix->drawPixel(x + ((NUM_WIDTH + 1) * LEDS_PER_COL), y + (mh - NUM_HEIGHT), CRGB(0, 0, 0));
      }
      if (nums[minute_tens][x / 4][y])
      {
        matrix->drawPixel(x, y + (mh - NUM_HEIGHT), CRGB(red, green, blue));
      }
      else
      {
        matrix->drawPixel(x, y + (mh - NUM_HEIGHT), CRGB(0, 0, 0));
      }
      if (nums[hour_ones][x / LEDS_PER_COL][y])
      {
        matrix->drawPixel(x + ((NUM_WIDTH + 1) * LEDS_PER_COL), y, CRGB(red, green, blue));
      }
      else
      {
        matrix->drawPixel(x + ((NUM_WIDTH + 1) * LEDS_PER_COL), y, CRGB(0, 0, 0));
      }
      if (nums[hour_tens][x / 4][y])
      {
        matrix->drawPixel(x, y, CRGB(red, green, blue));
      }
      else
      {
        matrix->drawPixel(x, y, CRGB(0, 0, 0));
      }
    }
  }
  
  FastLED.show();
}

#endif
