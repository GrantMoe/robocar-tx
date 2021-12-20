#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <bluefruit.h>

#include "Adafruit_miniTFTWing.h"
#include "Axis.h"
#include "tx.h"
// #include "Tx_tft.h"

BLEDis  bledis;
BLEHidGamepad blegamepad;

// defined in hid.h from Adafruit_TinyUSB_Arduino
hid_gamepad_report_t gp;

Adafruit_miniTFTWing ss;
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS_PIN, TFT_DC_PIN, TFT_RST_PIN);
// GFXcanvas1 canvas(tft.width(), tft.height());
// Tx_tft tx_display(tft, canvas);

// in, trim, exp, rev
Axis steering(ST_PIN, ST_TRM_PIN, ST_EXP_PIN, ST_REV_PIN);
Axis throttle(TH_PIN, TH_TRM_PIN, TH_EXP_PIN, TH_REV_PIN);

String id_str = "EF:EB:FD:C7:F8:DA";
int current_mode;
int previous_mode;

// TODO: callbacks can be useful for hand unit display, actually.
void connect_callback(uint16_t);
void disconnect_callback(uint16_t, uint8_t);
void startAdv(void);
int read_slider(int);
void show_menu(int);
void print_scroll(String);

const unsigned long tft_buttons[] = {
  TFTWING_BUTTON_DOWN,
  TFTWING_BUTTON_UP,
  TFTWING_BUTTON_RIGHT,
  TFTWING_BUTTON_LEFT,
  TFTWING_BUTTON_SELECT,
  TFTWING_BUTTON_A,
  TFTWING_BUTTON_B
};

const String menu_strings_[3][3] = {
  {"", "", "Drive"},
  {"confirm", "cancel", "Cal."},
  {"", "calibrate", "Pause"}
};

bool pressed[NUM_BUTTONS];
unsigned long last_pressed[NUM_BUTTONS];


void setup() {

  for (int i =0; i < NUM_BUTTONS; i++) {
    pressed[i] = false;
    last_pressed[i] = millis();
  }
  // set reverse switch pullups
  // do in axis instead?
  pinMode(ST_REV_PIN, INPUT_PULLUP);
  pinMode(TH_REV_PIN, INPUT_PULLUP);
  pinMode(CH_4_PIN, INPUT);

  Serial.begin(9600);

  //---------
  // TFT Wing
  //---------
  if(!ss.begin()) {
    Serial.println("seesaw couldn't be found!");
    while(1);
  }
  Serial.println("seesaw started");
  ss.tftReset();
  ss.setBacklight(TFTWING_BACKLIGHT_ON);

  tft.initR(INITR_MINI160x80);
  tft.cp437(true);
  tft.setRotation(1); // reverse for controller config
  tft.fillScreen(ST77XX_BLACK);
  // delay(200);
  Serial.println("TFT initialized");
  

  // ---------
  // BlueFruit
  // ---------
  // Config the peripheral connection with maximum bandwidth 
  // more SRAM required by SoftDevice
  // Note: All config***() function must be called before begin()
  // Bluefruit.configPrphBandwidth(BANDWIDTH_MAX);
  Bluefruit.begin();
  Bluefruit.setTxPower(4);    // Check bluefruit.h for supported values

  // Bluefruit.setName(getMcuUniqueID()); // useful testing with multiple central connections
  Bluefruit.Periph.setConnectCallback(connect_callback);
  Bluefruit.Periph.setDisconnectCallback(disconnect_callback);

  // Configure and Start Device Information Service
  bledis.setManufacturer("Adafruit Industries");
  bledis.setModel("Bluefruit Feather52");
  bledis.begin();

  // Configure BLE HID
  blegamepad.begin();

  /* Set connection interval (min, max) to your perferred value.
   * Note: It is already set by BLEHidAdafruit::begin() to 11.25ms - 15ms
   * min = 9*1.25=11.25 ms, max = 12*1.25= 15 ms
   */
  /* Bluefruit.Periph.setConnInterval(9, 12); */

  // Set up and start advertising
  startAdv();
  Serial.println("started advertising");
  print_scroll("Dev ID : " + id_str);

  previous_mode = 999;
  current_mode = DRIVE_MODE;
}

