// C++ code
//

void set_timer(int baud_rate){
  int frequencia;
  frequencia = constrain(baud_rate,1,1500);
  //set timer1 interrupt
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  //OCR1A = contador;// = (16*10^6) / (10*1024) - 1 (must be <65536)
  OCR1A = ((16 * pow(10,6))/(1024*frequencia)) - 1;
  // turn on CTC mode (clear time on compare)
  TCCR1B |= (1 << WGM12);
  // Turn T1 off
  TCCR1B &= 0xF8;
  // Disable interrupts
  TIMSK1 = 0;
  TIFR1 = 0;
}

void start_timer(){
  Serial.println("T1 started");
  TCNT1 = 0;//initialize counter value to 0
  TIFR1 = 0;
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  // Set CS10 and CS12 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);
}

void stop_timer(){
  Serial.println("T1 stopped");
    // Turn T1 off
  TCCR1B &= 0xF8;
  TIMSK1 = 0;
}

// Pinos
#define PIN_RX 13
#define PIN_TX 13

// Baud rate
#define BAUD_RATE 1
#define HALF_BAUD 1000/(2*BAUD_RATE)

// Paridade
#define IS_ODD true

// String para guardar a mensagem
String msg = "Hello";

// Para percorrer a string
int iterator = 0;
int msg_length = msg.length();

// Usado para verificar se está atualmente em conexão
bool connected = false;
bool msg_sent = false;
bool first_iteration = true;

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

// Rotina de interrupcao do timer1
// O que fazer toda vez que 1s passou?
ISR(TIMER1_COMPA_vect){
  if(first_iteration == false){
    // Envio de um caractere
    if(iterator < msg_length){
      Serial.write(msg[iterator]);
      ++iterator;
    }

    // Finalização da conexão
    else{
      Serial.write(get_parity_bit(msg, IS_ODD));
      Serial.write(-1);
      iterator = 0;
      stop_timer();
    }
  }else{
    first_iteration = false;
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
  pinMode(PIN_TX, OUTPUT);
  
  // Configura timer
  set_timer(BAUD_RATE);
  
  // habilita interrupcoes
  interrupts();
}

// O loop() eh executado continuamente (como um while(true))
void loop ( ) {
  
  // Envio de RTS
  if(msg_sent == false){
    Serial.println("RTS");
    msg_sent = true;
  }
  
  // Estabelecimento de conexão
  else if(connected == false && Serial.available() > 0){ 
    connected = true;
    start_timer();
    Serial.flush();
  }
}