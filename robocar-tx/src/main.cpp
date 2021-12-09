#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include "Adafruit_miniTFTWing.h"
#include <InternalFileSystem.h>
#include "../lib/Bluefruit52Lib/src/services/BLEDfu.h"
#include "../lib/Bluefruit52Lib/src/services/BLEDis.h"
#include "../lib/Bluefruit52Lib/src/services/BLEBas.h"
#include "../lib/Bluefruit52Lib/src/services/BLEUart.h"
#include "../lib/Bluefruit52Lib/src/bluefruit.h"
#include <BLEAdvertising.h>


// BLE Service
BLEDfu  bledfu;  // OTA DFU service
BLEDis  bledis;  // device information
BLEUart bleuart; // uart over ble
BLEBas  blebas;  // battery

#define CH3_TRAIN 682
#define CH3_AUTO 341
#define WIPE_SECONDS 5
#define LOOP_DELAY 100

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
              tft_b, ch_4_switch };

enum mode { autonomous, manual, training };

bool is_active;
uint8_t mode;
String status;

Adafruit_miniTFTWing ss;
Adafruit_ST7735 tft = Adafruit_ST7735(pin::tft_cs, pin::tft_dc, pin::tft_rst);

String id_str = "EF:EB:FD:C7:F8:DA";
String mode_string[] = { "Autonomous", "Manual", "Data"};
char state_string[3][2][20] = { {"Stopped", "Driving"},  {"Disarmed", "Armed"}, {"Paused", "Recording"}  };

int current_mode;

void startAdv(void);
void connect_callback(uint16_t);
void disconnect_callback(uint16_t, uint8_t);
void print_scroll(String);
void print_screen(String, bool);
void processState(int);
void processMode(int);
void print_status(bool, bool, bool);


float norm_axis(long, long, long, float, float);
byte norm_byte(float, float, float);
float apply_exp(float, float);


void setup() {
  // set input pins
  pinMode(pin::ch_4, INPUT);
  pinMode(pin::st_rev, INPUT_PULLUP);
  pinMode(pin::th_rev, INPUT_PULLUP);

  Serial.begin(115200);

  // scroll_called = 0;

  // tft wing
  if(!ss.begin()) {
    Serial.println("seesaw couldn't be found!");
    while(1);
  }
  // Serial.println("seesaw started");
  ss.tftReset();
  ss.setBacklight(TFTWING_BACKLIGHT_ON);
  tft.initR(INITR_MINI160x80);
  tft.cp437(true);
  // Serial.println("TFT initialized");
  tft.setRotation(1); // USB jack on the right, joystick on the right
  tft.fillScreen(ST77XX_BLACK);
  delay(100);
  // print_test();

  // Setup the BLE LED to be enabled on CONNECT
  // Note: This is actually the default behavior, but provided
  // here in case you want to control this LED manually via PIN 19
  Bluefruit.autoConnLed(true);

  // Config the peripheral connection with maximum bandwidth 
  // more SRAM required by SoftDevice
  // Note: All config***() function must be called before begin()
  Bluefruit.configPrphBandwidth(BANDWIDTH_MAX);

  Bluefruit.begin();
  Bluefruit.setTxPower(4);    // Check bluefruit.h for supported values
  Bluefruit.setName(getMcuUniqueID()); // useful testing with multiple central connections
  Bluefruit.Periph.setConnectCallback(connect_callback);
  Bluefruit.Periph.setDisconnectCallback(disconnect_callback);

  // To be consistent OTA DFU should be added first if it exists
  bledfu.begin();

  // Configure and Start Device Information Service
  bledis.setManufacturer("Adafruit Industries");
  bledis.setModel("Bluefruit Feather52");
  bledis.begin();

  // Configure and Start BLE Uart Service
  bleuart.begin();

  // Start BLE Battery Service
  blebas.begin();
  blebas.write(100);

  // Set up and start advertising
  startAdv();

  // Serial.println("Please use Adafruit's Bluefruit LE app to connect in UART mode");
  // Serial.println("Once connected, enter character(s) that you wish to send");
  print_scroll("Dev ID : " + id_str);
  
  
  current_mode = mode::manual;
  is_active = false; // state_string[current_mode][false];
  print_status(true, true, true);
}

