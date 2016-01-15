/* 
* Ade33HwTester.h
*
* Created: 02/01/2016 13:32:01
* Author: Paul Soulsby
*/


#ifndef __ADE33HWTESTER_H__
#define __ADE33HWTESTER_H__

#include "Ade33Hardware.h"
#include "Ade33HardwareBase.h"

class Ade33HwTester : public Ade33HardwareBase
{
//variables
public:
protected:
private:
	Ade33Hardware& hardware_;
	bool auto_ = true;
	static const unsigned char EXT_LED = 4;
//functions
public:
	Ade33HwTester();
	~Ade33HwTester();
	void init();
	void poll(unsigned char ticksPassed);
	void hardwareSwitchChanged(unsigned char sw, unsigned char newValue);
	void hardwareSwitchHeld(unsigned char sw);
	void hardwareAnalogueInputChanged(unsigned char an, unsigned char newValue);
protected:
private:
	Ade33HwTester( const Ade33HwTester &c );
	Ade33HwTester& operator=( const Ade33HwTester &c );
	void setAuto(bool newValue);
}; //Ade33HwTester

#endif //__ADE33HWTESTER_H__
