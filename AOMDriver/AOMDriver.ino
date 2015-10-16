#include "AOMDriver.h"

#include <SPI.h>
#include "SerialCommand.h"


SerialCommand sCmd;

void setup() {
  // put your setup code here, to run once:


  Serial.begin(9600);
  Serial.println("Live!");
  
  sCmd.addCommand("ds", digital_set);
  sCmd.addCommand("vco", write_vco);
  sCmd.addCommand("setpt", write_setpt);
  
  sCmd.setDefaultHandler(unrecognized);
  
  AOMDriver::init();
  SPI.begin();
  

}

void loop() {
  // put your main code here, to run repeatedly:
	sCmd.readSerial();

}


void unrecognized(const char * command){
	Serial.print("What was ");
	Serial.print(command);
	Serial.println("?");
}

void write_vco(){
	char * arg;
	arg = sCmd.next();
	if (arg != NULL){
		int val = atoi(arg);
		AOMDriver::write_dac(VCO, val);
		Serial.print("Wrote VCO: ");
		Serial.println(val);
	} else {
		Serial.println("didn't catch that vco value!");
	}
	
}

void write_setpt(){
	char * arg;
	arg = sCmd.next();
	if (arg != NULL){
		int val = atoi(arg);
		AOMDriver::write_dac(SETPT, val);
		Serial.print("Wrote SETPT: ");
		Serial.println(val);
	} else {
		Serial.println("didn't catch that setpt value!");
	}
	
}

void digital_set(){
	char * arg;
	int dpin = 0;
	int val = 0;
	arg = sCmd.next();
	if (arg != NULL){
		dpin = atoi(arg);
	}
	arg = sCmd.next();
	if (arg != NULL) {
		val = atoi(arg);
	}
	if (dpin != 0){
		digitalWrite(dpin, val);
		Serial.print("Wrote pin ");
		Serial.print(dpin);
		Serial.print(" = ");
		Serial.println(val);
	} else {
		Serial.println("Didn't catch that.");
		}

}