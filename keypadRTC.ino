#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
#include "Arduino.h"
#include "uRTCLib.h"
uRTCLib rtc(0x68);

enum Mode {
  HOME,
  SET
};

Mode mode = HOME;

const byte ROWS = 4;  //four rows
const byte COLS = 3;  //four columns
//define the cymbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
  { '1', '2', '3' },
  { '4', '5', '6' },
  { '7', '8', '9' },
  { '*', '0', '#' }
};
byte rowPins[ROWS] = { 2, 3, 4, 5 };  //connect to the row pinouts of the keypad
byte colPins[COLS] = { 6, 7, 8 };     //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

char buf[17];
const char* hari[7] = { "Ming", "Sen", "Sel", "Rab", "Kam", "Jum", "Sab" };

int main() {
  init();
  URTCLIB_WIRE.begin();
  lcd.init();
  lcd.backlight();
  customKeypad.addEventListener(keypadEvent);
  // rtc.set(0, 49, 9, 2, 29, 4, 25);

  while (1) {
    rtc.refresh();
    customKeypad.getKey();

    if (mode == HOME) {
      tampil_home();
    } else if (mode == SET) {
      tampil_set();
    }
  }  // kurawal while loop

  return 0;
}

void keypadEvent(KeypadEvent eKey) {
  switch (customKeypad.getState()) {
    case HOLD:
      if (eKey == '*') {
        mode = SET;
      }
      break;
  }
}

void tampil_home() {
  lcd.setCursor(0, 0);
  snprintf(buf, sizeof(buf), "Tgl: %s-%d/%d/%d", hari[rtc.dayOfWeek() - 1], rtc.day(), rtc.month(), rtc.year());
  lcd.print(buf);
  lcd.setCursor(0, 1);
  snprintf(buf, sizeof(buf), "Jam: %.2d:%.2d:%.2d", rtc.hour(), rtc.minute(), rtc.second());
  lcd.print(buf);
  _delay_ms(100);
}

void tampil_set() {
  lcd.clear();
  lcd.print("MASUK MENU");
  _delay_ms(200);
  lcd.clear();
  mode = HOME;
}
