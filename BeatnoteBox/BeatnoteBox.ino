/*
   BeatnoteBox.ino  - Arduino sketch for Beatnote Box
   
   Created by Neal Pisenti, 2015
   JQI - Joint Quantum Institute

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

// For use with a Teensy; may work with other arduino-style boards too.

*/

// pin mappings for the Teensy frequency shield
#include "Teensy_frequencyShield_2ch.h"

#include <SPI.h>
#include "ADF4350.h"
#include "ADF4107.h"
#include "AD9954.h"

// is this what I want for Teensy?
#include <EEPROM.h>


#define DDS_REF_FREQ 400.0	// DDS reference frequency


// EEPROM memory locations
// this is where we'll persist settings/etc. to non-voltile memory
#define EEPROM_BEATNOTE_P 0
#define EEPROM_BEATNOTE_B 1
#define EEPROM_BEATNOTE_A 2
#define EEPROM_BEATNOTE_R 3

// actual (current) ftw for dds objects
#define EEPROM_AUXDDS_LOW 4
#define EEPROM_AUXDDS_HIGH 5
#define EEPROM_REFDDS_LOW 6
#define EEPROM_REFDDS_HIGH 7

// "center frequency" (CF) setting for dds objects
#define EEPROM_AUXDDS_CF_LOW 8
#define EEPROM_AUXDDS_CF_HIGH 9
#define EEPROM_REFDDS_CF_LOW 10
#define EEPROM_REFDDS_CF_HIGH 11


// Some handy LCD defines...
#define LCD_BLANKLINE '                '






ADF4350 clock(PLL1_LE);

// should refactor/update DDS library; PS1, OSK not used or connected --
// what should be desired behavior?
AD9954 auxDDS(DDS1_CS, DDS1_RESET, DDS1_IOUPDATE, DDS1_PSO, 0, 0); 

AD9954 refDDS(DDS2_CS, DDS2_RESET, DDS2_IOUPDATE, DDS2_PSO, 0, 0)

// what should be done about MUX input?? 
// Also refactor PLL libraries to accomodate this.
ADF4107 beatnotePLL(PLL2_LE);



// Globals (in memory)
int beatnotePLL_P, beatnotePLL_B, beatnotePLL_A, beatnotePLL_R;


// typedef ftw_t union {... lookup...}
unsigned long auxDDS_ftw, refDDS_ftw;
double auxDDS_freq, refDDS_freq;

double


void setup(){

	// init SPI bus
	SPI.begin();
	SPI.setDataMode(SPI_MODE0);
	SPI.setClockDivider(4);
	delay(50);	// give it a sec
	
	clock.initialize(DDS_REF_FREQ, 10);	// init clock to output 400MHz, from 
										// onboard 10MHz oscillator
	
	
	
	writeLCD(LCD_BLANKLINE, LINE0);
	writeLCD(LCD_BLANKLINE, LINE1);
	writeLCD('Initializing...', LINE0);
	
	
	// Read in values from last time
	auxDDS_ftw.low = EEPROM.read(EEPROM_AUXDDS_LOW);
	auxDDS_ftw.high = EEPROM.read(EEPROM_AUXDDS_HIGH);
	// ... etc; fix when have internet connection!
	
	
	writeLCD('BN: ', 0);
	writeLCD(str(
	
	

}


void loop(){


}


// Auxiliary functions

// calculates DDS FTW from frequency
unsigned long freqToFTW(double freq){
	return (unsigned long)(freq/(double)DDS_REF_FREQ);
}

// calculates DDS frequency from FTW
double FTWToFreq(unsigned long ftw){
	return ((double)DDS_REF_FREQ)*((double)ftw/4294967296.0);
}

// Writes to LCD
// lcd_t mode defines behavior of write, eg,
// LINE0, LINE1, CONT
void writeLCD(char * str, lcd_t mode){

}

