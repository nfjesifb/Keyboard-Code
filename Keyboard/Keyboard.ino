/*********************************************************************
 This is an example for our nRF52 based Bluefruit LE modules
 Pick one up today in the adafruit shop!
 Adafruit invests time and resources providing this open source code,
 please support Adafruit and open-source hardware by purchasing
 products from Adafruit!
 MIT license, check LICENSE for more information
 All text above, and the splash screen below must be included in
 any redistribution
*********************************************************************/
#include <bluefruit.h>
//Hi github
BLEDis bledis;
BLEHidAdafruit blehid;

bool hasKeyPressed = false;
int RowPin[5] = { 4, 3, 2, 5, 20};
int ColumnPin[12] = { 16, 12, 13, 14, 8, 6, 15, 7, 11, 27, 26, 25};
char Activation[2][5][12] = {
  
{{ HID_KEY_ESCAPE,HID_KEY_1,HID_KEY_2,HID_KEY_3,HID_KEY_4,HID_KEY_5,HID_KEY_6,HID_KEY_7,HID_KEY_8,HID_KEY_9,HID_KEY_0, HID_KEY_BACKSPACE } ,
{ HID_KEY_TAB,HID_KEY_Q,HID_KEY_W,HID_KEY_E,HID_KEY_R,HID_KEY_T,HID_KEY_Y,HID_KEY_U,HID_KEY_I,HID_KEY_O,HID_KEY_P,HID_KEY_MINUS } ,
{ HID_KEY_CAPS_LOCK, HID_KEY_A,HID_KEY_S, HID_KEY_D,HID_KEY_F,HID_KEY_G,HID_KEY_H,HID_KEY_J,HID_KEY_K,HID_KEY_L,HID_KEY_SEMICOLON, HID_KEY_RETURN } ,
{ HID_KEY_SHIFT_LEFT,HID_KEY_Z,HID_KEY_X, HID_KEY_C,HID_KEY_V, HID_KEY_B,HID_KEY_N,HID_KEY_M,HID_KEY_COMMA,HID_KEY_PERIOD,HID_KEY_BACKSLASH, HID_KEY_SHIFT_RIGHT } ,
{ HID_KEY_CONTROL_LEFT, HID_KEY_GUI_LEFT,HID_KEY_SPACE,HID_KEY_SPACE,HID_KEY_SPACE,HID_KEY_SPACE,HID_KEY_SPACE, HID_KEY_SPACE, HID_KEY_NONE, HID_KEY_ALT_RIGHT, HID_KEY_POWER }} ,

{{ HID_KEY_NONE, HID_KEY_NONE, HID_KEY_NONE, HID_KEY_NONE, HID_KEY_NONE, HID_KEY_NONE, HID_KEY_NONE, HID_KEY_NONE, HID_KEY_NONE, HID_KEY_NONE,HID_KEY_EQUAL, HID_KEY_NONE } ,
{ HID_KEY_NONE, HID_KEY_NONE, HID_KEY_NONE, HID_KEY_NONE, HID_KEY_NONE, HID_KEY_NONE, HID_KEY_NONE, HID_KEY_NONE, HID_KEY_NONE, HID_KEY_NONE, HID_KEY_NONE,HID_KEY_BACKSLASH } ,
{ HID_KEY_NONE, HID_KEY_NONE, HID_KEY_NONE, HID_KEY_NONE, HID_KEY_NONE, HID_KEY_NONE, HID_KEY_NONE, HID_KEY_NONE, HID_KEY_NONE, HID_KEY_NONE,HID_KEY_NONE, HID_KEY_NONE } ,
{ HID_KEY_NONE, HID_KEY_NONE, HID_KEY_NONE, HID_KEY_NONE, HID_KEY_NONE, HID_KEY_NONE, HID_KEY_NONE, HID_KEY_NONE,HID_KEY_NONE, HID_KEY_NONE,HID_KEY_BRACKET_LEFT, HID_KEY_BRACKET_RIGHT } ,
{ HID_KEY_NONE, HID_KEY_NONE, HID_KEY_NONE, HID_KEY_NONE, HID_KEY_NONE, HID_KEY_NONE, HID_KEY_NONE, HID_KEY_NONE, HID_KEY_NONE, HID_KEY_NONE, HID_KEY_NONE, HID_KEY_NONE }}

};
void setup() 
{
  Serial.begin(115200);
  while ( !Serial ) delay(10);   // for nrf52840 with native usb

  Serial.println("Bluefruit52 HID Keyboard Example");
  Serial.println("--------------------------------\n");

  Serial.println();
  Serial.println("Go to your phone's Bluetooth settings to pair your device");
  Serial.println("then open an application that accepts keyboard input");

  Serial.println();
  Serial.println("Enter the character(s) to send:");
  Serial.println();  

  Bluefruit.begin();
  // Set max power. Accepted values are: -40, -30, -20, -16, -12, -8, -4, 0, 4
  Bluefruit.setTxPower(4);
  Bluefruit.setName("Bluefruit52");

  // Configure and Start Device Information Service
  bledis.setManufacturer("Adafruit Industries");
  bledis.setModel("Bluefruit Feather 52");
  bledis.begin();

  /* Start BLE HID
   * Note: Apple requires BLE device must have min connection interval >= 20m
   * ( The smaller the connection interval the faster we could send data).
   * However for HID and MIDI device, Apple could accept min connection interval 
   * up to 11.25 ms. Therefore BLEHidAdafruit::begin() will try to set the min and max
   * connection interval to 11.25  ms and 15 ms respectively for best performance.
   */
  blehid.begin();

  // Set callback for set LED from central
  blehid.setKeyboardLedCallback(set_keyboard_led);

  /* Set connection interval (min, max) to your perferred value.
   * Note: It is already set by BLEHidAdafruit::begin() to 11.25ms - 15ms
   * min = 9*1.25=11.25 ms, max = 12*1.25= 15 ms 
   */
  /* Bluefruit.setConnInterval(9, 12); */
  
pinMode(20, OUTPUT);
pinMode(2, OUTPUT);
pinMode(3, OUTPUT);
pinMode(4, OUTPUT);
pinMode(5, OUTPUT);
pinMode(12, INPUT_PULLUP);
pinMode(13, INPUT_PULLUP);
pinMode(14, INPUT_PULLUP);
pinMode(8, INPUT_PULLUP);
pinMode(6, INPUT_PULLUP);
pinMode(25, INPUT_PULLUP);
pinMode(26, INPUT_PULLUP);
pinMode(27, INPUT_PULLUP);
pinMode(11, INPUT_PULLUP);
pinMode(7, INPUT_PULLUP);
pinMode(15, INPUT_PULLUP);
pinMode(16, INPUT_PULLUP);

  // Set up and start advertising
  startAdv();
}

