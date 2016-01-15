/*
 * Ade33HardwareBase.h
 *
 * Created: 02/01/2016 13:35:57
 *  Author: Paul Soulsby
 */ 


#ifndef ADE33HARDWAREBASE_H_
#define ADE33HARDWAREBASE_H_

class Ade33HardwareBase
{
	public:
	virtual void hardwareSwitchChanged(unsigned char sw, unsigned char newValue) = 0;
	virtual void hardwareSwitchHeld(unsigned char sw) = 0;
	virtual void hardwareAnalogueInputChanged(unsigned char an, unsigned char newValue) = 0;	
	virtual void hardwareAnalogueClockEdge(unsigned char an, bool rising) = 0;	
};

#endif /* ADE33HARDWAREBASE_H_ */