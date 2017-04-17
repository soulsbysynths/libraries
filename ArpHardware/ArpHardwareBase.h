/*
 * ArpHardwareBase.h
 *
 * Created: 11/04/2017 11:00:17
 *  Author: paulsoulsby
 */ 


#ifndef ARPHARDWAREBASE_H_
#define ARPHARDWAREBASE_H_

class ArpHardwareBase
{
	public:
	virtual void hardwareSwitchChanged(unsigned char sw, unsigned char newValue) = 0;
	virtual void hardwareSwitchHeld(unsigned char sw) = 0;
	virtual void hardwareJackDetectChanged(unsigned char jack, bool newValue) = 0; 
	virtual void hardwareCvInputChanged(unsigned char input, unsigned int newValue) = 0;
	virtual void hardwareGateInputChanged(bool newValue) = 0;
	virtual void hardwareMidiChannelChanged(unsigned char channel) = 0;
	virtual void hardwareMidiReceived(unsigned char data) = 0;
	virtual void hardwareMidiError(unsigned char errorType) = 0;
};



#endif /* ARPHARDWAREBASE_H_ */