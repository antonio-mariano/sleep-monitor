#pragma once

#include <WebServer.h> // ← isto é obrigatório aqui extern WebServer server;
extern WebServer server;



extern time_t initial_time;

void connect_wifi();

//Desliga wifi
void disconnect_wifi();

void setup_wifi();

bool is_wifi_on();

void handleClient();

void handleCommand();

void handleEvents();

void formatTime(long seg, char str[]);