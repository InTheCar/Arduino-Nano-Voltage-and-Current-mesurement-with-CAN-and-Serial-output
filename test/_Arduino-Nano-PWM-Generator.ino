

//Debug over UART settings
#define DebugOnOffPin 5 // bridge or switch between this digital pin and GND. If the bridge is detected the serial (UART) will be initialized.

//Debug definitions
#define DebugActive 0 // If the digitalRead returns a 0 the bridge is connected
#define DebugInactive 1
int StoredDebugSwitchState = DebugInactive;
#define Serial_Speed 38400
//9600, 38400, 115200

//Rotary Switche
int SW1_PIN = A1;
int SW2_PIN = A2;

//blinking LED
#include <TimerOne.h>
#define PIN_LED 6 // Build in LED
#define on 1 // for LEDstatus, on off blinking
#define off 0 // for LEDstatus, on off blinking
#define blinking 2 // for LEDstatus, on off blinking

volatile int LEDstatus = on;   // global variable also accessable from
                                // interrupt rutine led_handle()
                                // volatile is nessesary to be sure that the variable
                                // is loaded from the RAM and not from a storage register

//Button PIN
#define Button_PIN 2

void ButtonPressed(void);
void led_handle(void);
void serialEvent();// Read serial Interface but do nothing special
void debug_interface(void);//Read switch state and enable or disable debug interface
void debug(String);
volatile static int SW1_saved_switch_position;
volatile static int SW1_actual_switch_position;

volatile static int SW2_saved_switch_position;
volatile static int SW2_actual_switch_position;

//PWM signals
void PWM_OFF(void);
void PWM_50_50(void);
void PWM_100_50(void);
void PWM_150_50(void);
 

void setup() {
  // for debug interface
  Serial.end(); //set serial in a defined status
  StoredDebugSwitchState = DebugInactive;
  pinMode(DebugOnOffPin, INPUT_PULLUP); //initial PIN for debug switch with internal pullup resistant
  debug_interface(); // Check if debug is actice or not

  //blinking LED
  TCCR2A = 0x00; // Wave Form Generation Mode 0: Normal Mode; OC2A disconnected
  TCCR2B = (1<<CS22) + (1<<CS21) + (1<<CS20); // prescaler = 1024
  TIMSK2 = (1<<TOIE2); // interrupt when TCNT2 is overflowed
  DDRD |= (1<<PD7);  // Portmanipulation: replaces pinMode(7, OUTPUT);

  pinMode(PIN_LED, OUTPUT); // initialise the pin for the LED as output
  //Timer1.initialize(100000); // call every 0,1s the interrupt rutine
  //Timer1.attachInterrupt(led_handle); // use led_handle as interrupt rutine

  //button set
  
  pinMode(Button_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(Button_PIN), ButtonPressed, CHANGE);

  SW1_saved_switch_position=get_switch_11_level(SW1_PIN);
  SW1_actual_switch_position =SW1_saved_switch_position;
  
  SW2_saved_switch_position=get_switch_11_level(SW2_PIN);
  SW2_actual_switch_position =SW2_saved_switch_position;
  PWM_50_50;
}
//----------------------------------------------------------------------------------------------------
// start loop
void loop() {
  // for rotary switches SW1 and SW2

  SW1_actual_switch_position=get_switch_11_level(SW1_PIN);
  SW2_actual_switch_position=get_switch_11_level(SW2_PIN);

//  debug("SW1 and SW2 position and LED: " + String(SW1_actual_switch_position) + " " + String(SW2_actual_switch_position) + " " + String(LEDstatus));
//  debug(String(SW1_actual_switch_position));
  if(SW1_actual_switch_position != SW1_saved_switch_position || SW2_actual_switch_position != SW2_saved_switch_position){
    //debug("SW1 or SW2 position != Stored Position");
    LEDstatus = blinking;

  }
  else{
    LEDstatus = on;

  }

  
  debug_interface();  
}



//----------------------------------------------------------------------------------------------------
// Read serial Interface but do nothing special
void serialEvent() {
 int incomingByte;
 incomingByte = Serial.read();  // Statement(s)
 Serial.println("no keyboard commands implemented");
}
//----------------------------------------------------------------------------------------------------
void debug_interface(void){
  int ActualDebugSwitchState = digitalRead(DebugOnOffPin);
  if(StoredDebugSwitchState == ActualDebugSwitchState){ return; }

  switch(ActualDebugSwitchState) {
    case DebugInactive: {
      debug("Debug will be disabled");
      Serial.end();      
      break;
      }
    case DebugActive: {
      Serial.begin(Serial_Speed);
      debug("Debug enabled");
      break;
    }
    default: {
      debug("default state unknown");
      break;
    }
  } 
  StoredDebugSwitchState = ActualDebugSwitchState;
}
//----------------------------------------------------------------------------------------------------
void debug(String debub_string_to_uart){
 //if (StoredDebugSwitchState == DebugActive){
 Serial.println(String(Serial.availableForWrite()));
 Serial.println(debub_string_to_uart);
 //}
}
//----------------------------------------------------------------------------------------------------
int get_switch_11_level(int AD_Wandler){
  int return_value = 0;
  int x = analogRead(AD_Wandler);
  if (x >= 0 && x <= 51){
    return_value = 1;
  }
  else if (x >= 52 && x <= 153){
    return_value = 2;
  }
  else if (x >= 154 && x <= 256){
    return_value = 3;
  }
  else if (x >= 257 && x <= 358){
    return_value = 4;
  }
  else if (x >= 359 && x <= 460){
    return_value = 5;
  }
  else if (x >= 461 && x <= 563){
    return_value = 6;
  }
  else if (x >= 564 && x <= 665){
    return_value = 7;
  }
  else if (x >= 666 && x <= 767){
    return_value = 8;
  }
  else if (x >= 768 && x <= 870){
    return_value = 9;
  }
  else if (x >= 871 && x <= 972){
    return_value = 10;
  }
  else if (x >= 973 && x <= 1023){
    return_value = 11;
  }
return(return_value);
}
//----------------------------------------------------------------------------------------------------
ISR(TIMER2_OVF_vect){  // Interrupt Service Routine for LED
  static int counter = 0;
  counter++;
  if(counter>=10){
    debug("ISR Counter reached LEDstatus: " + String(LEDstatus));
//    PORTD ^= (1<<PD7); 
    if (LEDstatus == blinking){
      if (digitalRead(PIN_LED) == LOW) {
        debug("was low");
        digitalWrite(PIN_LED, HIGH);
    } else {
      debug("was High");
      digitalWrite(PIN_LED, LOW);
    }
    }
     else if (LEDstatus == off){
        if (digitalRead(PIN_LED) == HIGH) {
        digitalWrite(PIN_LED, LOW);
       }
      }
    else if (LEDstatus == on){
      if (digitalRead(PIN_LED) == LOW) {
        digitalWrite(PIN_LED, HIGH);
      }
    }

    counter = 0; 
  }
}

