#include <AccelStepper.h>

const byte STEP_PIN = 3;
const byte DIR_PIN  = 2;

// Joystick
const byte X_PIN = A1;
const byte SW_PIN = 4;

// IR & BUZZER
const byte IR_PIN = 7;
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

bool lastIrState = LOW;
int counter = 0;

bool lockJoystick = false;

int main() {
	init();
	Serial.begin(9600);

	pinMode(BUZZER_PIN, OUTPUT);
	pinMode(IR_PIN, INPUT);
	pinMode(SW_PIN, INPUT_PULLUP);

	digitalWrite(BUZZER_PIN, LOW);

	stepper.setMaxSpeed(STEP_SPEED);
	stepper.setAcceleration(STEP_ACCEL);

	stepper.setCurrentPosition(1000);

	while(1){
		unsigned long now = millis();

		stepper.run();

		if (!lockJoystick) {
			if (now - lastRead >= READ_INTERVAL) {
				lastRead = now;

				int xRaw = analogRead(X_PIN);

				xFiltered += (xRaw - xFiltered) * FILTER_ALPHA;

				if (abs(xFiltered - 512.0f) < DEADZONE)
					xFiltered = 512.0f;

				long target =
					POS_MIN + ((xFiltered / 1023.0f) * (POS_MAX - POS_MIN));

				target = constrain(target, POS_MIN, POS_MAX);

				if (abs(target - lastTarget) > JOY_THRESHOLD) {
					stepper.moveTo(target);
					lastTarget = target;
				}
			}

			bool irState = digitalRead(IR_PIN);

			// rising edge
			if (irState == HIGH && lastIrState == LOW) {
				if (counter < 5) {
					counter++;

					Serial.print("Counter: ");
					Serial.println(counter);
				}
			}

			lastIrState = irState;

			if (counter >= 5) {
				lockJoystick = true;
			}
		}

		if (lockJoystick) {
			if (now - lastBuzzerToggle >= BUZZER_INTERVAL) {
				lastBuzzerToggle = now;
				buzzerState = !buzzerState;
				digitalWrite(BUZZER_PIN, buzzerState);
			}
		} else {
			buzzerState = false;
			digitalWrite(BUZZER_PIN, LOW);
		}

		// reset joystick switch
		if (digitalRead(SW_PIN) == LOW) {
			counter = 0;
			lockJoystick = false;

			buzzerState = false;
			digitalWrite(BUZZER_PIN, LOW);

			Serial.println("Counter Reset");
		}
	}

	return 0;
}