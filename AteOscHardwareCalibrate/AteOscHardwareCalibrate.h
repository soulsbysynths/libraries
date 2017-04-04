/* 
* AteOscHardwareCalibrate.h
*
* Created: 13/03/2017 19:13:12
* Author: paulsoulsby
*/


#ifndef __ATEOSCHARDWARECALIBRATE_H__
#define __ATEOSCHARDWARECALIBRATE_H__

#include <AteOscHardware.h>
#include <AteOscHardwareBase.h>
#include <AtmAudio.h>
#include <Wavetable.h>
#include <avr/pgmspace.h>
#include <AteOscOscillatorProgmem.h>
#include <AteOscPitch/AteOscPitch.h>
#include <AteOscHardwareTesterProgmem.h>
#include <Arduino.h>

class AteOscHardwareCalibrate  : public AteOscHardwareBase
{
//variables
public:
protected:
private:
	AteOscHardware& hardware_;
//functions
public:
	AteOscHardwareCalibrate();
	~AteOscHardwareCalibrate();
	bool initFram();
	bool initEeprom();
	void initTest(char test);
	void hardwareSwitchChanged(unsigned char sw, unsigned char newValue);
	void hardwareSwitchHeld(unsigned char sw);
	void hardwareCvInputChanged(unsigned char control, unsigned int newValue);
	void hardwareRotaryEncoderChanged(unsigned char rotary, unsigned char newValue, bool clockwise);
	void hardwareAudioBufferStatusChanged(unsigned char newStatus);
	void hardwareGateInputChanged(unsigned char input, bool newValue);
protected:
private:
	AteOscHardwareCalibrate( const AteOscHardwareCalibrate &c );
	AteOscHardwareCalibrate& operator=( const AteOscHardwareCalibrate &c );

}; //AteOscHardwareCalibrate

#endif //__ATEOSCHARDWARECALIBRATE_H__
