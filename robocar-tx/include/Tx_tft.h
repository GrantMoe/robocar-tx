#ifndef TX_TFT_H
#define TX_TFT_H

#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>


class Tx_tft {
 public:
  Tx_tft(Adafruit_ST7735, GFXcanvas1);
  
  void init();
  void print_scroll(String);
  void show_menu(int);
  void print_screen(String, bool);

 private:
  Adafruit_ST7735 tft_;
  GFXcanvas1 canvas_;
};


#endif 