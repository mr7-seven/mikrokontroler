#include <AccelStepper.h>

const byte STEP_PIN = 3;
const byte DIR_PIN  = 2;

// Joystick
const byte X_PIN = A1;

// LDR & BUZZER
const byte LDR_PIN = A2;
const byte BUZZER_PIN = 8;

const long POS_MIN = 0;
const long POS_MAX = 2000;

float xFiltered = 512.0f;

const float FILTER_ALPHA = 0.10f;
const int DEADZONE = 50;

const float JOY_THRESHOLD = 3.0f;

const int STEP_SPEED = 800;
const int STEP_ACCEL = 300;

AccelStepper stepper(AccelStepper::DRIVER, STEP_PIN, DIR_PIN);

long lastTarget = 0;

unsigned long lastRead = 0;
const uint16_t READ_INTERVAL = 20;

bool buzzerState = false;
unsigned long lastBuzzerToggle = 0;
const uint16_t BUZZER_INTERVAL = 150;

const int LDR_THRESHOLD = 750;


int main() {
	init();
	  Serial.begin(9600);

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  stepper.setMaxSpeed(STEP_SPEED);
  stepper.setAcceleration(STEP_ACCEL);

  stepper.setCurrentPosition(1000); // start di tengah
  
  while(1){
	    unsigned long now = millis();

  stepper.run();

  if (now - lastRead >= READ_INTERVAL) {
    lastRead = now;

    int xRaw = analogRead(X_PIN);

    // filter smoothing
    xFiltered += (xRaw - xFiltered) * FILTER_ALPHA;

    // deadzone
    if (abs(xFiltered - 512.0f) < DEADZONE)
      xFiltered = 512.0f;

    // mapping joystick → posisi stepper
    long target =
      POS_MIN + ((xFiltered / 1023.0f) * (POS_MAX - POS_MIN));

    target = constrain(target, POS_MIN, POS_MAX);

    // update hanya jika berubah signifikan
    if (abs(target - lastTarget) > JOY_THRESHOLD) {
      stepper.moveTo(target);
      lastTarget = target;
    }
  }

  int ldrValue = analogRead(LDR_PIN);
  bool laserDetected = ldrValue > LDR_THRESHOLD;

  if (laserDetected) {
    if (now - lastBuzzerToggle >= BUZZER_INTERVAL) {
      lastBuzzerToggle = now;
      buzzerState = !buzzerState;
      digitalWrite(BUZZER_PIN, buzzerState);
    }
  } else {
    buzzerState = false;
    digitalWrite(BUZZER_PIN, LOW);
  }
  }


  return 0;
}