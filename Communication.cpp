#include <Arduino.h>

#include <WiFi.h>
#include <WebServer.h>
#include <time.h>

#include "Communication.h"
#include "sensors.h"
#include "Events.h"


WebServer server(80);


time_t initial_time;




//seg = segundos desde que o pico foi iniciado. Coloca em str o tempo formatado
void formatTime(long seg, char str[])
{
  // Converter para estrutura local
  time_t agora = initial_time + seg;
  struct tm* horaLocal = localtime(&agora);
  strftime(str, 24, "%Y-%m-%d %H:%M:%S", horaLocal);
}



void handleCommand() {
  if (server.hasArg("action")) {
    String action = server.arg("action");
    
    if (action == "cs")      calibration('s');
    else if(action == "ce")  calibration('e');
    else                     Serial.println("Comando desconhecido: " + action);
    
    server.send(200, "text/plain", "Comando recebido: " + action);
  } else {
    server.send(400, "text/plain", "Falta parâmetro 'action'");
  }
}

//Envia os eventos quando o cliente acede ao site https://<ip>/dados
void handleEvents() {

  char data_str[25];
  String resposta =  "Time (min), Position, Pressure\n";

  for(int n = 0; n < i_evt; n++){
    formatTime(events[n][0], data_str);
    resposta += data_str;
    resposta += "," + String(events[n][1]) + "," + String(events[n][2]) + "\n";
  }

  server.send(200, "text/plain", resposta);
}

// Apaga os eventos (posicionando i_evt no inicio do array)
// quando o cliente acede ao site https://<ip>/apagar
void handleClear() {
  i_evt = 0;
  server.send(200, "text/plain", "Dados apagados");
  Serial.println("Dados apagados.");
}








void handleClient(){
  server.handleClient();
}
