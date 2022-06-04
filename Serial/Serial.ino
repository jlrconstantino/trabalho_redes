//////////////////////////////// INFO /////////////////////////////////////////
/*
*
* Implementação das funcionalidades de controle do Arduíno.
*
* Autores: 
* Erick Patrick Andrade Barcelos
* Karoliny Oliveira Ozias Silva
* João Lucas Rodrigues Constantino
*
*/
///////////////////////////// IMPORTAÇÕES /////////////////////////////////////

// Bibliotecas-padrão
#include <stdio.h>
#include <stdlib.h>

// Bibliotecas locais
#include "./Timer.h"


/////////////////////////////// MACROS ////////////////////////////////////////

#define PINO_RX 13
#define PINO_TX 13
#define BAUD_RATE 1
#define HALF_BAUD 1000/(2*BAUD_RATE)


/////////////////////////////// FUNÇÕES ///////////////////////////////////////

// Calcula bit de paridade - Par ou impar
bool bitParidade(char dado){

}

// Rotina de interrupcao do timer1
// O que fazer toda vez que 1s passou?
void ISR(TIMER1_COMPA_vect){
  //>>>> Codigo Aqui <<<<
}

// Executada uma vez quando o Arduino reseta
void setup(){
  //desabilita interrupcoes
  noInterrupts();
  // Configura porta serial (Serial Monitor - Ctrl + Shift + M)
  Serial.begin(9600);
  // Inicializa TX ou RX
  //>>>> Codigo Aqui <<<<
  // Configura timer
  //>>>> Codigo Aqui <<<<
  // habilita interrupcoes
  interrupts();
}

// O loop() eh executado continuamente (como um while(true))
void loop ( ) {
  //>>>> Codigo Aqui <<<<
}
