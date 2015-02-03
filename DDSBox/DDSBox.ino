#include <SPI.h>
#include <SimpleLCD.h>
#include <ADF4350.h> 
#include <AD9954.h>
#include <LockFreq.h>
#include <Wire.h>
#include <MyEEPROM.h>
/* A whole bunch of parameters/pin mappings */

#define D1IOUPDATE 2
#define D1PS1 3
#define D1PS0 4
#define D1OSK 5
#define D1SS 6
#define D1RESET 7

#define D2IOUPDATE 8
#define D2PS1 9
#define D2PS0 10
#define D2OSK 11
#define D2SS 12
#define D2RESET 13

#define D3IOUPDATE 46
#define D3PS1 44
#define D3PS0 42
#define D3OSK 40
#define D3SS 38
#define D3RESET 36

#define P1SS 32
#define P2SS 34

// POT pins
#define POT1 A1
#define POT2 A3
#define POT3 A5

// Switch pins
#define SW1 23
#define SW2 27
#define SW3 31

// Freq tuning resolutions...
#define RES1 2
#define RES2 2
#define RES3 2

#define MULT1 15
#define MULT2 15
#define MULT3 15


/* Declare PLL and DDS objects; pass them to Lock; declaer SimpleLCD object */

ADF4350 Clock(P1SS);
AD9954 DDS1(D1SS, D1RESET, D1IOUPDATE, D1PS0, D1PS1, D1OSK);
AD9954 DDS2(D2SS, D2RESET, D2IOUPDATE, D2PS0, D2PS1, D2OSK);
AD9954 DDS3(D3SS, D3RESET, D3IOUPDATE, D3PS0, D3PS1, D3OSK);

SimpleLCD LCD(&Serial2);

LockFreq Lock1(POT1, RES1, MULT1);
LockFreq Lock2(POT2, RES2, MULT2);
LockFreq Lock3(POT3, RES3, MULT3);


// Object to keep track of whether or not the EEPROM has been updated with correct frequency...
// the 4th element of that array is for the LCD.
bool writeFreq[] = {0,0,0,0};

// EEPROM memory addresses
int storageArray[] = {0x100, 0x200, 0x300};
AD9954* DDSArray[] = {&DDS1, &DDS2, &DDS3};

void setup(){
  
  pinMode(SW1, INPUT);
  pinMode(SW2, INPUT);
  pinMode(SW3, INPUT);
  Serial.begin(9600);
  Serial2.begin(9600);
  SPI.begin();
  SPI.setClockDivider(4);
  SPI.setDataMode(SPI_MODE0);
  
  // read out set values from EEPROM
  
  Wire.begin();
  byte buffer[4]; // each frequency is type unsigned long -> 4 bytes
  unsigned long freq1, freq2, freq3;
  MyEEPROM.read(storageArray[0], buffer, 4); // Read out 4 bytes into buffer
  // probably a better way to do this, for someone with more C++ experience...
  // but, construct frequency from individual bytes, which are stored Big Endian.
  freq1 = (buffer[0] << 24) + (buffer[1] << 16) + (buffer[2] << 8) + buffer[3];
  
  //rinse and repeat!
  MyEEPROM.read(storageArray[1], buffer, 4);
  freq2 = (buffer[0] << 24) + (buffer[1] << 16) + (buffer[2] << 8) + buffer[3];

  MyEEPROM.read(storageArray[2], buffer, 4);
  freq3 = (buffer[0] << 24) + (buffer[1] << 16) + (buffer[2] << 8) + buffer[3];


  
  delay(1000);
  //LCD.backlight(157);
  LCD.clearScreen();
  LCD.setDecimalCount(2);
  LCD.write(1, "Initializing...");
  
  // initialize clock PLL to 400 MHz, with onboard reference of 10MHz
  Clock.initialize(400,10);
  // enable auxiliary output on PLL; set power to 5dbm
  Clock.auxEnable(1);
  Clock.setAuxPower(3);
  
  delay(1000);
  
  
  // initialize DDS's with 400MHz clock
  DDS1.initialize(400000000);
  DDS2.initialize(400000000);
  DDS3.initialize(400000000);
  delay(100);
  

  // LockFreq::initialize(dds, baseFrequency, initialization frequency).
  //   If the initialization frequency is zero, then it will poll the voltage on it's POT tuning pin.
  
  // Here, we initialize the DDS to (either a specified frequency or to frequency pulled from POT voltage), with baseFreq of 0 MHz.
  //   If you would like to specify a particular frequency on a particular channel, change the third argument to Lock.initialize(..).
  Lock1.initialize(DDS1, 0, freq1);
  Lock2.initialize(DDS2, 0, freq2);
  Lock3.initialize(DDS3, 0, freq3);
  
/*
  DDS1.setFreq(Lock1.getSetpoint());
  DDS2.setFreq(Lock2.getSetpoint());
  DDS3.setFreq(Lock3.getSetpoint());
  */

  updateLCD(0);
 

}



