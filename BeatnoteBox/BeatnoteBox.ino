// Beatnote box with digital PLL -- testing!!

#include "Teensy_FrequencyShield_2ch.h"
#include <SPI.h>
#include "SerialCommand.h"

#include "ADF4350.h"
#include "AD9954.h"
#include "ADF4107.h"

#define DDS_REF_FREQ 400.0	// DDS reference frequency


SerialCommand sCmd;


ADF4350 clock(PLL2_LE);

// should refactor/update DDS library; PS1, OSK not used or connected --
// what should be desired behavior?
AD9954 refDDS(DDS1_CS, DDS1_RESET, DDS1_IOUPDATE, DDS1_PS0, 0, 0); 

//AD9954 refDDS(DDS2_CS, DDS2_RESET, DDS2_IOUPDATE, DDS2_PSO, 0, 0)

// what should be done about MUX input?? 
// Also refactor PLL libraries to accomodate this.
ADF4107 beatnotePLL(PLL1_LE);

SPISettings aa(100000, MSBFIRST, SPI_MODE0);

void setup(){
  Serial.begin(9600);
  SPI.begin();
 // SPI.setClockDivider(6);
  //SPI.setDataMode(SPI_MODE0);
  SPI.beginTransaction(aa);
  delay(200);
  
  clock.initialize(400,10);
  
  // enable auxiliary outputs
  clock.auxEnable(1);
  clock.setAuxPower(3);
   
  delay(500);
  
  refDDS.initialize(400000000);
  
  delay(500);

  //DDS1.setFreq(16000000);
  refDDS.setFreq(58526000);
  //refDDS.setFreq(58534000); //87 beatnote is 1463.15MHz/25= 58.526
  //DDS1.setFreq(47753000); //88 beatnote is 1241.59MHz/26= 47.753  
  delay(500);
  // P, B, A, R
  // beatnote = [(P*B+A)/R]*DDS Ref Freq.
  // Thus, DPLL.initialize(8,3,1,1) gives a 25x multiplier to DDS frequency.
  beatnotePLL.initialize(8,3,1,1);
  //DPLL.initialize(8,3,2,1);


  sCmd.addCommand("dds", setDDS);
   sCmd.addCommand("pll", setPLL);
   sCmd.addCommand("l",setT);
   sCmd.setDefaultHandler(unrecognized);    
   
}


void setT(){
  beatnotePLL.initialize(8,3,1,1);
}

void loop(){
  sCmd.readSerial();
}


void setDDS(){
  char * arg;
  arg = sCmd.next();
  int freq;
  if (arg != NULL) {
   freq = atoi(arg);  
   refDDS.setFreq(freq); 
   Serial.print("Set DDS frequency: ");
   Serial.println(freq);
  }else {
    Serial.println("error setting dds");
  }
}

void setPLL(){
 char * arg;
 int p;
 int b; 
int a;
int r;
arg = sCmd.next();
p = atoi(arg);
arg = sCmd.next();
b = atoi(arg);
arg = sCmd.next();
a = atoi(arg);
arg = sCmd.next();
r = atoi(arg);

if (p != 0 && b != 0 && a != 0 && r != 0){
  beatnotePLL.initialize(p,b,a,r);
  Serial.print("set pll: p = ");
  Serial.print(p);
  Serial.print("; b = ");
  Serial.print(b);
  Serial.print("; a = ");
  Serial.print(a);
  Serial.print("; r = ");
  Serial.println(r);
}else {
  Serial.println("error setting pll");
} 
}

void unrecognized(const char *command) {
  Serial.print("What is ");
  Serial.print(command);
  Serial.println("?");
  Serial.println("Valid commands: 'dds <FREQ>' and 'pll <P> <B> <A> <R>'");
}
