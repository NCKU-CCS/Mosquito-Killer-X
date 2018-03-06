#define CLK 5//pins definitions for TM1637 and can be changed to other ports
#define DIO 6

volatile uint32_t lastInterrupt = 0;
volatile uint32_t length = 0;
volatile bool interrupted = 0;
volatile bool eventStart = 0;

uint32_t count = 0;

void setup(){
  Serial.begin(9600);
  Serial1.begin(9600);
  
  Serial.println("Started");

  pinMode(7, INPUT);
  /*4(external interrupt) is correspond to d7 pin*/
}


uint32_t pulseStart = 0;
bool pulseEnter = 0;

void loop(){
  if(digitalRead(7)==0) 
  {
    while(digitalRead(7)==1){};
    while(digitalRead(7)==0){};
    count++;
    Serial.print(count);Serial.print(",");Serial.println(length);
    Serial1.print(count);
    Serial1.flush();
  }
}
