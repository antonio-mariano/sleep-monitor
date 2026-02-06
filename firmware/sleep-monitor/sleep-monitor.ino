#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <time.h>

#include "WifiControl.h"
#include "Communication.h"
#include "Sensors.h"
#include "Events.h"


// Div
bool print_sensors = false;

void setup() {

  Serial.begin(115200);
  setup_wifi();
  disconnect_wifi();
  analogReadResolution(12);

  setup_sensors();
  read_eeprom();
}

void loop()
{
  long time = millis();
  static long prev_time = 0;

  if(time >= prev_time + 100)
  {
    prev_time = time;
    Read_G(); //Lê o G para os 4 sensores em matriz (calculos em matriz que explico no word)
    
    // Para os 4 sensores, calcula a pressão a partir dos G lidos, imprime, e verifica se há um evento ("sinal variando")
    for(int n = 0; n < 4; n++) 
    {
      float p = pressure(n);
      if(print_sensors)  Serial.printf("P%d:%.2f ", n, p);
      detect_event(time, n, p);
    }
    if(print_sensors)  Serial.printf("Min:-1000 Max:1000\n");

    // Botão do wifi
    wifi_button();
  }

  handleClient();
  handleConsole();
}

//Handle serial monitor input
void handleConsole()
{
  static char str[16] = "";
  int i = 0;

  while (Serial.available() > 0) {
    int inChar = Serial.read();

    if (inChar != '\n') {
      str[i++] = inChar;
    }
    else { 
      handleMessage(str);
      // clear the string for new input:
      str[0] = '\0';
      i = 0;
    }
  }
}


// Handle input commands
void handleMessage(char msg[])
{
  char c;
  float value;
  sscanf(msg, "%c %f", &c, &value);

  if(c == 'w' || c == 'W')  is_wifi_on() ? disconnect_wifi() : connect_wifi(); // Toggle wifi
  if(c == 't' || c == 'T')  { char str[24]; formatTime(millis()/1000, str); Serial.println(str); } //Imprime tempo atual
  if(c == 'c' || c == 'C')  calibration(msg[1]); // Calibration (Start ou End)
  if(c == 'p' || c == 'P')  print_sensors = !print_sensors; // Toggle o print dos sinais
  if(c == 'e' || c == 'E')  print_events(); // Imprime os eventos guardados
}