void loop(){
  delay(200);

// instructions for programming frequency from front pannel:
/* There are three switches, next to each output channel. Flipping any single switch will display the current frequency on that
   channel to full precision (right now, due to limited character # on the LCD, it is truncating at 2 decimal points).
   
   To set a new frequency, you should flip the switch of whichever channel you want to change, then flip the switch to the right
   (with wraparound boundary conditions). This will enable the POT on that channel, and you can dial in to the frequency you want.
   Then, flip the switch to the right again, and it will fix your current setpoint, then write to EEPROM persistent memory when you
   flip the channel switch back to off.
   
 */
  bool s[] = {digitalRead(SW1), digitalRead(SW2), digitalRead(SW3)};
  
  
  // 100
  if(s[0] && !s[1] && !s[2]){
      updateLCD(1);
      writeFreq[3] = 1;
  } 
  
  // 110
  if (s[0] && s[1] && !s[2]){
      Lock1.updateFreq();
      // set Ch 1 to be updated to EEPROM
      writeFreq[0] = 1;
      // make sure LCD updates when switched back into normal mode...
      writeFreq[3] = 1;
      updateLCD(1);
  }
  
  // 010
  if (!s[0] && s[1] && !s[2]){
    updateLCD(2);
    writeFreq[3] = 1;
  }
  
  // 011
  if (!s[0] && s[1] && s[2]){
     Lock2.updateFreq();
      writeFreq[1] = 1;
      writeFreq[3] = 1;
      updateLCD(2);
  }
  
  // 001
  if (!s[0] && !s[1] && s[2]){
    updateLCD(3);
    writeFreq[3] = 1;
  }
  
  // 101
   if (s[0] && !s[1] && s[2]){
     Lock3.updateFreq();
      writeFreq[2] = 1;
      writeFreq[3] = 1;
      updateLCD(3);
   }
   
   // 000 -- ie, default
   if (!s[0] && !s[1] && !s[2]){
    
    for(int ch = 0; ch < 3; ch++){
      if (writeFreq[ch]){
        unsigned long tempFreq = DDSArray[ch]->getFreq();
        
        byte toEEPROM[] = {lowByte(tempFreq >> 24), lowByte(tempFreq >> 16), lowByte(tempFreq >> 8), lowByte(tempFreq)};
        
        MyEEPROM.write(storageArray[ch], toEEPROM, 4);
        
      
       
        writeFreq[ch] = 0;
      }
    }
    if (writeFreq[3]){
      updateLCD(0);
      writeFreq[3] = 0;
    }
   }
  
  
  
  
  
}


// Auxiliary functions...

void updateLCD(int channel){
  if (channel == 0){
    LCD.setDecimalCount(2);
    LCD.write(1, "1:");
    LCD.write((double)DDS1.getFreq()/1000000.0);
    LCD.write(" 2:");
    LCD.write((double)DDS2.getFreq()/1000000.0);
    LCD.write(2, "3: ");
    LCD.write((double)DDS3.getFreq()/1000000.0);
    LCD.write(" MHz");
  } else if (channel == 1){
    LCD.setDecimalCount(6);
    LCD.write(1, DDS1.getFreq()/1000000.0);
    LCD.write(" MHz");
    LCD.write(2, "(CH1)");
  } else if (channel == 2){
    LCD.setDecimalCount(6);
    LCD.write(1, DDS2.getFreq()/1000000.0);
    LCD.write(" MHz");
    LCD.write(2, "(CH2)");
  } else if (channel == 3){
    LCD.setDecimalCount(6);
    LCD.write(1, DDS3.getFreq()/1000000.0);
    LCD.write(" MHz");
    LCD.write(2, "(CH3)");
  }
}



