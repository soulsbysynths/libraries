/*
 * JamHardwareBase.h
 *
 * Created: 19/06/2016 12:17:49
 *  Author: paulsoulsby
 */ 


#ifndef JAMHARDWAREBASE_H_
#define JAMHARDWAREBASE_H_

class JamHardwareBase
{
	public:
	virtual void hardwareSwitchChanged(unsigned char sw, unsigned char newValue) = 0;
	virtual void hardwareSwitchHeld(unsigned char sw) = 0;
	virtual void hardwareAnalogueControlChanged(unsigned char control, unsigned char newValue) = 0;
};



#endif /* JAMHARDWAREBASE_H_ */