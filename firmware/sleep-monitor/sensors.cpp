#include <Arduino.h>
#include <EEPROM.h>

#include "Communication.h"
#include "sensors.h"
#include "Events.h"

const uint32_t MAGIC = 0xCAFEBABE;

// Calibração
// Pins da matriz de velostat
const int Line[2] = {16, 17};
const int Col[2] = {27, 26}; //ADCs


bool calibrating = false;
float Gmax[4];
float Gmin[4];


// Condutância G 
const float Gc = 1/R0;
float G[4] = {0, 0, 0, 0};
float W[4];

//Parâmetros para normalização
float m[4] = {4000, 4000, 4000, 4000};
float b[4] = {0, 0, 0, 0};

/***** Funções ******/



void save_eeprom() {
  EEPROM.begin(32);
  EEPROM.put(0, MAGIC);   // Guarda o magic number
  EEPROM.put(4, m);       // Guarda o array
  EEPROM.commit();
}


bool read_eeprom() {
  EEPROM.begin(32);

  uint32_t magicLido;
  EEPROM.get(0, magicLido);

  if (magicLido != MAGIC) {
    Serial.println("Sem calibração válida. Usar valores default.");
    return false;
  }
  Serial.println("Calibração válida encontrada:");

  EEPROM.get(4, m);
  for (int i = 0; i < 4; i++)  Serial.printf("m[%d] = %.1f\n", i, m[i]);

  return true;
}


void setup_sensors()
{
  //Inicializa matriz de velostat
  for(int n = 0; n < 2; n++){
    pinMode(Line[n], OUTPUT);
    digitalWrite(Line[n], LOW);
  }
}

void calibration(char a)
{
    //Start
    if(a == 's')  // Prepara Gmax e Gmin, e usa a flag para informar que a gravação deve ser feita
    {
      calibrating = true;
      digitalWrite(LED_BUILTIN, HIGH); 
      for(int n = 0; n < 4; n++)  { Gmax[n] = 0.0; Gmin[n] = 9999; }
    }

   //End
    if(a == 'e')  // Calcula o m,b que mapeia (Gmin, Gmax) para (-10, 10), embora depois o b seja continuamente ajustado para manter G ~= 0
    {
      calibrating = false;
      if(!is_wifi_on()) digitalWrite(LED_BUILTIN, LOW); 
      for(int n = 0; n < 4; n++)
      {
        m[n] = 2000.0 / (Gmax[n] - Gmin[n]);
        b[n] = 1000.0 - m[n] * Gmax[n]; //Apenas para manter a funçao continua, pois b é ajustado dinamicamente
        Serial.printf("// m[%d] = %.3f\n", n, m[n]);

        save_eeprom();
      }
    }
}

// Lê os valores de condutância. Único caso em que o vetor G tem que ser percorrido como matriz
void Read_G()
{
  for(int l = 0; l < 2; l++)
    {
      digitalWrite(Line[l], HIGH);

      for(int c = 0; c < 2; c++)
      { 
        float v = analogRead(Col[c]);
        float w = v/(4096.0-v);
        float g = w * (Gc + G[2*c+(1-l)]); // w * (soma dos outros G)
        G[2*c+l]  = 0.8 * G[2*c+l]  + 0.2 * g;

        // Regista max e min para ao se premir "ce" fazer os cálculos
        if(calibrating){
          if(G[2*c+l]  > Gmax[2*c+l] )  Gmax[2*c+l]  = G[2*c+l] ;
          if(G[2*c+l]  < Gmin[2*c+l] )  Gmin[2*c+l]  = G[2*c+l] ;
        }
      }
      digitalWrite(Line[l], LOW);
    }
}

// Transoforma condutância em pressão normalizada e com DC = 0
float pressure(int n)
{
  // p = mG + b = 0 -> b = 0 - mG é o valor de b que colocaria imediatamente p = 0
  float b_aux = 0 - (m[n]*G[n]);

  // Faz LPF(b), fazendo com que p tenda lentamente a 0. Efeito semelhante a um HPF(p) em que se remove o DC
  const float Afilt = 0.90;
  b[n] = Afilt * b[n] + (1-Afilt) * b_aux;  
  
  float p = m[n]*G[n] + b[n]; 
  return p;    
}

// Verifica se há algum evento, isto é, se um sinal ultrapassou o threshold
// Se sim, espera que esse sinal atinja o máximo, guarda esse máximo, e retorne abaixo do thresh
void detect_event(long time, int n, float p)
{
  if(calibrating)  return;

  //Para evitar eventos repetidos no mesmo n e com o mesmo sinal (+ ou -) no espaço de 5s
  static int timer_block[4] = {0, 0, 0, 0};
  static int  sign_block[4] = {0, 0, 0, 0};

  //Ignora eventos consecutivos e de mesma polaridade no espaço de 5s
  if(time < timer_block[n] + 5000 && sign_block[n]*p > 0)  return;
  
  // flag = 0 : À espera de algum p > thresh;
  // flag = 2 : p[n] > t e está à espera do max(p[n])
  // flag = 1 : p[n] já passou pelo max, e está à espera que p[n] < thresh
  static int flag = 0;

  static int n_in_event;
  static float p_prev;
  const float thresh = 100;


  // |p| ultrapassou o thresh => início de um evento
  if(flag == 0 && abs(p) > thresh)
  {
    flag = 2;
    n_in_event = n; // Só vamos olhar para o Pn e ignorar os outros 3 sinais
    p_prev = p;
    return;
  }

  if(n_in_event != n) return;

  // |p| < |p_prev| => tivemos um máximo em p_prev => Registar evento
  if(flag == 2 && abs(p) < abs(p_prev))
  {
    flag = 1; //Já passámos o máximo, mas |p| ainda não regressou abaixo de thresh
    events[i_evt][0] = time/1000; //segundos
    events[i_evt][1] = n_in_event;
    events[i_evt][2] = p_prev;
    
    Serial.printf("\n//t = %d ; P%d = %d\n", events[i_evt][0], events[i_evt][1], events[i_evt][2]);
    if(i_evt < N_events-1) i_evt++;

    timer_block[n] = time;

    if(p_prev > 0) sign_block[n] = 1;
    else sign_block[n] = -1;

    ///Com o ignore eventos consecutivos, podemos terminá-lo logo aqui,
    // pois a curva a descer até ao thresh na causará novo evento
    flag = 0;
    n_in_event = -1;
  }

  // |p| voltou a estar abaixo de thresh => Prepara para um novo evento
  if(flag == 1 && abs(p) < thresh)
  {
    flag = 0;
    n_in_event = -1;  
    Serial.printf("//Fim do evento %d\n", n_in_event);
  }
  p_prev = p;
}

