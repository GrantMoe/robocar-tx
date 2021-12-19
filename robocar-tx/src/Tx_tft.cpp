#include "Tx_tft.h"
#include "tx.h"

Tx_tft::Tx_tft(Adafruit_ST7735 tft, GFXcanvas1 canvas) 
    : tft_(tft),  canvas_(canvas) {}


void Tx_tft::init() {
  tft_.initR(INITR_MINI160x80);
  tft_.cp437(true);
  tft_.setRotation(1); // reverse for controller config
  tft_.fillScreen(ST77XX_BLACK);
  delay(200);
}

void Tx_tft::print_screen(String new_str, bool refresh) {
    if (refresh) {
        tft_.fillScreen(ST77XX_BLACK);
        tft_.setCursor(0,0);
        tft_.setTextWrap(true);
    }
    tft_.setCursor(0,20);
    tft_.print(new_str); 
    }

  void Tx_tft::print_scroll(String new_str) {
    static int scroll_size = 9;
    static int scroll_called;
    static String scroll_strs[] = {"", "", "", "", "", "", "", "", ""};
    tft_.setTextWrap(false);
    tft_.setCursor(0, 0);
    tft_.setTextColor(ST7735_WHITE, ST77XX_BLACK);  // should be grey
    tft_.setTextSize(1);
    if (scroll_called < scroll_size) {
      scroll_strs[scroll_called] = new_str;
      scroll_called++;
    }
    else {
      for (int i_shift = 1; i_shift < scroll_size; i_shift++) {
        scroll_strs[i_shift-1] = scroll_strs[i_shift]; 
      }
      scroll_strs[scroll_size-1] = new_str;
    }
    for (int i_print = 0; i_print < scroll_size; i_print++) {
      tft_.print(scroll_strs[i_print]);
      if (tft_.getCursorX() < tft_.width()) {
        tft_.fillRect(tft_.getCursorX(), tft_.getCursorY(), 
                    tft_.width() - tft_.getCursorX(), 16, ST77XX_BLACK);
      }
      tft_.print('\n');
    }
  }

void Tx_tft::show_menu(int mode) {
  // brute
  String top_btn;
  String bot_btn;
  String mode_str;
  if (mode == DRIVE_MODE) {
    top_btn = "";
    bot_btn = "";
    mode_str = "Drive";
  } else if (mode == CALIBRATE_MODE) {
    top_btn = "confirm";
    bot_btn = "cancel";
    mode_str = "Calibrating";
  } else {
    top_btn = "calibrate";
    bot_btn = "";
    mode_str = "Paused";
  }
  // write top button at top
  canvas_.fillScreen(ST77XX_BLACK);
  canvas_.setTextSize(1);
  canvas_.setCursor(0, 15);
  canvas_.print(top_btn);
  canvas_.setCursor(0, 55);
  canvas_.print(bot_btn);
  canvas_.setCursor(20, 30);
  canvas_.setTextSize(2);
  canvas_.print(mode_str);
  tft_.drawBitmap(0, 0, canvas_.getBuffer(), tft_.width(), tft_.height(), 
                 ST7735_WHITE, ST7735_BLACK); 
}