void startAdv(void)
{
  // Advertising packet
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();
  Bluefruit.Advertising.addAppearance(BLE_APPEARANCE_HID_GAMEPAD);
  Bluefruit.Advertising.addService(blegamepad);
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
  // Serial.println("loop!");
  // display menu

    // handle sliders
  int steering_out = NEUTRAL_OUT;
  int throttle_out = NEUTRAL_OUT;
  int ch3_in = analogRead(CH_3_PIN);
  int ch3_out = read_slider(ch3_in);
  int ch4_in = digitalRead(CH_4_PIN);
  int ch4_out = 0;
  if (ch4_in == HIGH) {
    ch4_out = SLIDER_HIGH;
  } else {
    ch4_out = SLIDER_LOW;
  }

  unsigned long time_ms = millis();
  int ss_btns = ss.readButtons();
  for (int i = 0; i < NUM_BUTTONS; i++) {
    pressed[i] = false;
    if (! (ss_btns & tft_buttons[i])) {
      if (time_ms - last_pressed[i] > COOLDOWN) {
        pressed[i] = true;
        last_pressed[i] = time_ms;
      }
    }
  }
  
  switch(current_mode) {
  case DRIVE_MODE:
      // check if pausing 
    if (ch4_in == HIGH) {
      current_mode = PAUSE_MODE;
    } else {
      // read axes
      steering_out = steering.getOuput();
      throttle_out = throttle.getOuput();
    }
    break;
  case CALIBRATE_MODE:
    if (pressed[button::tft_b] == true) {
    // if (! (ss_btns & TFTWING_BUTTON_B)){
      // confirm calibration, apply new values
      steering.applyCalibration();
      throttle.applyCalibration();
      current_mode = PAUSE_MODE;
    } else if (pressed[button::tft_a] == true) {
    // } else if (! (ss_btns & TFTWING_BUTTON_A)) {
      // end calibrate mode without apply calibration
      current_mode = PAUSE_MODE;
    } else {
      // continue calibrating
      steering.calibrate();
      throttle.calibrate();
    }
    break;
  default: // pause mode
    if (pressed[button::tft_a] == true){
    // if (! (ss_btns & TFTWING_BUTTON_A)){
      // start calibration as ordered
      current_mode = CALIBRATE_MODE;
      steering.startCalibration();
      throttle.startCalibration();
    } else {
    // don't start drive and calibration at the same time
      if (ch4_in == LOW) {
        current_mode = DRIVE_MODE;
      }
    }
  } 

  // transmit report
  gp.x = steering_out;
  gp.y = throttle_out;
  gp.rx = ch3_out;
  gp.ry = ch4_out;
  gp.buttons = ss_btns;

  blegamepad.report(&gp);

  if (current_mode != previous_mode) {
    show_menu(current_mode);
  }
  previous_mode = current_mode;
}


int read_slider(int val) {
  if (val > SLIDER_HIGH) {
    return MAX_OUT;
  } else if (val < SLIDER_LOW) {
    return MIN_OUT;
  } else {
    return NEUTRAL_OUT;
  }
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
  print_scroll("Connected: ");
  print_scroll("  " + String(central_name));
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
  Serial.println();
  Serial.print("Disconnected, reason = 0x"); 
  Serial.println(reason, HEX);
  print_scroll("");
  print_scroll("Disconnected:"); 
  print_scroll(" reason = 0x" + String(reason, HEX));
  print_scroll("\n Dev ID :" + id_str); // in case you need to re-enter
}

void show_menu(int mode) {
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(0, 10);
  tft.print(menu_strings_[mode][0]);
  tft.setCursor(0, 60);
  tft.print(menu_strings_[mode][1]);
  tft.setCursor(20, 27);
  tft.setTextSize(3);
  tft.print(menu_strings_[mode][2]);
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