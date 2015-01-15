// SPI comm library
#include <SPI.h>

// Serial command library
// https://github.com/kroimon/Arduino-SerialCommand
#include <SerialCommand.h>

// encoder library;
// https://www.pjrc.com/teensy/td_libs_Encoder.html
#define ENCODER_DO_NOT_USE_INTERRUPTS
#include <Encoder.h>



// DAC library
#include "AD536x.h"
/***************************************
        Encoder Config
***************************************/
// Encoder pins
#define CH_ENC_B 14
#define CH_ENC_A 15
#define DAC_ENC_B 16
#define DAC_ENC_A 17

// LED pins
#define LED2 18
#define LED1 19
#define LED0 8

Encoder dacEnc(DAC_ENC_A, DAC_ENC_B);
Encoder chEnc(CH_ENC_A, CH_ENC_B);

long dacEncPos = 0;
long chEncPos = 0;

int activeCh = 0;
AD536x_ch_t dacCh[4] = {CH3, CH2, CH1, CH0};  // mapping of physical -> dac channel.
int ledPins[3] = {LED0, LED1, LED2};

/***************************************
        DAC Config
***************************************/
#define CS 9
#define CLR 7
#define LDAC 6
#define RESET 5

AD536x dac(CS, CLR, LDAC, RESET);

// dac channels, one for each led + one extra.
// will need to change in loop if change # of channels...
unsigned int dacPos[4] = {0,0,0,0};


/***************************************
        Serial Config
***************************************/

SerialCommand sCmd;

// callbacks...

void dacDelta(){
  int ch = -1;
  long delta = 0;
  char * arg;

  // parse first argument
  arg = sCmd.next();
  if (arg != NULL){
    ch = atoi(arg);
  }
  
  // parse second argument
  arg = sCmd.next();
  if (arg != NULL){
    delta = atol(arg);
  }
  
  
   // program arduino
  
  // make sure both commands were parsed correctly, and are positive...
  if (ch >= 0 && ch < 4){
    // make sure you don't go out of bounds
    unsigned int oldVal = dac.getDAC(BANK0, dacCh[ch]);
    
    long newVal = delta + (long) oldVal;
     
    if (newVal > 0){
      // update local variable array
      dacPos[ch] = newVal;
    
      // write DAC
      dac.writeDAC(BANK0, dacCh[ch], newVal);
    } else {
     dacPos[ch] = 0;
     dac.writeDAC(BANK0, dacCh[ch], 0); 
    }
  }  else {
    Serial.println("INVALID");
  } 
  
}


void dacIncrease(){
  int ch = -1;
  long delta = -1;
  char * arg;
  
  // parse first argument
  arg = sCmd.next();
  if (arg != NULL){
    ch = atoi(arg);
  }
  
  // parse second argument
  arg = sCmd.next();
  if (arg != NULL){
    delta = atol(arg);
  }
  
  // program arduino
  
  // make sure both commands were parsed correctly, and are positive...
  if (ch >= 0 && ch < 4 && delta >= 0){
    unsigned int newVal = dac.getDAC(BANK0, dacCh[ch]) + delta;
    // update local variable array
    dacPos[ch] = newVal;
    
    // write DAC
    dac.writeDAC(BANK0, dacCh[ch], newVal);
  }  else {
    Serial.println("INVALID");
  } 
}

void dacDecrease(){
  int ch = -1;
  long delta = -1;
  char * arg;
 
  // parse first argument
  arg = sCmd.next();
  if (arg != NULL){
    // convert to unsigned long...
    ch = atoi(arg);
  }
  
  // parse second argument
  arg = sCmd.next();
  if (arg != NULL){
    // convert to unsigned long...
    delta = atol(arg);//strtoul(arg, NULL, 0);
    //delta = atoi(arg);
  }
  
  // program arduino
  
  // make sure both commands were parsed correctly, and are positive...
  if (ch >= 0 && ch < 4 && delta >= 0){
    unsigned int oldVal = dac.getDAC(BANK0, dacCh[ch]);
    unsigned int newVal;
    if(oldVal > delta){
      newVal = oldVal - delta;
    }else {
      newVal = 0;
    }
    // update local variable array
    dacPos[ch] = newVal;
    
    // write DAC
    dac.writeDAC(BANK0, dacCh[ch], newVal);
  }  else {
    Serial.println("INVALID");
  }
}

void dacRead(){
  int ch = -1;
  char * arg;
 
  // parse first argument
  arg = sCmd.next();
  if (arg != NULL){
    ch = atoi(arg);
  }
  
  if(ch >= 0 && ch < 4){
    Serial.println(dac.getDAC(BANK0, dacCh[ch]));
  } else {
    Serial.println("INVALID CHANNEL");
  }
}


