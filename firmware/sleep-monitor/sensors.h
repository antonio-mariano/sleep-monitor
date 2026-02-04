#pragma once

#include "Communication.h"
// Calibração

/***** Funções ******/

bool read_eeprom();

void calibration(char a);

// Lê os valores de condutância. Único caso em que o vetor G tem que ser percorrido como matriz
void Read_G();

// Transoforma condutância em pressão normalizada e com DC = 0
float pressure(int n);


// Verifica se há algum evento, isto é, se um sinal ultrapassou o threshold
// Se sim, espera que esse sinal atinja o máximo, guarda esse máximo, e retorne abaixo do thresh
void detect_event(long time, int n, float p);

void setup_sensors();