void led_handle(void){
  debug("LED status in led_handle: " + String(LEDstatus));
  if (LEDstatus == blinking){
    if (digitalRead(PIN_LED) == LOW) {
      digitalWrite(PIN_LED, HIGH);
    } else {
      digitalWrite(PIN_LED, LOW);
    }
  }
  else if (LEDstatus == off){
    if (digitalRead(PIN_LED) == HIGH) {
      digitalWrite(PIN_LED, LOW);
    }
  }
  else if (LEDstatus == on){
    if (digitalRead(PIN_LED) == LOW) {
      digitalWrite(PIN_LED, HIGH);
    }
  }
}
//----------------------------------------------------------------------------------------------------
void ButtonPressed(void){
  debug("button falling");
  SW1_actual_switch_position = get_switch_11_level(SW1_PIN);
  SW2_actual_switch_position = get_switch_11_level(SW2_PIN);
  debug("SW1 state: " + String(SW1_actual_switch_position));
  switch(SW1_actual_switch_position) {
    case 1: {
      debug("set PWM_OFF");
      PWM_OFF();
      SW1_saved_switch_position=SW1_actual_switch_position;
      SW2_saved_switch_position=SW2_actual_switch_position;
      LEDstatus = on;

      break;
      }
    case 2: {
      debug("set PWM_50_50");
      PWM_50_50();
      SW1_saved_switch_position=SW1_actual_switch_position;
      SW2_saved_switch_position=SW2_actual_switch_position;
      LEDstatus = on;
      break;
      }
    case 3: {
      debug("set PWM_100_50");
      PWM_100_50();
      SW1_saved_switch_position=SW1_actual_switch_position;
      SW2_saved_switch_position=SW2_actual_switch_position;
      LEDstatus = on;
      break;
      }
    case 4: {
      debug("set PWM 150_50");
      PWM_150_50();
      SW1_saved_switch_position=SW1_actual_switch_position;
      SW2_saved_switch_position=SW2_actual_switch_position;
      LEDstatus = on;
      break;
      }
    default: {
      debug("SW1 switch position not defined");
      break;
    }
  } 


  LEDstatus = on;
}

//----------------------------------------------------------------------------------------------------
void PWM_OFF(){
  TCCR1A = (0<<COM1A1)+(0<<COM1A0) + (0<<COM1B1)+(0<<COM1B0) + (0<<WGM11)+ (0<<WGM10); 
  TCCR1B = (0<<ICNC1)+(0<<ICES1)+(0<<WGM13)+ (0<<WGM12)+ (0<<CS12)+ (0<<CS11)+ (0<<CS10); 
  ICR1 = 0;
  OCR1A = 0;
  DDRB |= (1<<PB1);
}
void PWM_50_50(){
  TCCR1A = (1<<COM1A1)+(0<<COM1A0) + (0<<COM1B1)+(0<<COM1B0) + (1<<WGM11)+ (0<<WGM10); 
  TCCR1B = (0<<ICNC1)+(0<<ICES1)+(1<<WGM13)+ (1<<WGM12)+ (0<<CS12)+ (1<<CS11)+ (1<<CS10); 
  ICR1 = 24999;
  OCR1A = 12499;
  DDRB |= (1<<PB1);
}
void PWM_100_50(){
  TCCR1A = (1<<COM1A1)+(0<<COM1A0) + (0<<COM1B1)+(0<<COM1B0) + (1<<WGM11)+ (0<<WGM10); 
  TCCR1B = (0<<ICNC1)+(0<<ICES1)+(1<<WGM13)+ (1<<WGM12)+ (0<<CS12)+ (1<<CS11)+ (1<<CS10); 
  ICR1 = 37499;
  OCR1A = 25000;
  DDRB |= (1<<PB1);
}
void PWM_150_50(){
  TCCR1A = (1<<COM1A1)+(0<<COM1A0) + (0<<COM1B1)+(0<<COM1B0) + (1<<WGM11)+ (0<<WGM10); 
  TCCR1B = (0<<ICNC1)+(0<<ICES1)+(1<<WGM13)+ (1<<WGM12)+ (1<<CS12)+ (0<<CS11)+ (1<<CS10); 
  ICR1 = 3124;
  OCR1A = 2343;
  DDRB |= (1<<PB1);
}
