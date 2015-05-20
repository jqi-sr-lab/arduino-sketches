#include <SPI.h>

#include "Teensy_FrequencyShield.h"

#include "AD9954.h"
#include "ADF4350.h"
#include "ADF4107.h"

// Serial command library
// https://github.com/kroimon/Arduino-SerialCommand
#include <SerialCommand.h>
SerialCommand sCmd;

#define DDS_REF_FREQ 400.0	// DDS reference frequency




AD9954 DDS0(DDS4_CS, DDS4_RESET, DDS4_IOUPDATE, DDS4_PS0, 0, 0);
AD9954 DDS1(DDS3_CS, DDS3_RESET, DDS3_IOUPDATE, DDS3_PS0, 0, 0);

AD9954 DDS2(DDS1_CS, DDS1_RESET, DDS1_IOUPDATE, DDS1_PS0, 0, 0);
//AD9954 beatnoteDDS(DDS2_CS, DDS2_RESET, DDS2_IOUPDATE, DDS2_PS0, 0, 0);
AD9954 beatnoteDDS(DDS2_CS, DDS2_RESET, DDS2_IOUPDATE, DDS2_PS0, 0, 0);

ADF4350 clock(PLL1_LE);

ADF4107 beatnotePLL(PLL2_LE);

SPISettings spi_settings(1000000, MSBFIRST, SPI_MODE0); 

void setup() {
         Serial.begin(9600);
     //  delay(1000);
   Serial.println("life");
 delay(1000);
  // put your setup code here, to run once:
	SPI.begin();
	//SPI.setDataMode(SPI_MODE0);
	//SPI.setClockDivider(4);
	delay(50);	// give it a sec
	SPI.beginTransaction(spi_settings);
delay(100);
	clock.initialize(400, 10);	// init clock to output 400MHz, from 
					        // onboard 10MHz oscillator
 delay(100);
 clock.auxEnable(1);
delay(100);
  clock.setAuxPower(3);
delay(100);
  DDS0.initialize(400000000);
  DDS1.initialize(400000000);
  DDS2.initialize(400000000);
  beatnoteDDS.initialize(400000000);
  
 DDS0.setFreq(10000000);
  DDS1.setFreq(11000000);
   DDS2.setFreq(12000000);
 beatnoteDDS.setFreq(8000000);
 delay(100);
 
 // 25x multiplier
 beatnotePLL.initialize(8,3,1,1);
sCmd.addCommand("BN", setBN);
}

void loop() {
  // put your main code here, to run repeatedly:
//  Serial.println("heartbeat");
  //DDS2.initialize(400000000);
  
 //  DDS2.setFreq(12000000);
//  clock.initialize(400, 10);
 // delay(1000);
sCmd.readSerial();
}


void unrecognizedCmd(const char *command){
  Serial.println("UNRECOGNIZED COMMAND");
}

void setBN(){
  
  char * arg;
  unsigned long freq;
    arg = sCmd.next();
  if (arg != NULL){
    freq = atoi(arg);
       beatnoteDDS.setFreq(freq);
       Serial.print("Ref set to ");
       Serial.println(freq);
       Serial.print("BN: ");
       double tmp = freq*25.0/1000000.0;
       Serial.println(tmp);
} else {
    Serial.println("Please enter frequency");

}
}
  

