/* 
* JamHardwareTester.h
*
* Created: 18/06/2016 22:21:52
* Author: paulsoulsby
*/


#ifndef __JAMHARDWARETESTER_H__
#define __JAMHARDWARETESTER_H__

#include "JamHardwareBase.h"
#include "JamHardware.h"
//#include "u8g.h"

class JamHardwareTester : public JamHardwareBase
{
//variables
public:
protected:
private:
	JamHardware& hardware_;
	static const unsigned char FLASH_TICKS = 8;
//functions
public:
	JamHardwareTester();
	~JamHardwareTester();
	void initialize();
	void poll(unsigned char ticksPassed);
	void pollAudio();
	void hardwareSwitchChanged(unsigned char sw, unsigned char newValue);
	void hardwareSwitchHeld(unsigned char sw);
	void hardwareAnalogueControlChanged(unsigned char control, unsigned char newValue);

protected:
private:
	JamHardwareTester( const JamHardwareTester &c );
	JamHardwareTester& operator=( const JamHardwareTester &c );

}; //JamHardwareTester

#endif //__JAMHARDWARETESTER_H__
