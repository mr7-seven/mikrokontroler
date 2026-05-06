#define RPWM_PIN 5
#define LPWM_PIN 6

#define X_AXIS_PIN A0
#define SW_PIN 2

const int DEADZONE = 40;
const int JOY_CENTER = 512;
const int MIN_PWM = 70;

const float ACCEL_RATE = 0.06;
const float BRAKE_RATE = 0.15;
const float ADC_FILTER = 0.10;

const int REVERSE_DELAY = 150;

int requestedSpeed = 0;
int targetSpeed = 0;

float currentSpeed = 0;

bool sw;
bool last_sw = false;

bool reversing = false;

unsigned long reverseTimer = 0;
unsigned long lastUpdate = 0;
unsigned long serialTimer = 0;

int main(){
  init();
  Serial.begin(9600);

  pinMode(RPWM_PIN, OUTPUT);
  pinMode(LPWM_PIN, OUTPUT);

  pinMode(SW_PIN, INPUT_PULLUP);

  lastUpdate = millis();
  serialTimer = millis();

  motor(0);
  
  while(1){
  bacaPushButton();

  bacaJoystick();

  updateMotor();

  serialMonitor();
	  
  }
	
	return 0;
}


void bacaPushButton() {

  sw = !digitalRead(SW_PIN);

  if (sw && !last_sw) {

    requestedSpeed = 0;
  }

  last_sw = sw;
}

void bacaJoystick() {

  static float filteredX = JOY_CENTER;

  int rawX = analogRead(X_AXIS_PIN);

  filteredX += (rawX - filteredX) * ADC_FILTER;

  int x = (int)filteredX;

  x -= JOY_CENTER;

  if (abs(x) < DEADZONE) {
    x = 0;
  }

  float normalized = x / 512.0;

  float expo = normalized * abs(normalized);

  requestedSpeed = expo * 255;
}

void updateMotor() {

  if (millis() - lastUpdate < 20) return;

  lastUpdate += 20;

  bool bedaArah =
    (currentSpeed > 0 && requestedSpeed < 0) ||
    (currentSpeed < 0 && requestedSpeed > 0);

  if (bedaArah && !reversing) {

    reversing = true;
  }

  if (reversing) {

    targetSpeed = 0;

    currentSpeed += (0 - currentSpeed) * BRAKE_RATE;

    if (abs(currentSpeed) < 5) {

      currentSpeed = 0;

      motor(0);

      if (reverseTimer == 0) {

        reverseTimer = millis();
      }

      if (millis() - reverseTimer >= REVERSE_DELAY) {

        reversing = false;

        reverseTimer = 0;
      }
    }

    motor((int)currentSpeed);

    return;
  }

  targetSpeed = requestedSpeed;

  currentSpeed += (targetSpeed - currentSpeed) * ACCEL_RATE;

  if (abs(currentSpeed) < 2) {

    currentSpeed = 0;
  }

  motor((int)currentSpeed);
}

void motor(int speed) {

  speed = constrain(speed, -255, 255);

  if (abs(speed) < 20) {
    speed = 0;
  }

  if (speed > 0) {

    speed = map(speed, 1, 255, MIN_PWM, 255);
  }
  else if (speed < 0) {

    speed = -map(-speed, 1, 255, MIN_PWM, 255);
  }

  if (speed > 0) {

    analogWrite(RPWM_PIN, speed);
    analogWrite(LPWM_PIN, 0);
  }
  else if (speed < 0) {

    analogWrite(RPWM_PIN, 0);
    analogWrite(LPWM_PIN, -speed);
  }
  else {

    analogWrite(RPWM_PIN, 0);
    analogWrite(LPWM_PIN, 0);
  }
}

void serialMonitor() {

  if (millis() - serialTimer < 100) return;

  serialTimer += 100;

  Serial.print("Req: ");
  Serial.print(requestedSpeed);

  Serial.print("  Target: ");
  Serial.print(targetSpeed);

  Serial.print("  Current: ");
  Serial.print((int)currentSpeed);

  Serial.print("  Reverse: ");
  Serial.println(reversing);
}