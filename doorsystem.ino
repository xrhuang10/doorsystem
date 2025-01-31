//COPY PASTE INTO ARDUINO IDE
#include <Arduino.h>

#define PLAYER_WAIT_TIME 5000 

byte sequence[100];           
byte  curLen = 0;             
byte inputCount =  0;      
byte lastInput = 0;          
byte  expRd = 0;             
bool  btnDwn = false;   
bool wait =  false;   
bool  resetFlag = false;       
bool gameStarted = false;
byte winningScore = 3;

byte TRIG_PIN = 4;
byte ECHO_PIN = 3;
byte LOSE_PIN = 5;

byte soundPin = 8;   

#define NOTE_C5  523
#define NOTE_E5  659
#define NOTE_G5  784
#define NOTE_C6  1047

int melody[] = { NOTE_C5, NOTE_E5, NOTE_G5, NOTE_C6 };
int noteDurations[] = { 200, 200, 200, 400 };  


byte noPins =  4;             
                               
byte pins[] = {10, 13, 12, 11};  
                             
                              
long inputTime = 0;         

void setup()  {
  delay(1000);               
  Serial.begin(9600);      
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  Reset();
  
}


void setPinDirection(byte dir){
  for(byte i = 0; i < noPins; i++){
    pinMode(pins[i], dir); 
  }
}

void writeAllPins(byte val){
  for(byte  i = 0; i < noPins; i++){
    digitalWrite(pins[i], val); 
  }
}


void beep(byte freq){
  analogWrite(soundPin,  2);
  delay(freq);
  analogWrite(soundPin, 0);
  delay(freq);
}


void flash(short freq){
  setPinDirection(OUTPUT); 
  for(int i = 0; i < 3; i++){
    writeAllPins(HIGH);
    beep(50);
    delay(freq);
    writeAllPins(LOW);
    delay(freq);
  }
}


void Reset(){
  flash(500);
  curLen = 0;
  inputCount  = 0;
  lastInput = 0;
  expRd = 0;
  btnDwn = false;
  wait = false;
  resetFlag = false;
  gameStarted = false;
}


void Lose(){
  delay(1000);
  digitalWrite(LOSE_PIN, HIGH); 
  tone(soundPin, 220, 500);
  delay(1000);
  noTone(soundPin);
  digitalWrite(LOSE_PIN, LOW);
}


void playSequence(){
 
  for(int i = 0; i < curLen; i++){
      Serial.print("Seq: ");
      Serial.print(i);
      Serial.print("Pin: ");
      Serial.println(sequence[i]);
      digitalWrite(sequence[i],  HIGH);
      delay(500);
      digitalWrite(sequence[i], LOW);
      delay(250);
    } 
}


void DoLoseProcess(){
  Lose();          
  playSequence(); 
  delay(1000);
  Reset();   
  
  gameStarted = false;
}



#define SEG_A A0
#define SEG_B A1
#define SEG_C A2
#define SEG_D A3
#define SEG_E A4
#define SEG_F A5
#define SEG_G 2


#define DIGIT_PIN 1

const byte numbers[10][7] = {
  {1, 1, 1, 1, 1, 1, 0}, // 0
  {0, 1, 1, 0, 0, 0, 0}, // 1
  {1, 1, 0, 1, 1, 0, 1}, // 2
  {1, 1, 1, 1, 0, 0, 1}, // 3
  {0, 1, 1, 0, 0, 1, 1}, // 4
  {1, 0, 1, 1, 0, 1, 1}, // 5
  {1, 0, 1, 1, 1, 1, 1}, // 6
  {1, 1, 1, 0, 0, 0, 0}, // 7
  {1, 1, 1, 1, 1, 1, 1}, // 8
  {1, 1, 1, 1, 0, 1, 1}  // 9
};


void displayDigit(int num) {

  digitalWrite(SEG_A, LOW);
  digitalWrite(SEG_B, LOW);
  digitalWrite(SEG_C, LOW);
  digitalWrite(SEG_D, LOW);
  digitalWrite(SEG_E, LOW);
  digitalWrite(SEG_F, LOW);
  digitalWrite(SEG_G, LOW);
  
 
  digitalWrite(SEG_A, numbers[num][0]);
  digitalWrite(SEG_B, numbers[num][1]);
  digitalWrite(SEG_C, numbers[num][2]);
  digitalWrite(SEG_D, numbers[num][3]);
  digitalWrite(SEG_E, numbers[num][4]);
  digitalWrite(SEG_F, numbers[num][5]);
  digitalWrite(SEG_G, numbers[num][6]);

  
  digitalWrite(DIGIT_PIN, HIGH);  
  delay(5);
  digitalWrite(DIGIT_PIN, LOW);   
}

void clearDisplay() {
  digitalWrite(SEG_A, LOW);
  digitalWrite(SEG_B, LOW);
  digitalWrite(SEG_C, LOW);
  digitalWrite(SEG_D, LOW);
  digitalWrite(SEG_E, LOW);
  digitalWrite(SEG_F, LOW);
  digitalWrite(SEG_G, LOW);
}

void victory() {

  int sequence[] = {3, 5, 5, 9};  
  
  for (int i = 0; i < 4; i++) {
    flash(50);
    tone(soundPin, melody[i], noteDurations[i]); 
    delay(noteDurations[i] * 1.3); 
    displayDigit(sequence[i]);  
    delay(500);                 
    clearDisplay();          
    delay(250);              
  }
  noTone(soundPin);
}

void  loop() {  
  long duration, distance;

  digitalWrite(TRIG_PIN, LOW);  
  delayMicroseconds(2);  
  digitalWrite(TRIG_PIN, HIGH);  
  delayMicroseconds(10); 
  digitalWrite(TRIG_PIN, LOW); 
  
  duration = pulseIn(ECHO_PIN, HIGH);
  distance = (duration / 2) / 29.1;
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  delay(500);


  bool victoryPlayed = false;
  while (distance <= 100 && !gameStarted){
    if (inputCount == winningScore && victoryPlayed == false){
      victory();
      victoryPlayed = true;
      inputCount = 0;
      break;
    }

    if(!wait){      
                            
    setPinDirection(OUTPUT);                   
    
    randomSeed(analogRead(A0));                 
    sequence[curLen] = pins[random(0,noPins)];   
    curLen++;                              
    
    playSequence();                         
    beep(50);                      
    
    wait = true;         
    inputTime  = millis();                    
  }else{ 
                            
    setPinDirection(INPUT);                      

    if(millis() - inputTime  > PLAYER_WAIT_TIME){  
      DoLoseProcess();                          
      return;
    }      
        
    if(!btnDwn){                                  
      expRd  = sequence[inputCount];            
     
      
      for(int i = 0; i < noPins; i++){         
        if(pins[i]==expRd)                        
          continue;                             
        if(digitalRead(pins[i]) == HIGH){         
          lastInput = pins[i];
          resetFlag = true;                      
          btnDwn = true;                      
          
        }
      }      
    }

    if(digitalRead(expRd) ==  1 && !btnDwn)     
    {
      inputTime  = millis();                       
      lastInput = expRd;
      inputCount++;                              
      btnDwn = true;                             
      
    }else{
      if(btnDwn && digitalRead(lastInput) == LOW){  
        btnDwn = false;
        delay(20);
        if(resetFlag){                            
          DoLoseProcess();                   
        }
        else{
          if(inputCount == curLen){              
            wait = false;                        
            inputCount  = 0;                        
            delay(1500);
          }
        }
      }
    }    
  }
  }

}

