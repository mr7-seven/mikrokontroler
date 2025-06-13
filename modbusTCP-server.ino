#ifdef ESP8266
#include <ESP8266WiFi.h>
#else  //ESP32
#include <WiFi.h>
#endif
#include <ModbusIP_ESP8266.h>
 
//Modbus Registers Offsets
const int LED_COIL = 100;
const int SENSOR_IREG = 100;
const int SWITCH_ISTS = 100;
const int TEST_HREG = 100;
//Used Pins
const int ledPin = 2;     //GPIO2, LED BUILT-IN
const int switchPin = 0;  // GPIO0, tombol BOOT

//ModbusIP object
ModbusIP mb;

unsigned long ts = 0;

const char* ssid = "IoT";
const char* password = "12345678";

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  mb.server();

  pinMode(ledPin, OUTPUT);
  pinMode(switchPin, INPUT_PULLUP);

  mb.addCoil(LED_COIL);
  mb.addIsts(SWITCH_ISTS);
  mb.addIreg(SENSOR_IREG);
}

void loop() {
  //Call once inside loop() - all magic here
  mb.task();

  if (millis() - ts >= 1000) {
    ts = millis();

    mb.addHreg(TEST_HREG, 1234);
    mb.Ireg(SENSOR_IREG, random(100, 200));
  }

  mb.Ists(SWITCH_ISTS, !digitalRead(switchPin));
  digitalWrite(ledPin, mb.Coil(LED_COIL));
  delay(10);
}