void startAdv(void)
{
  // Advertising packet
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();

  // Include bleuart 128-bit uuid
  Bluefruit.Advertising.addService(bleuart);

  // Secondary Scan Response packet (optional)
  // Since there is no room for 'Name' in Advertising packet
  Bluefruit.ScanResponse.addName();
  
  /* Start Advertising
   * - Enable auto advertising if disconnected
   * - Interval:  fast mode = 20 ms, slow mode = 152.5 ms
   * - Timeout for fast mode is 30 seconds
   * - Start(timeout) with timeout = 0 will advertise forever (until connected)
   * 
   * For recommended advertising interval
   * https://developer.apple.com/library/content/qa/qa1931/_index.html   
   */
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);    // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
  Bluefruit.Advertising.start(0);                // 0 = Don't stop advertising after n seconds  
}


void loop() {

  // poll analog
  int ch_3_in = analogRead(pin::ch_3);
  int st_in = analogRead(pin::st);
  int st_exp_in = analogRead(pin::st_dr);
  int st_trm_in = analogRead(pin::st_trm) - 511;
  int th_in = analogRead(pin::th);
  int th_exp_in = analogRead(pin::th_dr);
  int th_trm_in = analogRead(pin::th_trm) - 511;

  // poll digital
  int ch_4_in = digitalRead(pin::ch_4);
  bool st_rev = digitalRead(pin::st_rev) == LOW;
  bool th_rev = digitalRead(pin::th_rev) == LOW;
  uint32_t ss_btns = ss.readButtons();


  // process analog

  // apply trim
  st_in += st_trm_in;
  th_in += th_trm_in;

  // norm axes
  float st_val = norm_axis(st_in, -st_trm, 1023+st_trm, -1.0, 1.0);
  float th_val = norm_axis(th_in, -th_trm, 1023+th_trm, -1.0, 1.0);
  float st_exp_val = float(st_exp_in) / 1023.0;
  float th_exp_val = float(th_exp_in) / 1023.0;

  // reverse 
  st_val = st_rev ? -1.0 * st_val : st_val;
  th_val = th_rev ? -1.0 * th_val : th_val;

  // apply exponent mod (TEST)
  st_val = apply_exp(st_val, st_exp_val);
  th_val = apply_exp(th_val, th_exp_val);

  // convert button presses to byte
  byte btns_out = 0;
  byte ch_3_out = norm_byte(ch_3_in, 0, 1023);
  byte st_out = norm_byte(st_val, -1.0, 1.0);
  byte th_out = norm_byte(th_val, -1.0, 1.0);

  if (ch_4_in == LOW) {
    btns_out |= 1 << button::ch_4_switch;
  } 
  // SCREEN ROTATED SO IT'S ALL REVERSED
  if (! (ss_btns & TFTWING_BUTTON_LEFT)) {
    btns_out |= 1 << button::joy_right;
    // print_scroll("RIGHT");
  }
  if (! (ss_btns & TFTWING_BUTTON_RIGHT)) {
    btns_out |= 1 << button::joy_left;
    // print_scroll("LEFT");
  }
  if (! (ss_btns & TFTWING_BUTTON_DOWN)) {
    btns_out |= 1 << button::joy_up;
    // print_scroll("UP");
  }
  if (! (ss_btns & TFTWING_BUTTON_UP)) {
    btns_out |= 1 << button::joy_down;
    // print_scroll("DOWN");  
  }
  if (! (ss_btns & TFTWING_BUTTON_A)) {
    btns_out |= 1 << button::tft_a;
    // print_scroll("A");
  }
  if (! (ss_btns & TFTWING_BUTTON_B)) {
    btns_out |= 1 << button::tft_b;
    // print_scroll("B");
  }
  if (! (ss_btns & TFTWING_BUTTON_SELECT)) {
    btns_out |= 1 << button::joy_select;
    // print_scroll("select");
  }

  // send to car
  uint8_t buf[8];
  buf[0] = 'c';
  buf[1] = st_out;
  buf[2] = ',';
  buf[3] = th_out;
  buf[4] = ',';
  buf[5] = ch_3_out;
  buf[6] = ',';
  buf[7] = btns_out;

  bleuart.write(buf, 8);

  // avert your eyes!
  processMode(ch_3_in);
  processState(ch_4_in);

  // How much?
  delay(LOOP_DELAY);
}

