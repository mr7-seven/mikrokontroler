#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
#include <RTClib.h>
RTC_DS3231 rtc;
#define set_high(port, pin) port |= (1 << pin)
#define set_low(port, pin) port &= ~(1 << pin)

enum Mode {
  HOME,
  SET
};

enum SetState {
  SET_IDLE,
  SET_MINUTE,
  SET_SECOND
};

SetState setState = SET_IDLE;

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

uint8_t set_menit, set_detik = 0;
DateTime now;

unsigned long lastRTC = 0;
unsigned long lastLCD = 0;
unsigned long alarmStart = 0;
bool alarmActive = false;
int lastSecond = -1;
int lastCountdown = -1;

int main() {
  init();
  lcd.init();
  lcd.backlight();

  if (!rtc.begin()) {
    lcd.setCursor(0, 0);
    lcd.print("RTC Error");
    while (1) delay(10);
  }
  lcd.clear();

  DDRB |= (1 << PB1) | (1 << PB2);
  customKeypad.addEventListener(keypadEvent);
  // customKeypad.setDebounceTime(10);
  // customKeypad.setHoldTime(500);
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  while (1) {

    key = customKeypad.getKey();
    if(key) {
      beep();
    }
    // update RTC
    if (millis() - lastRTC >= 100) {
      lastRTC = millis();
      now = rtc.now();
    }

    // ===== trigger alarm) =====
    if (!alarmActive && now.minute() == set_menit && now.second() == set_detik) {

      alarmActive = true;
      alarmStart = millis();
      lastCountdown = -1;
    }

    // ===== jalankan alarm =====
    if (alarmActive) {

      int sisa = 10 - (millis() - alarmStart) / 1000;
      if (sisa < 0) sisa = 0;

      if (sisa != lastCountdown) {
        lastCountdown = sisa;

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("!!! ALARM !!!");

        lcd.setCursor(0, 1);
        lcd.print("Sisa: ");
        lcd.print(sisa);
        lcd.print(" detik ");
      }

      // buzzer kedip
      if ((millis() / 200) % 2 == 0) {
        set_high(PORTB, PB1);
      } else {
        set_low(PORTB, PB1);
      }

      // stop setelah 10 detik
      if (millis() - alarmStart >= 10000) {
        alarmActive = false;
        set_low(PORTB, PB1);
        lcd.clear();
      }

      continue;
    }

    // ===== MODE NORMAL =====
    if (mode == HOME) {
      tampil_home();
    } else if (mode == SET) {
      tampil_set(key);
    }
  }  // kurawal while loop

  return 0;
}

void keypadEvent(KeypadEvent eKey) {
  switch (customKeypad.getState()) {
    case PRESSED:

      if (alarmActive && key == '#') {
        alarmActive = false;
        set_low(PORTB, PB1);
        lcd.clear();
      }
      break;
    case HOLD:
      if (eKey == '*') {
        mode = SET;
        setState = SET_MINUTE;
        set_menit = 0;
        set_detik = 0;
        lcd.clear();
      }
      break;
  }
}

void tampil_home() {
  if (now.second() == lastSecond) return;
  lastSecond = now.second();
  snprintf(baris1, size_t(baris1), "%s, %02d-%s-%04d", hari[now.dayOfTheWeek()], now.day(), bulan[now.month() - 1], now.year());
  snprintf(baris2, size_t(baris2), "%02d:%02d:%02d", now.hour(), now.minute(), now.second());
  lcd.setCursor(0, 0);
  lcd.print(baris1);
  lcd.setCursor(4, 1);
  lcd.print(baris2);
}

void tampil_set(char key) {

  lcd.setCursor(0, 0);
  lcd.print("SETTING ALARM ");

  // ===== INPUT MENIT =====
  if (setState == SET_MINUTE) {

    if (key >= '0' && key <= '9') {
      set_menit = (set_menit * 10 + (key - '0')) % 60;
    }

    if (key == '#') set_menit = 0;

    if (key == '*') {
      setState = SET_SECOND;
    }

    snprintf(buf, sizeof(buf), "Jam:__:%02d:__", set_menit);
  }

  // ===== INPUT DETIK =====
  else if (setState == SET_SECOND) {

    if (key >= '0' && key <= '9') {
      set_detik = (set_detik * 10 + (key - '0')) % 60;
    }

    if (key == '#') set_detik = 0;

    if (key == '*') {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("SAVING...");
      for (byte i = 0; i < 16; i++) {
        lcd.setCursor(i, 1);
        lcd.print("-");
        _delay_ms(10);
      }
      mode = HOME;
      setState = SET_IDLE;
      lcd.clear();
      return;
    }

    snprintf(buf, sizeof(buf), "Jam:__:%02d:%02d", set_menit, set_detik);
  }

  lcd.setCursor(0, 1);
  lcd.print(buf);
}

void beep() {
  set_high(PORTB, PB1);
  _delay_ms(10);
  set_low(PORTB, PB1);
}
