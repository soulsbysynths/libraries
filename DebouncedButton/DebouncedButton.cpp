/* 
* DebouncedButton.cpp
*
* Created: 11/01/2015 23:36:46
* Author: Paul Soulsby
*/


#include "DebouncedButton.h"

// default constructor
DebouncedButton::DebouncedButton()
{
	mbDebounceTicks = 150;
} //DebouncedButton

// default destructor
DebouncedButton::~DebouncedButton()
{
} //~DebouncedButton

void DebouncedButton::Poll(byte ticks, byte newstate){
	if(mbState==HIGH){
		miHoldTime += ticks;
	}
	if(mbTicksPassed<mbDebounceTicks){
		mbTicksPassed += ticks;
	}
	if(mbTicksPassed>=mbDebounceTicks){
		if(newstate!=mbState){
			if(newstate==HIGH && mbState==LOW){
				meEvent = BUTTON_PRESSED;
				miHoldTime = 0;
			}
			else if(newstate==LOW && mbState==HIGH){
				meEvent = BUTTON_RELEASED;
			}
			mbState = newstate;
			mbTicksPassed = 0;
		}		
	}
}
DebouncedButton::buttonEvent DebouncedButton::getEvent(){
	buttonEvent r;
	r = meEvent;
	meEvent  = BUTTON_IDLE;
	return r;
}

void DebouncedButton::setDebounceTime(byte ticks){
	//keep well below 255.  If you  have debounceticks + tickspassed>255 you're in trouble.
	mbDebounceTicks = ticks;
}
byte DebouncedButton::getDebounceTime(){
	return mbDebounceTicks;
}
unsigned int DebouncedButton::getHoldTime(){
	return miHoldTime;
}
byte DebouncedButton::getState(){
	return mbState;
}