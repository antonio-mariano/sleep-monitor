#pragma once

#include <WebServer.h> 
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

//Cria uma string str[] no formato aaaa-mm-dd; t_seg = segundos desde que o pico foi iniciado. 
void formatTime(long seg, char str[]);