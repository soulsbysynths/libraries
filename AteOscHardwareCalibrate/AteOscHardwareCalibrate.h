/*
//AteOscHardwareCalibrate.h  Oscitron hardware calibration software
//Copyright (C) 2017  Paul Soulsby info@soulsbysynths.com
//
//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
