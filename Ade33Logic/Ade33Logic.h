/* 
* Ade33Logic.h
*
* Created: 05/01/2016 17:19:16
* Author: Paul Soulsby
*/


#ifndef __ADE33LOGIC_H__
#define __ADE33LOGIC_H__

#include "Ade33LogicEngine.h"
#include "Ade33LogicEngineBase.h"
#include "Ade33Hardware.h"
#include "Ade33HardwareBase.h"

class Ade33Logic : public Ade33HardwareBase, Ade33LogicEngineBase
{
//variables
public:
protected:
private:
	Ade33Hardware& hardware_;
	Ade33LogicEngine& engine_;
	static const unsigned char HIGH = 1;
	static const unsigned char LOW = 0;
	static const unsigned int AUTOSCROLL_TICKS = 500;
	unsigned char patchNum = 0;
	bool autoscrollMode = false;
	unsigned int autoscrollModeTick = 0;
//functions
public:
	Ade33Logic();
	~Ade33Logic();
	void initialize();
	void poll(unsigned char ticksPassed);
	void hardwareSwitchChanged(unsigned char sw, unsigned char newValue);
	void hardwareSwitchHeld(unsigned char sw) ;
	void hardwareAnalogueInputChanged(unsigned char an, unsigned char newValue);
	void hardwareAnalogueClockEdge(unsigned char an, bool rising);	
	void engineFunctionChanged(unsigned char func, unsigned char val);
	void engineOutputChanged(unsigned char val);
protected:
private:
	Ade33Logic( const Ade33Logic &c );
	Ade33Logic& operator=( const Ade33Logic &c );
	
}; //Ade33Logic

#endif //__ADE33LOGIC_H__
