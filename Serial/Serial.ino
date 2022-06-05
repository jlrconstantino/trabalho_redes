//////////////////////////////// INFO /////////////////////////////////////////
/*
*
* Implementação das funcionalidades de controle do Arduino.
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
#define PIN_CTS 12
#define PIN_RTS 11

// Baud rate
#define BAUD_RATE 1
#define HALF_BAUD 1000/(2*BAUD_RATE)

// Paridade
#define IS_ODD true


/////////////////////////// VARIÁVEIS GLOBAIS /////////////////////////////////

// Dados recebidos via comunicação
int bit_received;
char byte_received = 0;
int parity_received;

// Controle lógico
bool connected = false;
int bit_iterator = -1;


////////////////////////// FUNÇÕES AUXILIARES /////////////////////////////////

// Calcula bit de paridade - Par ou impar
int get_parity_bit(char data, bool is_odd){

  // Conta a quantia de 1's
  int count = 0;
  for(int i = 0; i < 8; ++i){
    count += bitRead(data, i);
  }

  // Ímpar
  if(is_odd){
    if(count % 2 == 0){
      return HIGH;
    }
    return LOW;
  }

  // Par
  else {
    if(count % 2 == 0){
      return LOW;
    }
    return HIGH;
  }
}


////////////////////// CHAMADAS-PADRÃO DO ARDUINO /////////////////////////////


// Executada uma vez quando o Arduino reseta
void setup(){
  
  //desabilita interrupcoes
  noInterrupts();
  
  // Configura porta serial (Serial Monitor - Ctrl + Shift + M)
  Serial.begin(9600);
  
  // Inicializa TX ou RX
  pinMode(PIN_RX, INPUT);
  
  // RTS e CTS
  pinMode(PIN_CTS, OUTPUT);
  digitalWrite(PIN_CTS, LOW);
  pinMode(PIN_RTS, INPUT);
  
  // Configura timer
  set_timer(BAUD_RATE);
  
  // habilita interrupcoes
  interrupts();
}

// O loop() eh executado continuamente (como um while(true))
void loop ( ) {
  
  // Verifica se há conexão e se há mensagem
  if(connected == false && digitalRead(PIN_RTS) == HIGH){
    
    // Comunicação (CTS)
    Serial.println("CTS");
    digitalWrite(PIN_CTS, HIGH);
    connected = true;
    delay(HALF_BAUD);
    start_timer();
  }
}

/////////////////////// LÓGICA DE SINCRONIZAÇÃO ///////////////////////////////

// Rotina de interrupcao do timer1
// O que fazer toda vez que 1s passou?
ISR(TIMER1_COMPA_vect){
  
  // Bit de inicialização (ignorado)
  if(bit_iterator == -1){
    bit_iterator = 0;
  }
  
  // Executa se o RTS ainda estiver definido
  else if(digitalRead(PIN_RTS) == HIGH){
    
    // Itera ao longo dos bits a serem recebidos
    if(bit_iterator < 8){
      
      // Bit atual
      bit_received = digitalRead(PIN_RX);
      bitWrite(byte_received, 7-bit_iterator, bit_received);
      Serial.write(bit_received + 48);
      
      // Incremento do iterador
      ++bit_iterator;
    }

    // Bit de paridade
    else if(bit_iterator < 9){
      
      // Bit atual
      bit_received = digitalRead(PIN_RX);
      
      // Início da mensagem a ser impressa
      Serial.print(" (");
      Serial.print(byte_received);
      
      // Verificação de paridade
      if(get_parity_bit(byte_received, IS_ODD) == bit_received){
        Serial.print("). Parity is correct: expected and received ");
        Serial.print(bit_received);
      }else{
        Serial.print("). Parity is INCORRECT: expected ");
        Serial.print(!bit_received);
        Serial.print(", but received ");
        Serial.print(bit_received);
      }
      
      // Final da mensagem a ser impressa
      Serial.println(".");
      
      // Incremento do iterador
      ++bit_iterator;
    }
    
    // Bit terminador
    else{
      bit_iterator = -1;
    }
  }
  
  // Finaliza a conexão
  else{
    digitalWrite(PIN_CTS, LOW);
    stop_timer();
    connected = false;
  }
}