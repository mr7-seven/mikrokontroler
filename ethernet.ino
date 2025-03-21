#include <SPI.h>
#include <Ethernet.h>

//MAC address
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
IPAddress server(192, 168, 1, 10);  // numeric IP for Google (no DNS)
// char server[] = "www.google.com";    // name address for Google (using DNS)

// Set the static IP address to use if the DHCP fails to assign
IPAddress ip(192, 168, 1, 2);
#define port 1234
// that you want to connect to (port 80 is default for HTTP):
EthernetClient client;

// Waktu untuk mengontrol interval pengiriman
unsigned long previousMillis = 0;
const long interval = 5000;  // 5000 ms (5 detik)
String data;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB port only
  }

  // start the Ethernet connection:
  Serial.println("Initialize Ethernet with DHCP:");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // Check for Ethernet hardware present
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
      while (true) {
        delay(1);  // do nothing, no point running without Ethernet hardware
      }
    }
    if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println("Ethernet cable is not connected.");
    }
    // try to configure using IP address instead of DHCP:
    Ethernet.begin(mac, ip);
  } else {
    Serial.print("  DHCP assigned IP ");
    Serial.println(Ethernet.localIP());
  }
  // give the Ethernet shield a second to initialize:
  delay(1000);
  Serial.println("Menghubungkan ke server...");
  if (client.connect(server, port)) {  // Koneksi ke server di port 8080
    Serial.println("Terhubung!");
  } else {
    Serial.println("Gagal terhubung!");
  }
}

void loop() {

  unsigned long currentMillis = millis();

  if (client.available()) {
    char c = client.read();

    data += c;

    if (c == '\n') {
      Serial.println(data);
      data = "";
    }
  }

  // Kirim data setiap 5 detik tanpa menghambat eksekusi lain
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;  // Perbarui waktu terakhir pengiriman

    if (client.connected()) {
      int dataRandom = random(1, 100);  // Buat data acak untuk dikirim
      Serial.print("Mengirim data: ");
      Serial.println(dataRandom);

      // client.print("Data dari Arduino: ");
      client.println(dataRandom);
    } else {
      Serial.println("Terputus dari server. Mencoba menghubungkan kembali...");
      client.stop();
      if (client.connect(server, port)) {
        Serial.println("Berhasil terhubung kembali!");
      } else {
        Serial.println("Gagal terhubung, cek server!");
      }
    }
  }
}