void startAdv(void)
{  
  // Advertising packet
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();
  Bluefruit.Advertising.addAppearance(BLE_APPEARANCE_HID_KEYBOARD);
  
  // Include BLE HID service
  Bluefruit.Advertising.addService(blehid);

  // There is enough room for the dev name in the advertising packet
  Bluefruit.Advertising.addName();
  
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

void loop() 
{
  int RightKeyFlag;
  for ( int RowCount = 0; RowCount < 5; RowCount++)
  {
    digitalWrite(RowPin[RowCount], HIGH);
    for ( int ColumnCount = 0; ColumnCount < 12; ColumnCount++)
    {
      if ( digitalRead(ColumnPin[ColumnCount]))
      {  
        blehid.keyPress('q');
         blehid.keyRelease('q');
     }
   }
  }
}
/**
 *
 * Callback invoked when received Set LED from central.
 * Must be set previously with setKeyboardLedCallback()
 *
 * The LED bit map is as follows: (also defined by KEYBOARD_LED_* )
 *    Kana (4) | Compose (3) | ScrollLock (2) | CapsLock (1) | Numlock (0)
 */
void set_keyboard_led(uint8_t led_bitmap)
{
  // light up Red Led if any bits is set
  if ( led_bitmap )
  {
    ledOn( LED_RED );
  }
  else
  {
    ledOff( LED_RED );
  }
}
