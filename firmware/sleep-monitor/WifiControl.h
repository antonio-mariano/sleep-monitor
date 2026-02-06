#pragma once
#include <WiFi.h>
#include <WebServer.h>

extern const char* ssid;
extern const char* password;

extern const int button_pin;

//Liga wifi
void connect_wifi();

//Desliga wifi
void disconnect_wifi();

void setup_wifi();
bool is_wifi_on();

void wifi_button();

