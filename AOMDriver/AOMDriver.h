/*
   AOMDriver.h - Pin mappings for Teensy board attached to AOMDriver
   Created by Neal Pisenti, 2015
   JQI - Strontium - UMD

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

 */
 



#ifndef AOMDriver_H
#define AOMDriver_H

#include "Arduino.h"
#include "SPI.h"
//#include "LCD.h"


// generic switches
#define SW1         1
#define SW2         27
#define SW3         28

// encoder
#define ENC_SW      0
#define ENC_A       9
#define ENC_B       23

// enable or disable VCO (ie, VCO powerdown)
#define VCO_EN      8

// main SPI; for DAC and display communication
#define SPI_MOSI    11
#define SPI_MISO    12
#define SPI_CLK     13

// DAC specific IO pins
#define DAC_CLR     14
#define DAC_LDAC    15
#define DAC_RST     16
#define DAC_SYNC    17

// LCD Display specific pins; use JQI's LCD library
#define LCD_RST     20
#define LCD_RS      21
#define LCD_CS      22

// attenuator control pins
#define ATTEN_LE    24
#define ATTEN_CLK   25
#define ATTEN_MOSI  26

// generic I2C bus
#define I2C_SDA     18
#define I2C_SCL     19

// Internal/External controls; source select/enable/disable
#define INT_EXT_RF_CTL      2  // RF source set from teensy or external BNC?
#define INT_EXT_OUTPUT_CTL  4  // output on/off set from teensy or external BNC?
#define EXT_DITHER_CTL      10 // external dither enabled or disabled
#define SERVO_CTL           6  // power feedback, or daughter board feedback?   
#define SETPT_CTL           7  // external servo setpoint, or dac setpoint?

#define TEENSY_RF_TTL       3  // RF source select
#define TEENSY_OUTPUT_TTL   5  // Output on/off


// analog power measurement pin
// will probably want a calibration function somewhere?
#define PWR_MEAS    A10


enum dac_t { VCO, SETPT };

// probably bad practice... fix later. should be global for SPI settings


	// initialize SPI bus
	// use SPI.beginTransaction(dacSPISettings)...
	// see, eg, https://www.pjrc.com/teensy/td_libs_SPI.html
SPISettings dacSPISettings(20000000, MSBFIRST, SPI_MODE3);

class AOMDriver
{

	public: 
	static void init();
	static void write_attenuator(uint8_t data);
	static void write_dac(dac_t dac, uint16_t value);
	
	static void vco_enable(unsigned int data);	// data to write to VCO dac
	static void vco_disable();

	private:
	//static SPISettings dacSPISettings;
	
};


// function definitions

void AOMDriver::vco_enable(unsigned int data){

	digitalWrite(VCO_EN, HIGH);
	AOMDriver::write_dac(VCO, data);
}

void AOMDriver::vco_disable(){
	AOMDriver::write_dac(VCO, 0x00);	// set VCO to zero
	digitalWrite(VCO_EN, LOW);	// disable power supply to VCO
}


void AOMDriver::write_dac(dac_t dac, uint16_t value){
	uint32_t data = (1 << 20); 	// set data reg bit
	data |= (value & 0x7FFF);	// slap on value... mask for positive unipolar
	if (dac == VCO){
		data |= (0 << 16);
	} else if (dac == SETPT){
		data |= (1 << 16);
	} else {
		// else, don't do anything since invalid.
		return;
	}
	
	uint8_t xfer;
	
	SPI.beginTransaction(dacSPISettings);
	digitalWrite(DAC_SYNC, LOW);
	for (int i = 2; i >=0; i--){
		xfer = (data >> 8*i) & 0xFF;
		SPI.transfer(xfer);
	}
	digitalWrite(DAC_SYNC, HIGH);
	SPI.endTransaction();
}

