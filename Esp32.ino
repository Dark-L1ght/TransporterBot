#include <WiFi.h>
#include <WebServer.h>

// ================= KONFIGURASI WIFI =================
const char* ssid = "Robot_Controller";
const char* password = "12345678";

WebServer server(80);

// Definisi Pin Serial ke Nano (RX=16, TX=17)
#define RXD2 16
#define TXD2 17

// ================= HTML & CSS =================
// Konten HTML dipisahkan ke file index.h
#include "index.h"

// --- Handler Perintah ---
void handleCommand() {
  if (server.hasArg("val")) {
    String val = server.arg("val");
    char cmd = val.charAt(0);
    
    Serial2.print(cmd); // Kirim ke Nano
    Serial.print("Cmd: "); Serial.println(cmd);
    server.send(200, "text/plain", "OK");
  } else {
    server.send(400, "text/plain", "Bad Request");
  }
}

// --- Handler Speed Slider ---
void handleSpeed() {
  if (server.hasArg("val")) {
    String speedVal = server.arg("val");
    int speed = speedVal.toInt();
    speed = constrain(speed, 0, 255);
    
    Serial2.print("V");
    Serial2.println(speed); 
    
    Serial.print("Speed: "); Serial.println(speed);
    server.send(200, "text/plain", "OK");
  } else {
    server.send(400, "text/plain", "Bad Request");
  }
}

void setup() {
  Serial.begin(115200);
  
  // Inisialisasi Serial ke Nano
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);

  WiFi.softAP(ssid, password);
  Serial.println("WiFi AP dimulai.");
  Serial.println(WiFi.softAPIP());

  server.on("/", [](){ server.send_P(200, "text/html", index_html); });
  server.on("/cmd", handleCommand);
  server.on("/speed", handleSpeed);

  server.begin();
  Serial.println("Server Start");
}

void loop() {
  server.handleClient();
}