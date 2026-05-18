#include <ServoEasing.hpp>

const byte PAN_PIN = 9;
const byte TILT_PIN = 10;

const byte X_PIN = A1;
const byte Y_PIN = A0;

// LDR & BUZZER
const byte LDR_PIN = A2;
const byte BUZZER_PIN = 2;

const int PAN_MIN = 20;
const int PAN_MAX = 160;

const int TILT_MIN = 45;
const int TILT_MAX = 135;

// JOYSTICK FILTER
float xFiltered = 512.0f;
float yFiltered = 512.0f;

const float FILTER_ALPHA = 0.10f;

const int DEADZONE = 50;


// SERVO THRESHOLD
const float SERVO_THRESHOLD = 3.0f;

ServoEasing panServo;
ServoEasing tiltServo;

float lastPanAngle = 90.0f;
float lastTiltAngle = 90.0f;

unsigned long lastRead = 0;

const uint16_t READ_INTERVAL = 20;

// sesuaikan dengan kondisi nyata
const int LDR_THRESHOLD = 750;

bool buzzerState = false;

unsigned long lastBuzzerToggle = 0;

const uint16_t BUZZER_INTERVAL = 150;

int main() {
  Serial.begin(115200);

  panServo.attach(PAN_PIN, 90);
  tiltServo.attach(TILT_PIN, 90);

  panServo.setEasingType(EASE_LINEAR);
  tiltServo.setEasingType(EASE_LINEAR);

  panServo.setSpeed(120);
  tiltServo.setSpeed(120);

  panServo.startEaseTo(90);
  tiltServo.startEaseTo(90);

  pinMode(BUZZER_PIN, OUTPUT);

  digitalWrite(BUZZER_PIN, LOW);

  while (1) {
    updateAllServos();

    unsigned long now = millis();

    // BACA JOYSTICK
    if (now - lastRead >= READ_INTERVAL) {
      lastRead = now;

      int xRaw = analogRead(X_PIN);
      int yRaw = analogRead(Y_PIN);

      xFiltered += (xRaw - xFiltered) * FILTER_ALPHA;
      yFiltered += (yRaw - yFiltered) * FILTER_ALPHA;

      if (abs(xFiltered - 512.0f) < DEADZONE)
        xFiltered = 512.0f;

      if (abs(yFiltered - 512.0f) < DEADZONE)
        yFiltered = 512.0f;

      float panAngle =
        PAN_MIN + ((xFiltered / 1023.0f) * (PAN_MAX - PAN_MIN));

      float tiltAngle =
        TILT_MIN + ((yFiltered / 1023.0f) * (TILT_MAX - TILT_MIN));

      panAngle = constrain(panAngle, PAN_MIN, PAN_MAX);
      tiltAngle = constrain(tiltAngle, TILT_MIN, TILT_MAX);

      if (abs(panAngle - lastPanAngle) > SERVO_THRESHOLD) {
        panServo.startEaseTo((int)panAngle);

        lastPanAngle = panAngle;
      }

      if (abs(tiltAngle - lastTiltAngle) > SERVO_THRESHOLD) {
        tiltServo.startEaseTo((int)tiltAngle);

        lastTiltAngle = tiltAngle;
      }
    }

    int ldrValue = analogRead(LDR_PIN);

    // laser mengenai LDR
    bool laserDetected = ldrValue > LDR_THRESHOLD;

    if (laserDetected) {
      if (now - lastBuzzerToggle >= BUZZER_INTERVAL) {
        lastBuzzerToggle = now;

        buzzerState = !buzzerState;

        digitalWrite(BUZZER_PIN, buzzerState);
      }
    } else {
      // buzzer mati jika laser hilang
      buzzerState = false;
      digitalWrite(BUZZER_PIN, LOW);
    }

    /* =========================
       DEBUG OPTIONAL
    ========================= */
    /*
    Serial.print("LDR: ");
    Serial.println(ldrValue);
    */
  }

  return 0;
}

void loop() {
}