#if defined(ESP8266)
#include <ESP8266WiFi.h>
#elif defined(ESP32)
#include <WiFi.h>
#endif

#include <PubSubClient.h>
#include <ArduinoJson.h>

// ====================== Konfigurasi =========================
const char* ssid = "IoT";
const char* password = "12345678";
const char* mqtt_server = "test.mosquitto.org";
const int mqtt_port = 1883;
const char* mqtt_topic = "poliban/suhu";
const char* mqtt_cmd = "poliban/relay";


// Ganti dengan pin LED yang kamu pakai
#define LED_PIN 2  // built-in LED untuk ESP32/ESP8266

WiFiClient espClient;
PubSubClient client(espClient);

// Waktu untuk reconnect MQTT
unsigned long lastReconnectAttempt = 0;

// Waktu untuk publish data
unsigned long lastSend = 0;
const unsigned long interval = 60000;  // setiap 5 detik
char msg[128];
// ======================= WiFi Setup =========================
void setup_wifi() {
  WiFi.begin(ssid, password);
  Serial.print(F("Menghubungkan ke WiFi"));
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(F("\nWiFi terhubung!"));
  Serial.print(F("IP Address: "));
  Serial.println(WiFi.localIP());
}

// ===================== MQTT Callback ========================
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print(F("Pesan diterima ["));
  Serial.print(topic);
  Serial.print(F("]: "));

// 1. Tampilkan langsung (raw payload)
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

    // 2. Simpan ke buffer agar bisa di-parse sebagai JSON
  const size_t bufferSize = 256;
  if (length >= bufferSize) {
    Serial.println(F("Payload terlalu panjang, abaikan."));
    return;
  }

  char message[bufferSize];
  memcpy(message, payload, length);
  message[length] = '\0';  // penting untuk memastikan string valid

  // 3. Parse JSON (jika formatnya JSON)
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, message);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

// 4. Ambil isi JSON dan pastikan tipe datanya
  const char* device = doc["device"];  // e.g., "CH0", "CH1", etc
  int cmd = doc["cmd"];                // 1 = ON, 0 = OFF

  // Contoh aksi
    digitalWrite(LED_PIN, cmd == 1 ? HIGH : LOW);

    Serial.print(F("Perangkat "));
    Serial.print(device);
    Serial.print(F(" "));
    Serial.println(cmd == 1 ? F("dinyalakan") : F("dimatikan"));
}

// ============== MQTT Non-blocking Reconnect =================
bool reconnect() {
  String clientId = "ESPClient-";
  clientId += String(random(0xffff), HEX);
  if (client.connect(clientId.c_str())) {
    client.subscribe(mqtt_cmd);
    Serial.println(F("MQTT: Terhubung & Subscribed!"));
  }
  return client.connected();
}

// ==================== Kirim Data JSON =======================
void kirimData() {
  StaticJsonDocument<128> doc;

  doc["temp"] = random(20, 30);
  doc["hum"] = random(40, 80);

  char payload[128];
  size_t len = serializeJson(doc, payload);

  Serial.print(F("Data dikirim: "));
  client.publish(mqtt_topic, payload, len);
  Serial.print(F("Data dikirim: "));
  Serial.println(payload);
}


// ===================== Setup Awal ===========================
void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);  // pastikan LED mati saat awal
  setup_wifi();

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  lastReconnectAttempt = 0;
}

// ======================== Loop ==============================
void loop() {
  if (!client.connected()) {
    unsigned long now = millis();
    if (now - lastReconnectAttempt > 5000) {
      lastReconnectAttempt = now;
      if (reconnect()) {
        lastReconnectAttempt = 0;
      }
    }
  } else {
    client.loop();

    unsigned long now = millis();
    if (now - lastSend > interval) {
      lastSend = now;
      kirimData();
    }
  }
}
