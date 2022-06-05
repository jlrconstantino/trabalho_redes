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

// Baud rate
#define BAUD_RATE 1
#define HALF_BAUD 1000/(2*BAUD_RATE)

// Paridade
#define IS_ODD true


/////////////////////////// VARIÁVEIS GLOBAIS /////////////////////////////////

// String para guardar mensagem
String msg = "";

// Byte recebido para comunicação
char byte_received;

// Usado para verificar se está atualmente em conexão
bool connected = false;
bool first_iteration = true;


////////////////////////// FUNÇÕES AUXILIARES /////////////////////////////////

// Calcula bit de paridade - Par ou impar
char get_parity_bit(String message, bool is_odd){

  // Conta a quantia de 1's
  int count = 0, len = message.length();
  char byte;
  for(int i = 0; i < len; ++i){
    byte = message[i];
  	while(byte){
	    count += byte & 1;
	    byte >>= 1;
  	}
  }

  // Ímpar
  if(is_odd){
    if(count % 2 == 0){
      return '1';
    }
    return '0';
  }

  // Par
  else {
    if(count % 2 == 0){
      return '0';
    }
    return '1';
  }
}


// Imprime, na saída Serial, uma string em formato binário
void print_string_as_binary(String message){
  int i, len = message.length() - 1;
  for(i=0; i<len; ++i){
    Serial.print(message[i], BIN);
  }
  Serial.println(message[i], BIN);
}



// Limpeza de serial
void clear_serial(){
  while(Serial.available() > 0) {
    char t = Serial.read();
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
    delay(HALF_BAUD);
    start_timer();
  }
}

/////////////////////// LÓGICA DE SINCRONIZAÇÃO ///////////////////////////////

// Rotina de interrupcao do timer1
// O que fazer toda vez que 1s passou?
ISR(TIMER1_COMPA_vect){
	
  // Sincronização
  if(first_iteration == false){
  
    // Leitura caractere a caractere
    byte_received = Serial.read();
    Serial.print("Byte received: ");
    Serial.println(byte_received);

    // Verificação de término de mensagemm
    if(byte_received < 0){
      
      // Encerra o timer
      stop_timer();
      
      // Bit de paridade
      char received_parity_bit = msg[msg.length() - 1];
      msg[msg.length() - 1] = '\0';
      char expected_parity_bit = get_parity_bit(msg, IS_ODD);
      
      // Verificação de bit de paridade
      if(received_parity_bit == expected_parity_bit){
        Serial.print("Parity bit is correct. Expected and received ");
        Serial.print(expected_parity_bit);
		if(IS_ODD){
          Serial.println(". (ODD PARITY)");
		}else{
          Serial.println(". (EVEN PARITY)");
		}
      }else{
        Serial.print("Parity bit is incorrect. Expected ");
        Serial.print(expected_parity_bit);
        Serial.print(", but received ");
        Serial.print(received_parity_bit);
		if(IS_ODD){
          Serial.println(" instead. (ODD PARITY)");
		}else{
          Serial.println(" instead. (EVEN PARITY)");
		}
      }
      
      // Visualização da mensagem
      Serial.print("Received message: ");
      Serial.println(msg);
      Serial.print("As binary: ");
      print_string_as_binary(msg);
    }
	
	// Concatenação do caractere recebido à mensagem
	else{
      msg.concat(byte_received);
    }
  }
  
  // Limpeza do Serial e liberação da leitura
  else{
    clear_serial();
    first_iteration = false;
  }
}