void dacSet(){
  int ch = -1;
  long val = -1;
  char * arg;
 
  arg = sCmd.next();
  if (arg != NULL){
     ch = atoi(arg);
  }
  
  arg = sCmd.next();
  if (arg != NULL){
    val = atol(arg);
  }
  
  // make sure both commands were parsed correctly, and are positive...
  if (ch >= 0 && ch < 4 && val >= 0){
    // update local variable array
    dacPos[ch] = val;
    
    // write DAC
    dac.writeDAC(BANK0, dacCh[ch], val);
  }  else {
    Serial.println("INVALID");
  }
}



void unrecognizedCmd(const char *command){
  Serial.println("UNRECOGNIZED COMMAND");
}


void setup(){
  
  /************************************
          Serial setup
  ************************************/
  // for now, listen for commands over serial
  Serial.begin(115200);
    delay(500);
  
    SPI.begin();
	
    // want to do this better long-term; AD536x can handle 50MHz clock though.
    SPI.setClockDivider(SPI_CLOCK_DIV2);
    SPI.setBitOrder(MSBFIRST);
    SPI.setDataMode(SPI_MODE1);



  sCmd.addCommand("I", dacIncrease);
  sCmd.addCommand("D", dacDecrease);
  sCmd.addCommand("Q", dacRead);
  sCmd.addCommand("S", dacSet);
  sCmd.setDefaultHandler(unrecognizedCmd);
  Serial.println("ready");
  
  pinMode(LED0, OUTPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);

  
  /************************************
          DAC Setup
  ************************************/  
  digitalWrite(LED0, HIGH);
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
 
  // re-zero for unipolar positive output
  dac.assertClear(0);
  dac.writeGlobalOffset(BANK0, 0x00);
  dac.writeDAC(BANK0, CHALL, 0x00);
  dac.assertClear(1); 
 
  
}


void loop(){
  long newDacEncPos = dacEnc.read();
  long newChEncPos = chEnc.read();

  // check channel encoder; update if needed.
  if (newChEncPos > chEncPos + 1 && digitalRead(CH_ENC_A) == HIGH && digitalRead(CH_ENC_B) == HIGH){
    digitalWrite(ledPins[activeCh], LOW);

    // state machine...
    if (activeCh == 0){
       activeCh = 1;
    }else if(activeCh == 1){
      activeCh = 2;
    }else if(activeCh == 2){
      activeCh = 0;
    }
    digitalWrite(ledPins[activeCh], HIGH);
    chEncPos = newChEncPos;
  } else if (newChEncPos < chEncPos - 1 && digitalRead(CH_ENC_A) == HIGH && digitalRead(CH_ENC_B) == HIGH){
    digitalWrite(ledPins[activeCh], LOW);
    
    // state machine...
    if (activeCh == 0){
       activeCh = 2;
    }else if(activeCh == 1){
      activeCh = 0;
    }else if(activeCh == 2){
      activeCh = 1;
    }
    
    digitalWrite(ledPins[activeCh], HIGH);
    chEncPos = newChEncPos;
  }
  
  // check dac encoder; update if needed.
  if (newDacEncPos > dacEncPos + 1){
    unsigned int currentPos = dacPos[activeCh];
    // don't overflow!
    if (currentPos != 65535){
      dacPos[activeCh] += 1;  
    }
    //update encoder
    dacEncPos = newDacEncPos;
    
    // write to dac
    dac.writeDAC(BANK0, dacCh[activeCh], dacPos[activeCh]);
    
  } else if (newDacEncPos < dacEncPos - 1){
    unsigned int currentPos = dacPos[activeCh];
    // don't overflow!
    if (currentPos != 0){
      dacPos[activeCh] -= 1;  
    }
    // update encoder
    dacEncPos = newDacEncPos;
    
    // Write dac
    dac.writeDAC(BANK0, dacCh[activeCh], dacPos[activeCh]);
  }

  // temporary hack;
  // gives visual indication if hogging encoder line... keep the LED on!!  
  if ((digitalRead(DAC_ENC_A) == LOW && digitalRead(DAC_ENC_B) == LOW) || (digitalRead(CH_ENC_A) == LOW && digitalRead(CH_ENC_B) == LOW)){
   digitalWrite(ledPins[activeCh], LOW);
  } else {
    digitalWrite(ledPins[activeCh], HIGH);
  }

  // process any commands at serial terminal
  sCmd.readSerial();

     
}
