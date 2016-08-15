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
	static const unsigned char WAVE_LEN = 128;
	//const char sine_wave[32] = {-1,23,47,70,89,105,117,124,127,124,117,105,89,70,47,23,-1,-25,-49,-72,-91,-107,-119,-126,-128,-126,-119,-107,-91,-72,-49,-25};
	//const char saw_wave[32] = {-128,-120,-112,-103,-95,-87,-79,-70,-62,-54,-46,-38,-29,-21,-13,-5,4,12,20,28,37,45,53,61,69,78,86,94,102,111,119,127};
	//const char sine_wave[WAVE_LEN] = 	
	//{
		//-128,-126,-124,-122,-120,-118,-116,-114,-112,-110,-108,-106,-104,-102,-100,-98,-96,-94,-92,-90,-88,-86,-84,-82,-80,-78,-76,-74,-72,-70,-68,-66,-64,-62,-60,-58,-56,-54,-52,-50,-48,-46,-44,-42,-40,-38,-36,-34,-32,-30,-28,-26,-24,-22,-20,-18,-16,-14,-12,-10,-8,-6,-4,-2,0,2,4,6,8,10,12,14,16,18,20,22,24,26,28,30,32,34,36,38,40,42,44,46,48,50,52,54,56,58,60,62,64,66,68,70,72,74,76,78,80,82,84,86,88,90,92,94,96,98,100,102,104,106,108,110,112,114,116,118,120,122,124,126  //pure saw
	//};
	const char saw_wave[WAVE_LEN] = 
	{
		0,6,12,19,25,31,37,43,49,54,60,65,71,76,81,85,90,94,98,102,106,109,112,115,117,120,122,123,125,126,126,127,127,127,126,126,125,123,122,120,117,115,112,109,106,102,98,94,90,85,81,76,71,65,60,54,49,43,37,31,25,19,12,6,0,-6,-12,-19,-25,-31,-37,-43,-49,-54,-60,-65,-71,-76,-81,-85,-90,-94,-98,-102,-106,-109,-112,-115,-117,-120,-122,-123,-125,-126,-126,-127,-127,-127,-126,-126,-125,-123,-122,-120,-117,-115,-112,-109,-106,-102,-98,-94,-90,-85,-81,-76,-71,-65,-60,-54,-49,-43,-37,-31,-25,-19,-12,-6 //pure sine
	};
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
	void writeCvCalib(unsigned char cvIp, unsigned int addr);
}; //AteOscHardwareTester

#endif //__ATEOSCHARDWARETESTER_H__