void processMode(int mode_switch) {
  uint8_t new_mode;
  bool clear_mode;
  if (mode_switch > CH3_TRAIN) {
    new_mode = mode::training;
  } else if (mode_switch < CH3_AUTO) {
    new_mode = mode::autonomous;
  } else {
    new_mode = mode::manual;
  }
  clear_mode = new_mode != current_mode;
  current_mode = new_mode;
  print_status(clear_mode, clear_mode, false);
  clear_mode = false;
}

void processState(int state_switch) {
  uint8_t new_active;
  bool clear_state = false;
  if (state_switch == LOW) {
    new_active = true;
  } else {
    new_active = false;
  }
  clear_state = new_active != is_active;
  is_active = new_active;
  print_status(false, clear_state, false);
  clear_state = false;
}

void print_status(bool reset_mode, bool reset_state, bool reset_screen) {
  if (reset_screen) {
    tft.fillRect(0, 18, tft.width(), 41, ST77XX_BLACK);
    tft.setCursor(0,18);
    tft.println("Mode:  " + mode_string[current_mode] + "\n");    
    tft.print("State: ");
    tft.print(is_active ? state_string[current_mode][is_active] : state_string[current_mode][is_active]);
  }
  else 
  { 
    if (reset_mode) {
      tft.fillRect(30, 18, tft.width(), 25, ST77XX_BLACK);
      tft.setCursor(0, 18);
      tft.print("Mode:  " + mode_string[current_mode]);
      reset_state = true;
    } 
    if (reset_state) {
      tft.fillRect(30, 34, tft.width(), 42, ST77XX_BLACK);
      tft.setCursor(0, 34);
      tft.print("State: ");
      tft.print(is_active ? state_string[current_mode][is_active] : state_string[current_mode][is_active]);
    }
  }
}

void print_screen(String new_str, bool refresh) {
  if (refresh) {
    tft.fillScreen(ST77XX_BLACK);
    tft.setCursor(0,0);
    tft.setTextWrap(true);
  }
  tft.setCursor(0,20);
  tft.print(new_str);
}

// callback invoked when central connects
void connect_callback(uint16_t conn_handle)
{
  // Get the reference to current connection
  BLEConnection* connection = Bluefruit.Connection(conn_handle);

  char central_name[32] = { 0 };
  connection->getPeerName(central_name, sizeof(central_name));

  Serial.print("Connected to ");
  Serial.println(central_name);
  print_screen("Connected:" + String(central_name), true);
  // print_scroll("Connected: ");
  // print_scroll("  " + String(central_name));
}

/**
 * Callback invoked when a connection is dropped
 * @param conn_handle connection where this event happens
 * @param reason is a BLE_HCI_STATUS_CODE which can be found in ble_hci.h
 */
void disconnect_callback(uint16_t conn_handle, uint8_t reason)
{
  (void) conn_handle;
  (void) reason;

  // Serial.println();
  // Serial.print("Disconnected, reason = 0x"); 
  // Serial.println(reason, HEX);
  // print_scroll("");
  print_scroll("Disconnected:"); 
  print_scroll(" reason = 0x" + String(reason, HEX));
  is_active = false;
  print_screen("Dev ID : " + id_str, true);
}



void print_scroll(String new_str) {
  static int scroll_size = 9;
  static int scroll_called;
  static String scroll_strs[] = {"", "", "", "", "", "", "", "", ""};
  tft.setTextWrap(false);
  tft.setCursor(0, 0);
  tft.setTextColor(ST7735_WHITE, ST77XX_BLACK);  // should be grey
  tft.setTextSize(1);
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
    tft.print(scroll_strs[i_print]);
    if (tft.getCursorX() < tft.width()) {
      tft.fillRect(tft.getCursorX(), tft.getCursorY(), 
                   tft.width() - tft.getCursorX(), 16, ST77XX_BLACK);
    }
    tft.print('\n');
  }
}



float norm_axis(long x, long in_min, long in_max, float out_min, float out_max) {
  return float(x - in_min) * (out_max - out_min) / float(in_max - in_min) + out_min;
}

byte norm_byte(float x, float in_min, float in_max) {
  return (x - in_min) * 255 / (in_max - in_min);
}

// rcgroups.com/forums/showthread.php?1675540-who-know-the-algorithm-for-exponential-curve-in-RC
float apply_exp(float x, float a) {
  return a * pow(x, 3) + (1.0 - a) * x;
}