/* 
* DebouncedButton.h
*
* Created: 11/01/2015 23:36:46
* Author: Paul Soulsby
*/


#ifndef __DEBOUNCEDBUTTON_H__
#define __DEBOUNCEDBUTTON_H__
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#include <pins_arduino.h>
#endif

class DebouncedButton
{
//variables
public:
	enum buttonEvent : byte{
		BUTTON_IDLE, BUTTON_PRESSED, BUTTON_RELEASED
	};
protected:
private:
	byte mbState;
	buttonEvent meEvent;
	byte mbDebounceTicks;
	byte mbTicksPassed;
	unsigned int miHoldTime;
//functions
public:
	DebouncedButton();
	~DebouncedButton();
	DebouncedButton::buttonEvent getEvent();
	void Poll(byte, byte);
	void setDebounceTime(byte);
	byte getDebounceTime();
	unsigned int getHoldTime();
	byte getState();
protected:
private:
	DebouncedButton( const DebouncedButton &c );
	DebouncedButton& operator=( const DebouncedButton &c );

}; //DebouncedButton

#endif //__DEBOUNCEDBUTTON_H__
