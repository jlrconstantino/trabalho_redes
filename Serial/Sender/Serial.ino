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
#define PIN_CTS 12
#define PIN_RTS 11

// Baud rate
#define BAUD_RATE 1
#define HALF_BAUD 1000/(2*BAUD_RATE)

// Paridade
#define IS_ODD true

// String para guardar a mensagem
String msg = "Hello";

// Para percorrer a string
int char_iterator = 0;
int bit_iterator = -1;
int msg_length = msg.length();

// Controle lógico
bool connected = false;
bool msg_sent = false;
bool send_end_bit = false;

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

// Rotina de interrupcao do timer1
// O que fazer toda vez que 1s passou?
ISR(TIMER1_COMPA_vect){
  
  // Itera ao longo dos caracteres
  if(char_iterator < msg_length){
    
    // Bit inicializador
    if(bit_iterator == -1){
      digitalWrite(PIN_TX, HIGH);
      bit_iterator = 0;
    }
    
    // Itera ao longo dos bits
    else if(bit_iterator < 8){
      digitalWrite(PIN_TX, bitRead(msg[char_iterator], bit_iterator));
      ++bit_iterator;
    }
    
    // Bit de paridade
    else if(send_end_bit == false){
      digitalWrite(PIN_TX, get_parity_bit(msg[char_iterator], IS_ODD));
      send_end_bit = true;
    }
    
    // Bit terminador
    else {
      digitalWrite(PIN_TX, LOW);
      send_end_bit = false;
      ++char_iterator;
      bit_iterator = -1;
    }
  }
  
  // Encerra comunicação
  else{
    //connected = false;
    //msg_sent = false;
    char_iterator = 0;
    digitalWrite(PIN_RTS, LOW);
    stop_timer();
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
  
  // RTS e CTS
  pinMode(PIN_CTS, INPUT);
  pinMode(PIN_RTS, OUTPUT);
  digitalWrite(PIN_RTS, LOW);
  
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
    digitalWrite(PIN_RTS, HIGH);
    msg_sent = true;
  }
  
  // Estabelecimento de conexão
  else if(connected == false && digitalRead(PIN_CTS) == HIGH){ 
    connected = true;
    start_timer();
  }
}