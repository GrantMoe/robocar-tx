#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include "Adafruit_miniTFTWing.h"


enum pin { ch_3 = 30,
           ch_4 = 11,
           led_b = 6,
           led_g = 8,
           led_r = 14,
           st = 2,
           st_dr = 3,
           st_rev = 16,
           st_trm = 4,
           th = 28,
           th_dr = 29,
           th_rev = 27,
           th_trm = 5,
           tft_dc = 15,
           tft_cs = 7,
           tft_rst = -1
};

Adafruit_miniTFTWing ss;
Adafruit_ST7735 tft = Adafruit_ST7735(pin::tft_cs, pin::tft_dc, pin::tft_rst);

void setup() {
  Serial.begin(115200);

  // tft wing
  if(!ss.begin()) {
    Serial.println("seesaw couldn't be found!");
    while(1);
  }
  Serial.println("seesaw started");
  ss.tftReset();
  ss.setBacklight(TFTWING_BACKLIGHT_ON);
  tft.initR(INITR_MINI160x80);
  Serial.println("TFT initialized");
  tft.setRotation(3); // USB jack on the left, joystick on the right
  tft.fillScreen(ST77XX_RED);
  delay(100);
  tft.fillScreen(ST77XX_GREEN);
  delay(100);
  tft.fillScreen(ST77XX_BLUE);
  delay(100);
  tft.fillScreen(ST77XX_BLACK);
}

void loop() {
  // poll inputs

  // adjust for dual rate? how?

  // analog
  // switch

}