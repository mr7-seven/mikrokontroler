#if defined(ESP8266)
  #include <ESP8266WiFi.h>
#elif defined(ESP32)
  #include <WiFi.h>
#endif

#include <PubSubClient.h>
#include <ArduinoJson.h>

// ====================== Konfigurasi =========================
const char* ssid = "NAMA_WIFI";
const char* password = "PASSWORD_WIFI";
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;

// Ganti dengan pin LED yang kamu pakai
#define LED_PIN 2  // built-in LED untuk ESP32/ESP8266

WiFiClient espClient;
PubSubClient client(espClient);

// Waktu untuk reconnect MQTT
unsigned long lastReconnectAttempt = 0;

// Waktu untuk publish data
unsigned long lastSend = 0;
const unsigned long interval = 5000; // setiap 5 detik

// ======================= WiFi Setup =========================
void setup_wifi() {
  WiFi.begin(ssid, password);
  Serial.print("Menghubungkan ke WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi terhubung!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

// ===================== MQTT Callback ========================
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Pesan diterima [");
  Serial.print(topic);
  Serial.print("]: ");

  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, message);
  if (error) {
    Serial.print("deserializeJson() gagal: ");
    Serial.println(error.f_str());
    return;
  }

  const char* cmd = doc["cmd"];
  const char* val = doc["val"];

  // Cek perintah LED
  if (strcmp(cmd, "led") == 0) {
    if (strcmp(val, "ON") == 0) {
      digitalWrite(LED_PIN, HIGH);
      Serial.println("LED: ON");
    } else if (strcmp(val, "OFF") == 0) {
      digitalWrite(LED_PIN, LOW);
      Serial.println("LED: OFF");
    }
  }
}

// ============== MQTT Non-blocking Reconnect =================
bool reconnect() {
  if (client.connect("espClient")) {
    client.subscribe("esp/perintah");
    Serial.println("MQTT: Terhubung & Subscribed!");
  }
  return client.connected();
}

// ==================== Kirim Data JSON =======================
void kirimData() {
  JsonDocument doc;

#if defined(ESP8266)
  doc["device"] = "esp8266";
#elif defined(ESP32)
  doc["device"] = "esp32";
#endif

  doc["uptime"] = millis() / 1000;
  doc["status"] = "OK";

  char buffer[256];
  serializeJson(doc, buffer);

  client.publish("esp/status", buffer);
  Serial.println("Data dikirim: " + String(buffer));
}

// ===================== Setup Awal ===========================
void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW); // pastikan LED mati saat awal
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
