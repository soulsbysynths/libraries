//AteOscHardwareTester.h  Atmegatron Hardware Tester. Used by assembly factory.
//Copyright (C) 2015  Paul Soulsby info@soulsbysynths.com
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

#ifndef __ATEOSCHARDWARETESTER_H__
#define __ATEOSCHARDWARETESTER_H__

#include <Arduino.h>
#include <AteOscHardware.h>
#include <AteOscHardwareBase.h>
#include <AtmAudio.h>
#include <Wavetable.h>
#include <avr/pgmspace.h>



class AteOscHardwareTester : public AteOscHardwareBase
{
//variables
public:
	enum TestMode : unsigned char
	{
		TEST_TICK = 0,
		TEST_POTS = 1,
		TEST_AUDIO = 2
	};
protected:
private:
	const unsigned char NOTE_OFF = 0x80;   ///< Note Off
	const unsigned char NOTE_ON  = 0x90;    ///< Note On
	const unsigned char TEST_NOTE = 60;
	const unsigned char WAVE_LEN = 32;
	const char sine_wave[32] = {-1,23,47,70,89,105,117,124,127,124,117,105,89,70,47,23,-1,-25,-49,-72,-91,-107,-119,-126,-128,-126,-119,-107,-91,-72,-49,-25};
	const char saw_wave[32] = {-128,-120,-112,-103,-95,-87,-79,-70,-62,-54,-46,-38,-29,-21,-13,-5,4,12,20,28,37,45,53,61,69,78,86,94,102,111,119,127};
	AteOscHardware& hardware_;
	AtmAudio* audio_;
	unsigned char tick_;
	TestMode test_ = TEST_TICK;
	bool lastMoving_[6] = {false};
	bool sinewave_ = false;
	Wavetable wavetable_;
//functions
public:
	AteOscHardwareTester();
	~AteOscHardwareTester();
	void init();
	void tick();
	void poll(unsigned char ticksPassed);
	void hardwareSwitchChanged(unsigned char sw, unsigned char newValue);
	void hardwareSwitchHeld(unsigned char sw);
	void hardwareCvInputChanged(unsigned char control, unsigned int newValue);
	void hardwareRotaryEncoderChanged(unsigned char rotary, unsigned char newValue, bool clockwise);
	void hardwareAudioBufferStatusChanged(unsigned char newStatus);
protected:
private:
	AteOscHardwareTester( const AteOscHardwareTester &c );
	AteOscHardwareTester& operator=( const AteOscHardwareTester &c );
	void refreshAudioTest();
	void refreshSineWave();
	
}; //AteOscHardwareTester

#endif //__ATEOSCHARDWARETESTER_H__
