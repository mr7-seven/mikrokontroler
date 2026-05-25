#include <Arduino.h>
#include <NeoSWSerial.h>
#include <ctype.h>
#include "BluetoothHandler.h"

extern NeoSWSerial BT;

extern char buffer[32];
extern uint8_t indexBuffer;

extern void prosesPerintah(char *cmd);

unsigned long lastReceive = 0;

const unsigned long timeout = 100;

void bacaBluetooth() {

  while (BT.available()) {

    char c = BT.read();

    lastReceive = millis();

    if (indexBuffer < 31) {

      buffer[indexBuffer++] = c;
    }
  }

  if (indexBuffer > 0 &&
      millis() - lastReceive > timeout) {

    buffer[indexBuffer] = '\0';

    trimString(buffer);

    toUpperCase(buffer);

    Serial.print("Data: ");
    Serial.println(buffer);

    prosesPerintah(buffer);

    indexBuffer = 0;
  }
}

void trimString(char *str) {

  char *start = str;

  while (isspace(*start)) {
    start++;
  }

  memmove(str, start, strlen(start) + 1);

  char *end = str + strlen(str) - 1;

  while (end >= str && isspace(*end)) {

    *end = '\0';

    end--;
  }
}

void toUpperCase(char *str) {

  while (*str) {

    *str = toupper(*str);

    str++;
  }
}