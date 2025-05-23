#include <ModbusRTUSlave.h>

#define MODBUS_SERIAL Serial

#define MODBUS_BAUD 9600
#define MODBUS_CONFIG SERIAL_8N1
#define MODBUS_UNIT_ID 1


const int16_t buttonPins[2] = {5, 6};
const int16_t ledPins[3] = {2, 3, 4};
const int16_t knobPins[2] = {A0, A1};

ModbusRTUSlave modbus(MODBUS_SERIAL);

const uint8_t numCoils = 2;
const uint8_t numDiscreteInputs = 2;
const uint8_t numHoldingRegisters = 2;
const uint8_t numInputRegisters = 2;

bool coils[numCoils];
bool discreteInputs[numDiscreteInputs];
uint16_t holdingRegisters[numHoldingRegisters];
uint16_t inputRegisters[numInputRegisters];



void setup() {
  pinMode(knobPins[0], INPUT);
  pinMode(knobPins[1], INPUT);
  pinMode(buttonPins[0], INPUT_PULLUP);
  pinMode(buttonPins[1], INPUT_PULLUP);
  pinMode(ledPins[0], OUTPUT);
  pinMode(ledPins[1], OUTPUT);
  pinMode(ledPins[2], OUTPUT);

  modbus.configureCoils(coils, numCoils);
  modbus.configureDiscreteInputs(discreteInputs, numDiscreteInputs);
  modbus.configureHoldingRegisters(holdingRegisters, numHoldingRegisters);
  modbus.configureInputRegisters(inputRegisters, numInputRegisters);

  MODBUS_SERIAL.begin(MODBUS_BAUD, MODBUS_CONFIG);
  modbus.begin(MODBUS_UNIT_ID, MODBUS_BAUD, MODBUS_CONFIG);
}

void loop() {
  inputRegisters[0] = map(analogRead(knobPins[0]), 0, 1023, 0, 255);
  inputRegisters[1] = map(analogRead(knobPins[1]), 0, 1023, 0, 255);
  discreteInputs[0] = !digitalRead(buttonPins[0]);
  discreteInputs[1] = !digitalRead(buttonPins[1]);

  modbus.poll();

  analogWrite(ledPins[0], holdingRegisters[0]);
  analogWrite(ledPins[1], holdingRegisters[1]);
  digitalWrite(ledPins[0], coils[0]);
  digitalWrite(ledPins[1], coils[1]);
  digitalWrite(ledPins[2], coils[2]);
}