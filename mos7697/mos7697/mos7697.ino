#define CLK 5//pins definitions for TM1637 and can be changed to other ports
#define DIO 6

volatile uint32_t lastInterrupt = 0;
volatile uint32_t length = 0;
volatile bool interrupted = 0;
volatile bool eventStart = 0;

uint32_t count = 0;

void pinChanged(){
  
  if(!digitalRead(2)){
    if(millis()-lastInterrupt>100){
      lastInterrupt = millis();
      eventStart = 1;
    }
    
  }
  else{
    if(!interrupted && eventStart==1){
      length = millis()-lastInterrupt;
      interrupted = 1;
      eventStart = 0;
    }
  }

}

void setup(){
  Serial.begin(9600);
  
  Serial.println("Started");

  pinMode(2, INPUT);
  attachInterrupt(2,pinChanged, CHANGE);
  /*pin 2*/
}


uint32_t pulseStart = 0;
bool pulseEnter = 0;

void loop(){
  
  if(interrupted){
      count++;
      Serial.println(count);
      
      interrupted = 0;
  }
}