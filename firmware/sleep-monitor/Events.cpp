#include <Arduino.h>
#include "Events.h"

// Eventos
long events[N_events][3];
int i_evt = 1; //Porque testamos o evento anterior

void print_events()
{
  Serial.printf("[\n");
  for(int n = 0; n < i_evt; n++)  Serial.printf("%d %d %d; ", events[n][0], events[n][1],events[n][2]);
  Serial.printf("\n];\n");
}
