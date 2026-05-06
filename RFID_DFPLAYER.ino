#include <SPI.h>
#include <MFRC522.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

// ===== Pin Config =====
#define SDA_PIN 10
#define RST_PIN 9
#define BUZZER 4
#define LED_PIN 5

// DFPlayer
#define DF_RX 2
#define DF_TX 3

SoftwareSerial dfSerial(DF_RX, DF_TX);
DFRobotDFPlayerMini df;

MFRC522 rfid(SDA_PIN, RST_PIN);

// ===== State Machine =====
enum State {
  IDLE,
  READING,
  PROCESS
};

State currentState = IDLE;

// ===== Timing =====
unsigned long lastBeepTime = 0;
bool buzzerActive = false;

// ===== UID Buffer =====
char uidBuffer[20];

// ===== User List =====
const char *allowedUIDs[] = {
  "B3 0B 78 02",
  "11 22 33 44"
};

const int totalUsers = sizeof(allowedUIDs) / sizeof(allowedUIDs[0]);

// ===== scan =====
char lastUID[20] = "";
unsigned long lastScanTime = 0;
const unsigned long duplicateDelay = 2000;

// ===== RFID =====
void uidToString(MFRC522::Uid *uid, char *output) {
  output[0] = '\0';

  for (byte i = 0; i < uid->size; i++) {
    char buf[4];
    sprintf(buf, "%02X", uid->uidByte[i]);
    strcat(output, buf);

    if (i < uid->size - 1) strcat(output, " ");
  }
}

bool isAuthorized(char *uid) {
  for (int i = 0; i < totalUsers; i++) {
    if (strcmp(uid, allowedUIDs[i]) == 0) return true;
  }
  return false;
}

// ===== Beep =====
void beep(unsigned long duration = 100) {
  digitalWrite(BUZZER, HIGH);
  lastBeepTime = millis();
  buzzerActive = true;
}

void handleBeep() {
  if (buzzerActive && millis() - lastBeepTime >= 100) {
    digitalWrite(BUZZER, LOW);
    buzzerActive = false;
  }
}

// ===== Setup =====
void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();

  dfSerial.begin(9600);
  if (!df.begin(dfSerial)) {
    Serial.println("DFPlayer gagal!");
    while (true)
      ;
  }
  df.volume(20);

  pinMode(BUZZER, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  digitalWrite(BUZZER, LOW);
  digitalWrite(LED_PIN, LOW);

  Serial.println("RFID + DFPlayer Ready");
  df.play(2);  // track 1 
}

// ===== Loop =====
void loop() {
  handleBeep();

  switch (currentState) {

    case IDLE:
      if (!rfid.PICC_IsNewCardPresent()) return;
      if (!rfid.PICC_ReadCardSerial()) return;

      uidToString(&rfid.uid, uidBuffer);
      currentState = READING;
      break;

    case READING:
      Serial.print("UID: ");
      Serial.println(uidBuffer);

      beep();
      currentState = PROCESS;
      break;

    case PROCESS:

      if (strcmp(uidBuffer, lastUID) == 0 && millis() - lastScanTime < duplicateDelay) {
        Serial.println("Duplicate scan ignored");
        currentState = IDLE;
        return;
      }

      strcpy(lastUID, uidBuffer);
      lastScanTime = millis();

      if (isAuthorized(uidBuffer)) {
        Serial.println("Access GRANTED");
        digitalWrite(LED_PIN, HIGH);

        df.play(3);  // track 1 (akses diterima)

      } else {
        Serial.println("Access DENIED");
        digitalWrite(LED_PIN, LOW);

        df.play(1);  // track 2 (akses ditolak)
      }

      rfid.PICC_HaltA();
      currentState = IDLE;
      break;
  }
}