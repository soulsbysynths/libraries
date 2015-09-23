//AtmHardwareTester.h  Atmegatron Hardware Tester. Used by assembly factory.
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

#ifndef __ATMHARDWARETESTER_H__
#define __ATMHARDWARETESTER_H__

#include <AtmHardware.h>
#include <AtmHardwareBase.h>
#include <AtmAudio.h>
#include <Wavetable.h>
#include <avr/pgmspace.h>

class AtmHardwareTester : public AtmHardwareBase
{
//variables
public:
protected:
private:
	const unsigned char NOTE_OFF = 0x80;   ///< Note Off
	const unsigned char NOTE_ON  = 0x90;    ///< Note On
	const unsigned char TEST_NOTE = 60;
	const unsigned char WAVE_LEN = 32;
	static const unsigned char MIDI_UBRR = 63;
	const char sine_wave[32] = {-1,23,47,70,89,105,117,124,127,124,117,105,89,70,47,23,-1,-25,-49,-72,-91,-107,-119,-126,-128,-126,-119,-107,-91,-72,-49,-25};
	AtmHardware& hardware_;
	AtmAudio* audio_;
	unsigned char tick_;
	bool circTicking_[2] = {true,true};
	bool swTicking_[3] = {true,true,true};
	bool lastMoving_[6] = {false};
//functions
public:
	AtmHardwareTester();
	~AtmHardwareTester();
	void init();
	void tick();
	void poll(unsigned char ticksPassed);
	void hardwareSwitchChanged(unsigned char sw, unsigned char newValue);
	void hardwareSwitchHeld(unsigned char sw);
	void hardwareAnalogueControlChanged(unsigned char control, unsigned char newValue);
	void hardwareRotaryEncoderChanged(unsigned char rotary, unsigned char newValue, bool clockwise);
	void hardwareMidiReceived(unsigned char data);
	void hardwareMidiError(unsigned char errorType){}
protected:
private:
	AtmHardwareTester( const AtmHardwareTester &c );
	AtmHardwareTester& operator=( const AtmHardwareTester &c );
	void playSound(bool way);
}; //AtmHardwareTester

#endif //__ATMHARDWARETESTER_H__
