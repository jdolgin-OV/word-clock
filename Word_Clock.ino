#include "word_clock.h"
#include "scroll_text.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

void setup() {
  //Initialize Serial monitor with power up safety delay
  Serial.begin(9600);
  delay(2000);

  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

  //Initialize LEDs
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip ); //Initialize LED strips
  FastLED.setBrightness(brightness);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, MAX_POWER_MILLIAMPS);
  FastLED.clear();
  matrix->begin();

  // Start the I2C interface
  Wire.begin();

  //Ensure RTC connected
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }
  //Adjust to current date time
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  pinMode(HOUR_BUTTON, INPUT);
  pinMode(MINUTE_BUTTON, INPUT);
  pinMode(TIME_ZONE, INPUT);
  pinMode(MODE_BUTTON, INPUT);
  pinMode(PIR_PIN, INPUT);

  //When the mode button is pressed call update mode
  attachInterrupt(digitalPinToInterrupt(MODE_BUTTON), update_mode, HIGH);

  setupWebServer();

  start();

}

void loop()
{
  FastLED.setBrightness(brightness);

  current_time = millis();
  if (digitalRead(PIR_PIN))
    time_since_last_wake = millis();

  while (current_time - time_since_last_wake > SLEEP_TIME)
  {
    current_time = millis();
    if (digitalRead(PIR_PIN))
    {
      time_since_last_wake = millis();
      break;
    }
    FastLED.clear();
    FastLED.show();
  }

  if (change_mode)
  {
    Serial.println("Changed");
    FastLED.clear();
    FastLED.show();
    delay(50);
    change_mode = false;
  }

  EVERY_N_MILLISECONDS(10)
  {
    hue = (hue + 1) % 255;
  }

  //Go to the right mode
  switch (clock_mode)
  {
    case 1:
      show_word_time();
      break;
    case 2:
      show_digital_time();
      break;
    case 3:
      rainbow_show();
      break;
    case 4:
      ChangePalettePeriodically();

      static uint8_t startIndex = 0;
      startIndex = startIndex + 1; /* motion speed */

      FillLEDsFromPaletteColors( startIndex);

      FastLED.show();
      FastLED.delay(1000 / UPDATES_PER_SECOND);
      break;
    case 5:
      scroll_text(text);
      break;
    default:
      show_word_time();
      break;
  }
}

void show_IP()
{
  IPAddress ip = WiFi.localIP();
  String ip_address = ip.toString();
  Serial.println(ip_address);

  FastLED.clear();
  uint8_t dot[] = {123, 125};

  for (uint8_t i = 0; i < ip_address.length(); i++)
  {
    uint8_t ip[] = {7, 8};
    set_leds(ip, CRGB(red, green, blue));

    char c = ip_address.charAt(i);

    if (c == '.')
    {
      set_leds(dot, CRGB(red, green, blue));
      Serial.println("Dot");
    }
    else
    {
      c = c - 48;
      switch (c)
      {
        case 1:
          set_leds(one, CRGB(red, green, blue));
          Serial.println("One");
          break;
        case 2:
          set_leds(two, CRGB(red, green, blue));
          Serial.println("Two");
          break;
        case 3:
          set_leds(three, CRGB(red, green, blue));
          Serial.println("Three");
          break;
        case 4:
          set_leds(four, CRGB(red, green, blue));
          Serial.println("Four");
          break;
        case 5:
          set_leds(five_h, CRGB(red, green, blue));
          Serial.println("Five");
          break;
        case 6:
          set_leds(six, CRGB(red, green, blue));
          Serial.println("Six");
          break;
        case 7:
          set_leds(seven, CRGB(red, green, blue));
          Serial.println("Seven");
          break;
        case 8:
          set_leds(eight, CRGB(red, green, blue));
          Serial.println("Eight");
          break;
        case 9:
          set_leds(nine, CRGB(red, green, blue));
          Serial.println("Nine");
          break;
        default:
          uint8_t z[] = {11, 11};
          uint8_t e[] = {34, 34};
          uint8_t ro[] = {36, 37};
          set_leds(z, CRGB(red, green, blue));
          set_leds(e, CRGB(red, green, blue));
          set_leds(ro, CRGB(red, green, blue));
          Serial.println("Zero");
      }
    }
    FastLED.show();
    delay(1000);
    FastLED.clear();
    delay(500);
  }
}

void start()
{
  unsigned long start_time = millis();
  unsigned long new_time = millis();
  uint8_t temp_brightness = brightness;
  int8_t sign = -1;

  while (new_time - start_time <= 15000)
  {
    new_time = millis();
    EVERY_N_MILLISECONDS(1)
    {
      if (temp_brightness == 0)
      {
        sign = 1;
      }
      else if (temp_brightness >= brightness)
      {
        sign = -1;
      }
      temp_brightness += sign;
    }
    FastLED.setBrightness(temp_brightness);
    uint8_t omers[5] = {10, 13, 34, 37, 58};
    draw_vert(omers, 5);
    uint8_t cl[5] = {12, 35, 36, 59, 60};
    draw_vert(cl, 5);
    uint8_t jd[2] = {66, 67};
    set_leds(jd, CRGB(red, green, blue));
    uint8_t cr[2] = {42, 43};
    set_leds(cr, CRGB(red, green, blue));
    FastLED.show();
  }

  FastLED.clear();
  show_IP();
}

//used to draw a vertical line as opposed to horizontal
void draw_vert(uint8_t address[], uint8_t len)
{
  for (uint8_t i = 0; i < len; i ++)
  {
    uint8_t temp[2] = {address[i], address[i]};
    set_leds(temp, CRGB(red, green, blue));
  }
}
