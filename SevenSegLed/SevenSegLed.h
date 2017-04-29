/* 
* SevenSegLed.h
*
* Created: 11/04/2017 15:10:24
* Author: paulsoulsby
*/


#ifndef __SEVENSEGLED_H__
#define __SEVENSEGLED_H__

#include "SevenSegLedProgmem.h"
#include "SsHelpers.h"

class SevenSegLed
{
//variables
public:
protected:
private:
	unsigned int led_c_ = 0;
	unsigned int led_charbuf_c_ = 0;
	unsigned char flashCnt_[2] = {0};
	unsigned char flashOnOffTicks_c_[2] = {0};   //compresses on and off ticks into 4 bit
	unsigned int flashTick_[2] = {0};
	bool visible_ = true;
	bool dp_ = false;
	bool invertDp_ = false;
//functions
public:
	SevenSegLed();
	~SevenSegLed();
	void setState(unsigned int leds){led_c_ = leds;}
	unsigned int getState(){return led_c_;}
	void setVisible(bool newValue);
	bool getVisible(){return visible_;}
	void displayHex(unsigned char newValue);
	void displayTapeRoll(unsigned char frame);
	void displayChar(char character);
	void setDp(bool way);
	bool getDp(){return dp_;}
	void setInvertDp(bool way);
	void flash(bool dp, unsigned char flashes, unsigned char onTicks, unsigned char offTicks, bool startOn);
	void flashStop(bool dp);
	//void flashStopAll();
	void refreshFlash(unsigned char tick_inc);
protected:
private:
	SevenSegLed( const SevenSegLed &c );
	SevenSegLed& operator=( const SevenSegLed &c );
	void setFlashOnTicks(unsigned char index, unsigned char onticks);
	unsigned char getFlashOnticks(unsigned char index);
	void setFlashOffTicks(unsigned char index, unsigned char offticks);
	unsigned char getFlashOffTicks(unsigned char index);
}; //SevenSegLed

#endif //__SEVENSEGLED_H__
