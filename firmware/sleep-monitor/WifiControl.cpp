#include <Arduino.h>
#include "WifiControl.h"

#include <WiFi.h>
#include <WebServer.h>
#include <time.h>

#include "Communication.h"
#include "Sensors.h"

/// Put here your credentials
//const char* ssid = "Your SSID";
//const char* password = "Your Password";

const int button_pin = 15;
bool wifi_on = true;

//Liga wifi
void connect_wifi()
{
  Serial.print("\n");
  Serial.printf("Ligando WiFi no SSID %s\n", ssid);
  delay(100);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".\n");
  }
  Serial.print("WiFi ligado no IP: "); Serial.println(WiFi.localIP());
  digitalWrite(LED_BUILTIN, HIGH);  // LED aceso = Wi-Fi ligado
  wifi_on = true;
}

//Desliga wifi
void disconnect_wifi()
{
  WiFi.disconnect(true);  // Desliga e limpa credenciais
  WiFi.mode(WIFI_OFF);    // Desativa o módulo Wi-Fi
  Serial.print("Wifi foi desligado\n");
  digitalWrite(LED_BUILTIN, LOW);  
  wifi_on = false;
}


void setup_wifi()
{
  connect_wifi();

  //Config Time
  configTime(0 /*PT*/, 0 /*3600 no verao*/, "pool.ntp.org", "time.nist.gov");
  // Esperar que o tempo fique disponível
  initial_time = time(nullptr);
  while (initial_time < 100000) {  // tempo inválido
    delay(500);
    initial_time = time(nullptr);
  }

  //Server
  server.on("/events", handleEvents);
  //server.on("/clear", handleClear);
  server.on("/command", handleCommand);
  server.begin();
}

bool is_wifi_on(){
  return wifi_on;
}


void wifi_button(){
  static bool first_execution = true;
  static bool button_state = HIGH;

  if(first_execution){
    pinMode(button_pin, INPUT_PULLUP);
    first_execution = false;
  }

  if(button_state == HIGH && digitalRead(button_pin) == LOW){
      button_state = LOW;
      is_wifi_on() ? disconnect_wifi() : connect_wifi(); // Toggle wifi
    }
    else if(button_state == LOW && digitalRead(button_pin) == HIGH){
      button_state = HIGH;
    }
}


