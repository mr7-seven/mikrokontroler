#include <Password.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x25, 16, 2);
#include <EEPROM.h>
Password password = Password("1234");
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

bool pb, last_pb;
bool ledStatus = 0;
int data = 0;
byte pos = 3;

int main() {
  init();
  lcd.init();
  lcd.backlight();
  DDRD &= ~(1 << PD2);
  PORTD |= (1 << PD2);
  DDRD |= (1 << PD1);

  data = EEPROM.read(0);
  customKeypad.addEventListener(keypadEvent);  ///< add an event listener for this keypad

  while (1) {
    customKeypad.getKey();

    lcd.setCursor(0, 0);
    lcd.print("MASUKKAN PIN");

    pb = !(PIND & (1 << PD2));

    if (pb && !last_pb) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Simpan Data: ");
      lcd.print(data);
      EEPROM.update(0, data);
      _delay_ms(500);
      lcd.clear();
    }
    last_pb = pb;


    // PORTD = ledStatus ? PORTD | (1<<PD1): PORTD & ~(1<<PD1);
  }
  return 0;
}

void keypadEvent(KeypadEvent eKey) {
  switch (customKeypad.getState()) {
    case PRESSED:
      lcd.setCursor(pos++, 1);
      lcd.print(eKey);
      switch (eKey) {
        case '*': checkPassword(); break;
        case '#': password.reset(); break;
        default: password.append(eKey);
      }
  }
}

void checkPassword() {
  if (password.evaluate()) {
    Serial.println("Success");
    //Add code to run if it works
  } else {
    Serial.println("Wrong");
    //add code to run if it did not work
  }
}