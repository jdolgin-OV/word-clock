#include "word_clock.h"

void setup() {
  //Initialize Serial monitor with power up safety delay of 2s
  Serial.begin(9600);
  delay(2000);


  //Initialize LEDs
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip ); //Initialize LED strips
  FastLED.setBrightness(brightness);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, MAX_POWER_MILLIAMPS);
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
  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  pinMode(HOUR_BUTTON, INPUT);
  pinMode(MINUTE_BUTTON, INPUT);
  pinMode(TIME_ZONE, INPUT);
  pinMode(MODE_BUTTON, INPUT);

  //When the mode button is pressed call update mode
  attachInterrupt(digitalPinToInterrupt(MODE_BUTTON), update_mode, HIGH);
  
  //show omers logo while connecting to wifi
  show_image(OMERS_LOGO);

  //set up web server and show start animations
  setupWebServer();
  FastLED.clear();
  start();

}

void loop()
{
  //if auto change, change modes every 10 seconds
  if (auto_change)
  {
    EVERY_N_MILLISECONDS(AUTO_CHANGE_TIME)
    {
      update_mode();
    }
  }
  
  //readjust brightness
  FastLED.setBrightness(brightness);

  //if motion detected adjust time last sensed
  current_time = millis();

  //if time since last wake occured more than sleep time long ago
  //clear leds and wait for motion
  if (current_time - time_since_last_wake > SLEEP_TIME || (digitalRead(HOUR_BUTTON) && digitalRead(TIME_ZONE)))
  {
    power = false;
    FastLED.clear();
    FastLED.show();
    delay(500);
  }

  if (!power)
  {
    FastLED.clear();
    FastLED.show();
    while (!power)
    {
      current_time = millis();
      get_time();
      if (digitalRead(HOUR_BUTTON) && digitalRead(TIME_ZONE))
      {
        power = true;
        delay(500);
      }
    }
    time_since_last_wake = millis();
  }


  //if the mode was changed clear the LEDS
  if (change_mode)
  {
    Serial.println("Changed");
    FastLED.clear();
    FastLED.show();
    FastLED.delay(50);
    change_mode = false;
  }

  //for color fading animations fade the hue every 10 ms
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
    case 4:
      rainbow_show();
      break;
    case 5:
      ChangePalettePeriodically();

      static uint8_t startIndex = 0;
      startIndex = startIndex + 1; /* motion speed */

      FillLEDsFromPaletteColors( startIndex);

      FastLED.show();
      FastLED.delay(1000 / UPDATES_PER_SECOND);
      break;
      //   case 6:
      //      weather_display();
      break;
    case 3:
      scroll_text(text);
      break;
    default:
      show_word_time();
      break;
  }
}

void show_IP()
{
  //turn ip address to string
  IPAddress ip = WiFi.localIP();
  String ip_address = ip.toString();
  Serial.println(ip_address);

  FastLED.clear();
  uint8_t dot[] = {15, 17};

  for (uint8_t i = 0; i < ip_address.length(); i++)
  {
    uint8_t ip[] = {139, 140};
    set_leds(ip, CRGB(red, green, blue));
    //show ip
    char c = ip_address.charAt(i);
    if (c == '.')
    {
      set_leds(dot, CRGB(red, green, blue));
      Serial.println("Dot");
    }
    else
    {
      c = c - 48;
      //subtract from ascii value so c = num to display
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
          uint8_t z[] = {143, 143};
          uint8_t e[] = {118, 118};
          uint8_t ro[] = {96, 97};
          set_leds(z, CRGB(red, green, blue));
          set_leds(e, CRGB(red, green, blue));
          set_leds(ro, CRGB(red, green, blue));
          Serial.println("Zero");
      }
    }
    FastLED.show();
    FastLED.delay(1000);

    FastLED.clear();
    set_leds(ip, CRGB(red, green, blue));
    FastLED.delay(500);
  }
}

void start()
{
  FastLED.clear();


  unsigned long start_time = millis();
  unsigned long new_time = millis();
  uint8_t temp_brightness = brightness;
  int8_t sign = -1;

  //for 5s breathe leds to reveal omers clock, jd, cr
  while (new_time - start_time <= 5000)
  {
    new_time = millis();
    //every ms fade brightness either up or down by 1
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
    uint8_t omers[5] = {142, 121, 118, 97, 94};
    draw_vert(omers, 5);
    uint8_t cl[5] = {120, 119, 96, 95, 72};
    draw_vert(cl, 5);
    uint8_t jd[2] = {78, 79};
    set_leds(jd, CRGB(red, green, blue));
    uint8_t cr[2] = {102, 103};
    set_leds(cr, CRGB(red, green, blue));
    FastLED.show();
  }

  FastLED.clear();
  show_IP();
}

//used to draw a vertical line as opposed to horizontal, could also use matrix -> draw veritcal function
void draw_vert(uint8_t address[], uint8_t len)
{
  for (uint8_t i = 0; i < len; i ++)
  {
    uint8_t temp[2] = {address[i], address[i]};
    set_leds(temp, CRGB(red, green, blue));
  }
}
