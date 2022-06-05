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

// Bibliotecas locais
#include "./Timer.h"


/////////////////////////////// MACROS ////////////////////////////////////////

// Pinos
#define PIN_RX 13
#define PIN_TX 13

// Baud rate
#define BAUD_RATE 1
#define HALF_BAUD 1000/(2*BAUD_RATE)

// Paridade
#define IS_ODD true

/////////////////////////// VARIÁVEIS GLOBAIS /////////////////////////////////

// String para guardar mensagem
String msg;

// Byte recebido para comunicação
char byte_received;

// Usado para verificar se está atualmente em conexão
bool connected = false;
bool first_iteraction = true;

/////////////////////////////// FUNÇÕES ///////////////////////////////////////

// Calcula bit de paridade - Par ou impar
char get_parity_bit(char data, bool is_odd){

  // Conta a quantia de 1's
  int count = 0;
  while(data){
    count += data & 1;
    data >>= 1;
  }

  // Ímpar
  if(is_odd){
    if(count % 2 == 0){
      return '0';
    }
    return '1';
  }

  // Par
  else {
    if(count % 2 == 0){
      return '1';
    }
    return '0';
  }
}

// Rotina de interrupcao do timer1
// O que fazer toda vez que 1s passou?
ISR(TIMER1_COMPA_vect){
  if(first_iteraction == false){
  
    // Leitura de um caractere
    byte_received = Serial.read();

    Serial.print("Byte received: ");
    Serial.println(byte_received);

    // Verificação de término de mensagemm
    if(byte_received < 0){
      stop_timer();
    }
  }
  else{
    clear_serial();
    first_iteraction = false;
  }
}

// Limpeza de serial
void clear_serial(){
  while(Serial.available() > 0) {
    char t = Serial.read();
  }
}

// Executada uma vez quando o Arduino reseta
void setup(){
  
  //desabilita interrupcoes
  noInterrupts();
  
  // Configura porta serial (Serial Monitor - Ctrl + Shift + M)
  Serial.begin(9600);
  
  // Inicializa TX ou RX
  pinMode(PIN_RX, INPUT);
  
  // Configura timer
  set_timer(BAUD_RATE);
  
  // habilita interrupcoes
  interrupts();
}

// O loop() eh executado continuamente (como um while(true))
void loop ( ) {
  
  // Verifica se há conexão e se há mensagem
  if(connected == false && Serial.available() > 0){
    
    // Comunicação (CTS)
    Serial.println("CTS");
    connected = true;
    start_timer();
  }
}