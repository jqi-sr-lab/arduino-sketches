//#define SETLIST_DEBUG
#include <SetListArduino.h>
#include <SPI.h>
#include <ADF4350.h> 
#include <AD9954.h>

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
ADF4350 Clock(P1SS);

AD9954 DDS0(D1SS, D1RESET, D1IOUPDATE, D1PS0, D1PS1, D1OSK);
AD9954 DDS1(D2SS, D2RESET, D2IOUPDATE, D2PS0, D2PS1, D2OSK);


#define trigger 31
SetListArduino SetListImage(trigger);

#define baseFreq 80000000

void setDetuning(AD9954 * dds, int * params){
    int detuning = params[0];
    int freq = baseFreq + detuning*1000;
    dds->setFreq(freq);
}

// ramp function for DDS1
void setRamp(AD9954 * dds, int * params){
  int negR = LOW;  
  
  int d0 = params[0];
    int d1 = params[1];
    int tau = params[2];
    
    int freq0 = baseFreq + d0*1000;
    int freq1 = baseFreq + d1*1000;
    int delta = freq1 - freq0;
    
  
    
    int RR = 200;
    double rampRate;
    //rampRate = ((double) delta)/tau;   // Hz per second
    //int posDF = (int)(rampRate * RR * 100E-9);   // calculate posDF for DDS
    //int posDF = ((freq1-freq0)*1000*200)/(tau*100000);
   // int posDF = ((d1-d0)*RR)/(tau*100000);
   int posDF = ((d1-d0)*RR)/(tau*100);
    //Serial.println(posDF);
    
    // this should be bundled into the library, so pin is valid before write!!
   // digitalWrite(D1PS0, LOW);
    //dds->linearSweep(freq0, freq1, posDF, RR, posDF, RR);
    dds->linearSweep(freq0,freq1,posDF,RR,posDF,RR);
   // digitalWrite(D1PS0, HIGH);
}

// ramp function for DDS2
void setRamp2(AD9954 * dds, int * params){
  int negR = LOW;  
  
  int d0 = params[0];
    int d1 = params[1];
    int tau = params[2];
    
    int freq0 = baseFreq + d0*1000;
    int freq1 = baseFreq + d1*1000;
    int delta = freq1 - freq0;
   
    
    int RR = 200;
    double rampRate;
    //rampRate = ((double) delta)/tau;   // Hz per second
    //int posDF = (int)(rampRate * RR * 100E-9);   // calculate posDF for DDS
    //int posDF = ((freq1-freq0)*1000*200)/(tau*100000);
   // int posDF = ((d1-d0)*RR)/(tau*100000);
   int posDF = ((d1-d0)*RR)/(tau*100);
    //Serial.println(posDF);
    
    // this should be bundled into the library, so pin is valid before write!!
  //  digitalWrite(D2PS0, LOW);
    //dds->linearSweep(freq0, freq1, posDF, RR, posDF, RR);
    dds->linearSweep(freq0,freq1,posDF,RR,posDF,RR);
   // digitalWrite(D2PS0, HIGH);
}


void setup() {

  Serial.begin(115200);
 
  SPI.begin();
  SPI.setClockDivider(4);
  SPI.setDataMode(SPI_MODE0);
  
  Clock.initialize(400,10);
  DDS0.initialize(400000000);
  DDS1.initialize(400000000);
  
  DDS0.setFreq(80000000);
  DDS1.setFreq(80000000);
 
  SetListImage.registerDevice(DDS0, 0);
  SetListImage.registerDevice(DDS1, 1);
  
  SetListImage.registerCommand("d", 0, setDetuning);
  SetListImage.registerCommand("t", 0, setRamp);
  SetListImage.registerCommand("d", 1, setDetuning);
  SetListImage.registerCommand("t", 1, setRamp);  
}

void loop() {
 SetListImage.readSerial(); 
}
