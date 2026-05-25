#include <NeoSWSerial.h>
#include "BluetoothHandler.h"

#define RX_PIN 2
#define TX_PIN 3

NeoSWSerial BT(RX_PIN, TX_PIN);

char buffer[32];
uint8_t indexBuffer = 0;

#define LED_PIN 11
#define BUZZER_PIN 12

const byte segPins[7] = { 4, 5, 6, 7, 8, 9, 10 };

bool buzzerEnable = false;
bool buzzerState = false;

unsigned long previousMillis = 0;
const long interval = 300;

int main() {
  init();
  Serial.begin(9600);
  BT.begin(38400);

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  for (byte i = 0; i < 7; i++) {
    pinMode(segPins[i], OUTPUT);
  }

  tampilAngka('-');

  Serial.println("System Ready");

  while (1) {
    bacaBluetooth();

    kontrolBuzzer();
  }
  return 0;
}

void prosesPerintah(char *cmd) {

  if (strcmp(cmd, "TURN ON THE LIGHT") == 0) {

    digitalWrite(LED_PIN, HIGH);
    Serial.println("LED ON");
  }

  else if (strcmp(cmd, "TURN OFF THE LIGHT") == 0) {

    digitalWrite(LED_PIN, LOW);
    Serial.println("LED OFF");
  }

  else if (strcmp(cmd, "TURN ON THE ALARM") == 0) {

    buzzerEnable = true;
    Serial.println("BUZZER ON");
  }

  else if (strcmp(cmd, "TURN OFF THE ALARM") == 0) {

    buzzerEnable = false;

    digitalWrite(BUZZER_PIN, LOW);
    Serial.println("BUZZER OFF");
  }

  else if (strcmp(cmd, "SHOW NUMBER ONE") == 0) {

    tampilAngka('1');
    Serial.println("SHOW NUMBER 1");
  }

  else if (strcmp(cmd, "SHOW NUMBER TWO") == 0) {

    tampilAngka('2');
    Serial.println("SHOW NUMBER 2");
  }

  else if (strcmp(cmd, "SHOW NUMBER THREE") == 0) {

    tampilAngka('3');
    Serial.println("SHOW NUMBER 3");
  }

  else {

    Serial.println("Perintah tidak valid");
    tampilAngka('-');
  }
}

void kontrolBuzzer() {

  if (!buzzerEnable) {
    return;
  }

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {

    previousMillis = currentMillis;

    buzzerState = !buzzerState;

    digitalWrite(BUZZER_PIN, buzzerState);
  }
}

void tampilAngka(char value) {

  const byte angka[10] = {

    0x7E,  // 0
    0x30,  // 1
    0x6D,  // 2
    0x79,  // 3
    0x33,  // 4
    0x5B,  // 5
    0x5F,  // 6
    0x70,  // 7
    0x7F,  // 8
    0x7B   // 9
  };

  byte polaAngka = 0x01;  // '-'

  if (value >= '0' && value <= '9') {

    polaAngka = angka[value - '0'];
  }

  for (byte i = 0; i < 7; i++) {

    digitalWrite(segPins[i], bitRead(polaAngka, 6 - i));
  }
}