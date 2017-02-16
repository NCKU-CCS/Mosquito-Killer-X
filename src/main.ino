int SER_Pin = A5;   
int RCLK_Pin = A4;  
int SRCLK_Pin = A3; 
int Digital_Pin = 2;
byte digit[4] ={0};
byte Digicount = 0;

byte seven_seg_digits[10] = { B00000011,  // = 0
                              B10011111,  // = 1
                              B00100101,  // = 2
                              B00001101,  // = 3
                              B10011001,  // = 4
                              B01001001,  // = 5
                              B01000001,  // = 6
                              B00011111,  // = 7
                              B00000001,  // = 8
                              B00011001   // = 9
                             };

                             
volatile uint32_t lastInterrupt = 0;
volatile uint32_t length = 0;
volatile bool interrupted = 0;
volatile bool eventStart = 0;

void pinChanged(){
  //Serial.println("INT");
  if(!digitalRead(Digital_Pin)){
    //Serial.println("Low");
    //要超過500ms
    if(millis()-lastInterrupt>500){
      lastInterrupt = millis();
      eventStart = 1;//蚊子被電死:事件發生
    }
    
  }
  else{
    //Serial.println("High");
    if(!interrupted && eventStart==1){
      length = millis()-lastInterrupt;
      interrupted = 1;
      eventStart = 0;
    }
  }
}

void setup(){
  Serial.begin(115200);
  //Serial1.begin(115200);
  //while(!Serial);
  pinMode(A5,OUTPUT);
  pinMode(A4,OUTPUT);
  pinMode(A3,OUTPUT);

  
  Serial.println("Started");

  pinMode(Digital_Pin, INPUT);
  //當pin2改變時發interrupt ,執行change
  attachInterrupt(1, pinChanged, CHANGE);

}

uint32_t count = 0;
uint32_t pulseStart = 0;
bool pulseEnter = 0;

void loop(){

updateDisplay();


  if(interrupted){
      count++;
      //send to mcs
      Serial.print(count);Serial.print(",");Serial.println(length);
      //Serial1.print(count);Serial1.print(",");Serial1.println(length);

      uint8_t disp = count%10000;
    digit[3] = disp/1000;
      disp = disp%1000;
    digit[2] = disp/100;
      disp = disp%100;
    digit[1] = disp/10;
      disp = disp%10;
    digit[0] = disp;
      
      interrupted = 0;
  }
}


void updateDisplay(){

    Digicount = (Digicount+1)%4;
    digitalWrite(RCLK_Pin, LOW);
     shiftOut(SER_Pin, SRCLK_Pin, LSBFIRST, seven_seg_digits[digit[Digicount]]);
     shiftOut(SER_Pin, SRCLK_Pin, MSBFIRST, (uint8_t)1<<Digicount);
    digitalWrite(RCLK_Pin, HIGH);

}

