#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
#include <RTClib.h>
RTC_DS3231 rtc;
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
char baris1[17];
char baris2[17];

const char *hari[] = { "min", "Sen", "Sel", "Rab", "Kam", "Jum", "Sab" };
const char *bulan[] = { "Jan", "Feb", "Mar", "Apr", "Mei", "Jun", "Jul", "Agu", "Sep", "Okt", "Nov", "Des" };
char key;

uint8_t set_menit = 0;

int main() {
  init();
  if (!rtc.begin()) {
    lcd.setCursor(0, 0);
    lcd.print("RTC Error");
    while (1) delay(10);
  }
  lcd.clear();

  if (rtc.lostPower()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  lcd.init();
  lcd.backlight();
  customKeypad.addEventListener(keypadEvent);


  while (1) {
    customKeypad.getKey();
    DateTime now = rtc.now();
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
  DateTime now = rtc.now();
  snprintf(baris1, size_t(baris1), "%s, %02d-%s-%04d", hari[now.dayOfTheWeek()], now.day(), bulan[now.month() - 1], now.year());
  snprintf(baris2, size_t(baris2), "%02d:%02d:%02d", now.hour(), now.minute(), now.second());
  lcd.setCursor(0, 0);
  lcd.print(baris1);
  lcd.setCursor(4, 1);
  lcd.print(baris2);
}

void tampil_set() {
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("SETTING ALARM");

  do {
    key = customKeypad.getKey();
    DateTime now = rtc.now();
    if (key >= '0' && key <= '9')
      set_menit = set_menit * 10 + (key - '0');
    if (key == '#') set_menit = 0;
    lcd.setCursor(0, 1);
    snprintf(buf, sizeof(buf), "Jam: %.2d:%.2d:", now.hour(), set_menit);
    lcd.print(buf);

  } while (key != '*');

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("SAVING MENIT");
  for (byte i = 0; i < 16; i++) {
    lcd.setCursor(i, 1);
    lcd.print("=");
    _delay_ms(20);
  }
  lcd.clear();
  mode = HOME;
}
