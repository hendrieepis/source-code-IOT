#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>

// Ganti dengan WiFi kamu
const char* WIFI_SSID = "your_ssid";
const char* WIFI_PASS = "your_password";

const uint16_t UDP_PORT = 4210;
const int LED_PIN = 23;  // 

WiFiUDP udp;
char buf[64];

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Menghubungkan WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("IP ESP32: ");
  Serial.println(WiFi.localIP());

  udp.begin(UDP_PORT);
  Serial.printf("UDP listen di port %u\n", UDP_PORT);
}

void loop() {
  int size = udp.parsePacket();
  if (size <= 0) return;

  int len = udp.read(buf, sizeof(buf) - 1);
  if (len < 0) len = 0;
  buf[len] = '\0';
  Serial.printf("Dari %s:%u -> \"%s\"\n",
                udp.remoteIP().toString().c_str(), udp.remotePort(), buf);

  // Ambil karakter pertama yang bukan spasi/newline
  char c = 0;
  for (int i = 0; i < len; i++) {
    if (buf[i] != ' ' && buf[i] != '\r' && buf[i] != '\n') { c = buf[i]; break; }
  }

  const char* reply;
  if (c == '1') {
    digitalWrite(LED_PIN, HIGH);
    reply = "LED ON\n";
  } else if (c == '0') {
    digitalWrite(LED_PIN, LOW);
    reply = "LED OFF\n";
  } else {
    reply = "Kirim 1 atau 0\n";
  }

  // Balas ke pengirim (terlihat di ncat)
  udp.beginPacket(udp.remoteIP(), udp.remotePort());
  udp.print(reply);
  udp.endPacket();
}
