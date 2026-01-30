#include <Arduino.h>

// Pins da matriz de velostat
const int Line[2] = {16, 17};
const int Col[2] = {27, 26}; //ADCs
const int button_pin = 15;
const int LED_PIN = 25; //Built-in

// Eventos
#define N_events 4096
long events[N_events][3];
int i_evt = 1; //Porque testamos o evento anterior

// Div
bool print = false;
bool wifi_on = true;
bool button_state = HIGH;

void setup() {

  Serial.begin(115200);
  setup_wifi();
  disconnect_wifi();
  analogReadResolution(12);

  pinMode(button_pin, INPUT_PULLUP);

  //Inicializa matriz de velostat
  for(int n = 0; n < 2; n++){
    pinMode(Line[n], OUTPUT);
    digitalWrite(Line[n], LOW);
  }
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
      if(print)  Serial.printf("P%d:%.2f ", n, p);
      detect_event(time, n, p);
    }
    if(print)  Serial.printf("Min:-1000 Max:1000\n");

    // Botão do wifi
    wifi_button();

  }

  handleClient();
  handleConsole();
}

void print_events()
{
  Serial.printf("[\n");
  for(int n = 0; n < i_evt; n++)  Serial.printf("%d %d %d; ", events[n][0], events[n][1],events[n][2]);
  Serial.printf("\n];\n");
}

void wifi_button(){
  if(button_state == HIGH && digitalRead(button_pin) == LOW){
      button_state = LOW;
      wifi_on ? disconnect_wifi() : connect_wifi(); // Toggle wifi
    }
    else if(button_state == LOW && digitalRead(button_pin) == HIGH){
      button_state = HIGH;
    }
}

void handleMessage(char msg[])
{
  char c;
  float value;
  sscanf(msg, "%c %f", &c, &value);

  if(c == 'w') wifi_on ? disconnect_wifi() : connect_wifi(); // Toggle wifi
  if(c == 't'){ char str[24]; formatTime(millis()/1000, str); Serial.println(str); } //Imprime tempo atual
  if(c == 'c')  calibration(msg[1]); // Calibration
  if(c == 'p')  print = !print; // Toggle o print dos sinais
  if(c == 'e')  print_events(); // Imprime os eventos guardados
}

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

