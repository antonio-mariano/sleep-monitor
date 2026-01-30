#include <WiFi.h>
#include <WebServer.h>
#include <time.h>

#include "credentials.h"
//const char* ssid = "Your SSID";
//const char* password = "Your password";


WebServer server(80);

time_t initial_time;

//Liga wifi
void connect_wifi()
{
  Serial.print("Ligando WiFi...\n");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
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

//seg = segundos desde que o pico foi iniciado. Coloca em str o tempo formatado
void formatTime(long seg, char str[])
{
  // Converter para estrutura local
  time_t agora = initial_time + seg;
  struct tm* horaLocal = localtime(&agora);
  strftime(str, 24, "%Y-%m-%d %H:%M:%S", horaLocal);
}

void handleClient(){
  server.handleClient();
}
