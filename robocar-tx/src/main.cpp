#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include "Adafruit_miniTFTWing.h"

enum pin { ch_3 = 30,
           ch_4 = 11,
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

enum button { joy_up, joy_down, joy_left, joy_right, joy_select, tft_a, 
              tft_b, ch_4 };

Adafruit_miniTFTWing ss;
Adafruit_ST7735 tft = Adafruit_ST7735(pin::tft_cs, pin::tft_dc, pin::tft_rst);

void setup() {
  // set input pins
  pinMode(pin::ch_4, INPUT);
  pinMode(pin::st_rev, INPUT_PULLUP);
  pinMode(pin::th_rev, INPUT_PULLUP);

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

  // poll analog
  int ch_3_in = analogRead(pin::ch_3);
  int st_in = analogRead(pin::st);
  int st_exp_in = analogRead(pin::st_dr);
  int st_trm_in = analogRead(pin::st_trm);
  int th_in = analogRead(pin::th);
  int th_exp_in = analogRead(pin::th_dr);
  int th_trm_in = analogRead(pin::th_trm);

  // poll digital
  int ch_4_in = digitalRead(pin::ch_4);
  bool st_rev = digitalRead(pin::st_rev) == LOW;
  bool th_rev = digitalRead(pin::th_rev) == LOW;

  uint32_t ss_btns = ss.readButtons();

  // reverse st/th if necessary
  if (st_rev) {
    st_in = abs(st_in - 1023);
  }
  if (th_rev) {
    th_in = abs(th_in - 1023);
  }

  // convert to bytes
  byte btns_out = 0;
  byte ch_3_out = map(ch_3_in, 0, 1023, 0, 255);
  byte st_out = st_out = map(st_in, 0, 1023, 0, 255);
  byte th_out = th_out = map(th_in, 0, 1023, 0, 255);

  if (ch_4_in == HIGH) {
    btns_out |= 1 << button::ch_4;
  }

  if (! (ss_btns & TFTWING_BUTTON_LEFT)) {
    btns_out |= 1 << button::joy_left;
  }
  if (! (ss_btns & TFTWING_BUTTON_RIGHT)) {
    btns_out |= 1 << button::joy_right;
  }
  if (! (ss_btns & TFTWING_BUTTON_DOWN)) {
    btns_out |= 1 << button::joy_down;
  }
  if (! (ss_btns & TFTWING_BUTTON_UP)) {
    btns_out |= 1 << button::joy_up;  
  }
  if (! (ss_btns & TFTWING_BUTTON_A)) {
    btns_out |= 1 << button::tft_a;
  }
  if (! (ss_btns & TFTWING_BUTTON_B)) {
    btns_out |= 1 << button::tft_b;
  }
  if (! (ss_btns & TFTWING_BUTTON_SELECT)) {
    btns_out |= 1 << button::joy_select;
  }

  // send to car
  

}