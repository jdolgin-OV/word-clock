#ifndef _word_clock_H
#define _word_clock_H


#include "colorPallet.h"
#include "scroll_text.h"

//RTC libraries
#include <RTClib.h>
#include <Wire.h>

//Initialize the RTC
RTC_DS3231 rtc;
uint8_t hour, minute, second;
DateTime now;


int hue = 0;

//Button/PIR pin definitions
#define HOUR_BUTTON 33
#define MINUTE_BUTTON 25
#define TIME_ZONE 26
#define MODE_BUTTON 27

//Time zones
#define NUM_TIME_ZONES 3
uint8_t time_zone_count = 0;
int8_t time_zone_arr[] = {19, 21, 8};
String time_zone_names[] = {"Toronto", "San Francisco", "London"};
uint8_t time_zone_leds[][2] = {{6, 8}, {4, 5}, {0, 2}};

//Use for automatic sleep timer
unsigned long time_since_last_wake = 0;
unsigned long current_time = millis();

//12 hours in milliseconds
#define SLEEP_TIME 43200000

//array of LED indices for each word
uint8_t it[] = {132, 133};
uint8_t is[] = {135, 136};
uint8_t five[] = {115, 118};
uint8_t ten[] = {98, 100};
uint8_t a[] = {131, 131};
uint8_t quarter[] = {123, 129};
uint8_t twenty[] = {108, 113};
uint8_t half[] = {104, 107};

uint8_t to[] = {92, 93};
uint8_t past[] = {80, 83};
uint8_t minutes[] = {84, 90};

uint8_t one[] = {25, 27};
uint8_t two[] = {48, 50};
uint8_t three[] = {28, 32};
uint8_t four[] = {74, 77};
uint8_t five_h[] = {52, 55};
uint8_t six[] = {33, 35};
uint8_t seven[] = {60, 64};
uint8_t eight[] = {36, 40};
uint8_t nine[] = {56, 59};
uint8_t ten_h[] = {12, 14};
uint8_t eleven[] = {42, 47};
uint8_t twelve[] = {66, 71};

uint8_t oclock[] = {18, 23};
uint8_t in[] = {10, 11};

//Width and height of number character for digital time mode
#define NUM_WIDTH 4
#define NUM_HEIGHT 5

//nums[i] is an array of columns, where a 1 represents a light
/*
the first 3 indeces are just fillers, for some reason when changing the time in digital mode 
numbers 0, 1, 2 would be incorrect, moving the address fixed the issue. Maybe some sort of memory overlap?

 */
