#include <SPI.h>
#include <MFRC522.h>

/*
 * ---------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    
 *             Reader/PCD   Uno/101       Mega      Nano v3    
 * Signal      Pin          Pin           Pin       Pin        
 * ---------------------------------------------------------
 * RST/Reset   RST          9             5         D9         
 * SPI SS      SDA(SS)      10            53        D10        
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        
 * SPI MISO    MISO         12 / ICSP-1   50        D12        
 * SPI SCK     SCK          13 / ICSP-3   52        D13        
*/

#define RST_PIN         9           
#define SS_PIN          10          

MFRC522 mfrc522(SS_PIN, RST_PIN);   

unsigned long previousMillis = 0;
const unsigned long interval = 2000; // Waktu interval dalam milidetik

String scanUID() {
  // mencari kartu RFID
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return "";
  }

  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return "";
  }

  // menampilkan UID kartu RFID yang dibaca
  String uid = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    uid += String(mfrc522.uid.uidByte[i], HEX);
  }
  uid.toUpperCase();

  // membersihkan kartu RFID dari MFRC522
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();

  return uid;
}

void setup()
{
  Serial.begin(9600);
  SPI.begin();             // inisialisasi SPI bus
  mfrc522.PCD_Init();       // inisialisasi MFRC522
}

void loop()
{
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis; // Mengupdate waktu sekarang

    String uid = scanUID();
    if (uid.length() > 0) {
      Serial.println("UID tag: " + uid);
    }
  }
}