void AOMDriver::write_attenuator(uint8_t data){
	// make sure we're in "idle" state -- should be true anyways.
	digitalWrite(ATTEN_CLK, LOW);
	digitalWrite(ATTEN_LE, LOW);
	digitalWrite(ATTEN_MOSI, LOW);

	int send_bit;
	for (int i = 5; i >= 0; i--){
		send_bit = (data >> i) & 0x01;	// mask out i_th bit
										// start MSB first
		digitalWrite(ATTEN_MOSI, send_bit);
		delayMicroseconds(1);
	
		digitalWrite(ATTEN_CLK, HIGH);
		delayMicroseconds(1);
		digitalWrite(ATTEN_CLK, LOW);
		delayMicroseconds(1);
	}

	// latch in shift register.
	digitalWrite(ATTEN_LE, HIGH);
	delayMicroseconds(1);
	digitalWrite(ATTEN_LE, LOW);
}



void AOMDriver::init(){
	// generic switches
	pinMode(SW1, INPUT);
	pinMode(SW2, INPUT);
	pinMode(SW3, INPUT);

	// encoder pins
	pinMode(ENC_SW, INPUT);
	pinMode(ENC_A, INPUT);
	pinMode(ENC_B, INPUT);

	// VCO enable -- disable on startup
	pinMode(VCO_EN, OUTPUT);
	digitalWrite(VCO_EN, LOW);



	// DAC specific IO pins
	pinMode(DAC_CLR, OUTPUT);
	pinMode(DAC_LDAC, OUTPUT);
	pinMode(DAC_RST, OUTPUT);
	pinMode(DAC_SYNC, OUTPUT);

	digitalWrite(DAC_CLR, HIGH);	
	digitalWrite(DAC_LDAC, LOW);	// always update as SYNC goes high
	digitalWrite(DAC_RST, HIGH);
	digitalWrite(DAC_SYNC, HIGH);

	/*
	// LCD Display specific pins; use JQI's LCD library
	// ie, don't set these up here.
	pinMode(LCD_RST, OUTPUT);
	pinMode(LCD_RS, OUTPUT);
	pinMode(LCD_CS, OUTPUT);

	digitalWrite(LCD_RST, 
	digitalWrite(LCD_RS, 
	digitalWrite(LCD_CS, HIGH);

	*/

	// attenuator control pins
	pinMode(ATTEN_LE, OUTPUT);
	pinMode(ATTEN_CLK, OUTPUT);
	pinMode(ATTEN_MOSI, OUTPUT);

	/*
		For DAT-31R5A-SN attenuator:
		CLK and MOSI clock data into a shift register; when LE is low, 
		data doesn't hit attenuator. Bringing LE high for at least 30ns latches
		shift register into attenuator output.

		Data on MOSI is latched into the shift register with a clk pulse 
		low -> high -> low. CLK should be normally low.
	*/
	digitalWrite(ATTEN_LE, LOW);
	digitalWrite(ATTEN_CLK, LOW);
	digitalWrite(ATTEN_MOSI, LOW);


	// Internal/External controls; source select/enable/disable
	pinMode(INT_EXT_RF_CTL, OUTPUT);
	pinMode(INT_EXT_OUTPUT_CTL, OUTPUT);
	pinMode(EXT_DITHER_CTL, OUTPUT);
	pinMode(SERVO_CTL, OUTPUT); 
	pinMode(SETPT_CTL, OUTPUT);

	digitalWrite(INT_EXT_RF_CTL, LOW);
	digitalWrite(INT_EXT_OUTPUT_CTL, LOW);
	digitalWrite(EXT_DITHER_CTL, LOW);
	digitalWrite(SERVO_CTL, LOW);
	digitalWrite(SETPT_CTL, LOW);


	pinMode(TEENSY_RF_TTL, OUTPUT);
	pinMode(TEENSY_OUTPUT_TTL, OUTPUT);

	digitalWrite(TEENSY_RF_TTL, LOW);
	digitalWrite(TEENSY_OUTPUT_TTL, LOW);


	// analog power measurement pin
	// will probably want a calibration function somewhere?
	pinMode(PWR_MEAS, INPUT);


	


	// reset DAC
	digitalWrite(DAC_CLR, LOW);
	delayMicroseconds(1);
	digitalWrite(DAC_CLR, HIGH);


}


#endif