bool nums[13][NUM_WIDTH][NUM_HEIGHT] = {
  {{0, 1, 1, 1, 0}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {0, 1, 1, 1, 0}},
  {{0, 1, 1, 1, 0}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {0, 1, 1, 1, 0}},
  {{0, 1, 1, 1, 0}, {1, 0, 0, 0, 1}, {1, 0, 0, 0, 1}, {0, 1, 1, 1, 0}},
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

//boolean to store whether to show the dots with the time
bool dot = false;


//Show LED Rainbow animation
void rainbow_show()
{
  for (int i = 0; i < NUM_LEDS; i++)
  {
    //Create gradient hue
    leds[i] = CHSV((hue + 10 * i) % 255, 255, brightness);
  }
  FastLED.show();
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
  //If clock mode is 0, show user input color, else fade rainbow
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

  //always light "it is"
  set_leds(it, color);
  Serial.print("It ");

  set_leds(is, color);
  Serial.print("Is ");

  //either 5 past or 5 to
  if ((minute >= 5 && minute < 10) || minute >= 55)
  {
    set_leds(five, color);
    Serial.print("Five ");
  }
  //either 10 past or 10 to
  else if ((minute >= 10 && minute < 15) || minute >= 50)
  {
    set_leds(ten, color);
    Serial.print("Ten ");
  }
  //either quarter past or quarter to
  else if ((minute >= 15 && minute < 20) || minute >= 45)
  {
    set_leds(a, color);
    Serial.print("A ");

    set_leds(quarter, color);
    Serial.print("Quarter ");
  }
  //either twenty past or twenty to
  else if ((minute >= 20 && minute < 25) || minute >= 40)
  {
    set_leds(twenty, color);
    Serial.print("Twenty ");
  }
  //either twenty five past or twenty five to
  else if ((minute >= 25 && minute < 30) || minute >= 35)
  {
    set_leds(twenty, color);
    set_leds(five, color);
    Serial.print("Twenty Five ");
  }
  //either half past or half to
  else if ((minute >= 30))
  {
    set_leds(half, color);
    Serial.print("Half ");
  }

  //if less than 5 show O'clock
  if (minute < 5)
  {
    set_leds(oclock, color);
    Serial.print("O'Clock ");
  }
  //half half past roll over to "to"
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

  //only show minutes when not saying o'clock, a quarter, half
  if ((minute >= 5 && minute < 15) || (minute >= 20 && minute < 30) || (minute >= 35 && minute < 45) || minute >= 50)
  {
    set_leds(minutes, color);
    Serial.print("Minutes ");
  }

  //display the right hour
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

  //always light in
  set_leds(in, color);
  Serial.print("in ");

  //select time zone
  set_leds(time_zone_leds[time_zone_count], color);
  Serial.println(time_zone_names[time_zone_count]);
}

//read buttons and update time
void poll_buttons()
{
  if (digitalRead(HOUR_BUTTON))
  {
    //Update to the next hour
    rtc.adjust(DateTime(now.year(), now.month(), now.day(), (now.hour() + 1) % 24, now.minute(), 0));
    FastLED.delay(500);
    FastLED.clear();
  }

  if (digitalRead(MINUTE_BUTTON))
  {
    //Update to the next minute
    rtc.adjust(DateTime(now.year(), now.month(), now.day(), now.hour(), (now.minute() + 1) % 60, 0));
    FastLED.delay(500);
    FastLED.clear();
  }

  if (digitalRead(TIME_ZONE))
  {
    //Update time zone
    time_zone_count = (time_zone_count + 1) % 3;
    rtc.adjust(DateTime(now.year(), now.month(), now.day(), (now.hour() + time_zone_arr[time_zone_count]) % 24, now.minute(), 0));
    FastLED.delay(500);
    FastLED.clear();
  }
}


void show_word_time()
{

  //Get the current time
  get_time();
  //monitor and update the buttons
  poll_buttons();

  FastLED.clear();
  show_time();
  FastLED.show();
}

//converts an x and y square to an address, used in the set_leds fxn
uint8_t convert_x_y_to_address(uint8_t x, uint8_t y)
{
  if (y % 2 == 0)
  {
    return 143 - (mh * y + x);
  }
  else
  {
    return 143 - (mh * y + 12 - 1 - x);
  }
}


void show_digital_time()
{
  //update time
  poll_buttons();
  //update digital time
  get_time();


  //break 2 digit minute and hour into single digits
  uint8_t minute_ones = minute % 10 + 3;
  uint8_t minute_tens = minute / 10 + 3;
  uint8_t hour_ones = (hour % 12) % 10 + 3;
  uint8_t hour_tens = (hour % 12) / 10 + 3;

  Serial.println(String(hour_tens) + String(hour_ones) + ":" + String(minute_tens) + String(minute_ones));

  //if 00 show 12
  if (!hour_ones && !hour_tens)
  {
    hour_ones = 2;
    hour_tens = 1;
  }
  FastLED.clear();
  //every 1 second update whether to show the dot
  EVERY_N_MILLISECONDS(1000)
  {
    dot = !dot;
  }

  //if dot is true light up the dot
  if (dot)
  {
    uint8_t dot_address[][2] = {{97, 97}, {121, 121}};
    set_leds(dot_address[0], CRGB(red, green, blue));
    set_leds(dot_address[1], CRGB(red, green, blue));
  }

  uint8_t temp = 0;
  uint8_t addr[2] = {};

  for (uint8_t x = 0; x < NUM_WIDTH; x++)
  {
    for (uint8_t y = 0; y < NUM_HEIGHT; y++)
    {
      //show minute ones
      if (nums[minute_ones][x][y])
      {
        temp = convert_x_y_to_address(NUM_WIDTH - x + 2,  NUM_HEIGHT + y + 1);
        addr[0] = temp;
        addr[1] = temp;
        set_leds(addr, CRGB(red, green, blue));
      }

      //show minute tens
      if (nums[minute_tens][x][y])
      {
        temp = convert_x_y_to_address(2 * NUM_WIDTH - x + 3,  NUM_HEIGHT + y + 1);
        addr[0] = temp;
        addr[1] = temp;
        set_leds(addr, CRGB(red, green, blue));
      }
      //show hour ones
      if (nums[hour_ones][x][y])
      {
        temp = convert_x_y_to_address(NUM_WIDTH - x + 2,  y);
        addr[0] = temp;
        addr[1] = temp;
        set_leds(addr, CRGB(red, green, blue));
      }
      //show hour tens
      if (nums[hour_tens][x][y])
      {
        temp = convert_x_y_to_address(2 * NUM_WIDTH - x + 3,  y);
        addr[0] = temp;
        addr[1] = temp;
        set_leds(addr, CRGB(red, green, blue));
      }

    }
  }
  FastLED.show();
  FastLED.delay(1000 / UPDATES_PER_SECOND);

}


//SHOW RAIN AND LIGHTNING MODE

//frequency between strikes
uint8_t frequency = random(1, 30);
//speed at which rain falls
#define FALL_SPEED 200

//32 bit integer color values
const uint32_t RAIN = 0x006af5;
const uint32_t CLOUD = 0x7F7F7F;
const uint32_t LIGHTNING = 0xffee00;
const uint32_t BLACK = 0x000000;

//different lengths of lightning bolts
uint8_t lightning_lengths[5] = {0, 4, 7, 10, 13};

//array holding what is to be shown on screen
uint32_t weather[mh][mw / LEDS_PER_COL] PROGMEM = {{BLACK, CLOUD, CLOUD, CLOUD, BLACK,  BLACK,  BLACK, BLACK, CLOUD, CLOUD, CLOUD, BLACK},
  {CLOUD, CLOUD, CLOUD, CLOUD, CLOUD,  BLACK,  BLACK, CLOUD, CLOUD, CLOUD, CLOUD, CLOUD},
  {BLACK, BLACK, BLACK, BLACK, BLACK,  BLACK,  BLACK, BLACK, BLACK, BLACK, BLACK, BLACK},
  {BLACK, BLACK, BLACK, BLACK, BLACK,  BLACK,  BLACK, BLACK, BLACK, BLACK, BLACK, BLACK},
  {BLACK, BLACK, BLACK, BLACK, BLACK,  BLACK,  BLACK, BLACK, BLACK, BLACK, BLACK, BLACK},
  {BLACK, BLACK, BLACK, BLACK, BLACK,  BLACK,  BLACK, BLACK, BLACK, BLACK, BLACK, BLACK},
  {BLACK, BLACK, BLACK, BLACK, BLACK,  BLACK,  BLACK, BLACK, BLACK, BLACK, BLACK, BLACK},
  {BLACK, BLACK, BLACK, BLACK, BLACK,  BLACK,  BLACK, BLACK, BLACK, BLACK, BLACK, BLACK},
  {BLACK, BLACK, BLACK, BLACK, BLACK,  BLACK,  BLACK, BLACK, BLACK, BLACK, BLACK, BLACK},
  {BLACK, BLACK, BLACK, BLACK, BLACK,  BLACK,  BLACK, BLACK, BLACK, BLACK, BLACK, BLACK},
  {BLACK, BLACK, BLACK, BLACK, BLACK,  BLACK,  BLACK, BLACK, BLACK, BLACK, BLACK, BLACK},
  {BLACK, BLACK, BLACK, BLACK, BLACK,  BLACK,  BLACK, BLACK, BLACK, BLACK, BLACK, BLACK}
};

//clear the whole display except the clouds
void clear_display()
{
  for (uint8_t i = mh - 1; i >= 2; i--)
  {
    for (uint8_t j = 0; j < mw / LEDS_PER_COL; j++)
    {
      weather[i][j] = BLACK;
    }
  }
}


void show_lightning()
{
  uint8_t ptr = random(0, 5);
  uint8_t right_length = lightning_lengths[ptr];


  ptr = random(0, 5);
  uint8_t left_length = lightning_lengths[ptr];

  //clear display to show lightning
  clear_display();

  //right bolt x value starts at index 9
  uint8_t right_x = 9;
  int8_t right_sign = -1;
  //decide whether to go left or right
  if (random(0, 2))
  {
    right_sign = 1;
  }

  //left bolt starts at index 2
  uint8_t left_x = 2;
  int8_t left_sign = -1;
  //decide whether to go left or right
  if (random(0, 2))
  {
    left_sign = 1;
  }

  uint8_t curr_right_length = 0;
  uint8_t curr_left_length = 0;

  //draw the lightning
  for (uint8_t y = 2; y < mh - 1; y++)
  {
    if (curr_right_length < right_length)
    {
      //keep drawing
      //every 2 down steps move left or right
      if (y % 2 == 1)
      {
        weather[y][right_x] = LIGHTNING;
        curr_right_length++;

        weather[y][right_x + right_sign] = LIGHTNING;
        curr_right_length++;

        right_x += right_sign;
        right_sign *= -1;
      }
      //else just go down
      else
      {

        weather[y][right_x] = LIGHTNING;
        curr_right_length++;
      }
    }

    if (curr_left_length < left_length)
    {
      //keep drawing
      //every 2 down steps move left or right
      if (y % 2 == 1)
      {
        weather[y][left_x] = LIGHTNING;
        curr_left_length++;

        weather[y][left_x + left_sign] = LIGHTNING;
        curr_left_length++;

        left_x += left_sign;
        left_sign *= -1;
      }
      //else just go down
      else
      {
        weather[y][left_x] = LIGHTNING;
        curr_left_length++;
      }
    }
  }
  //show the lightning
  show_image(weather);
  //delay for a random time before clearing
  delay(random(1, 8) * 100);
  clear_display();
  show_image(weather);
}

void weather_display() {
  //every frequency num of seconds show lightning
  randomSeed(analogRead(3));
  EVERY_N_MILLISECONDS(1000 * frequency)
  {
    frequency = random(1, 61);
    Serial.println(frequency);
    show_lightning();
  }

  //every 2 times a drop falls add another
  EVERY_N_MILLISECONDS(2 * FALL_SPEED)
  {
    uint8_t col = 0;
    bool added = false;
    //if there is a drop right below where one is added dont add it
    col = random(0, 5);
    if (weather[3][col] != RAIN)
    {
      weather[2][col] = RAIN;
      added = true;
    }

    col = random(7, 12);
    //if there is a drop right below where one is added dont add it
    if (weather[3][col] != RAIN)
    {
      weather[2][col] = RAIN;
      added = true;
    }
    //if a drop was added
    if (added)
    {
      for (uint8_t i = mh - 1; i >= 3; i--)
      {
        for (uint8_t j = 0; j < mw / LEDS_PER_COL; j++)
        {
          //move each drop down a col
          if (weather[i][j] == RAIN)
          {
            weather[i][j] = BLACK;
            weather[i + 1][j] = RAIN;
          }
        }
      }
      show_image(weather);
      delay(FALL_SPEED);
    }
  }

  //then drop again
  for (uint8_t i = mh - 1; i >= 2; i--)
  {
    for (uint8_t j = 0; j < mw / LEDS_PER_COL; j++)
    {
      if (weather[i][j] == RAIN)
      {
        weather[i][j] = BLACK;
        weather[i + 1][j] = RAIN;
      }
    }
  }

  show_image(weather);
  delay(FALL_SPEED);
}


#endif
