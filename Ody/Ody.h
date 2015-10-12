//Ody.h  Odytron class (add this object to Arduino sketch)
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


#ifndef __ODY_H__
#define __ODY_H__

#include "OdyEngineBase.h"
#include "AtmHardwareBase.h"
#include "OdyEngine.h"
#include "AtmHardware.h"
#include "OdyOscillatorProgmem.h"

class Ody : public AtmHardwareBase, OdyEngineBase 
{
//variables
public:
protected:
private:
	AtmHardware& hardware_;
	OdyEngine& engine_;
	const unsigned char LED_FLASH_TICKS = 1;
	static const unsigned char MIDI_UBRR = 63; //(F_CPU / (MIDI_BAUD*8)) - 1
	unsigned char testNote_ = 60;
//functions
public:
	Ody();
	~Ody();
	void initialize();
	void poll();
	void hardwareSwitchChanged(unsigned char sw, unsigned char newValue);
	void hardwareSwitchHeld(unsigned char sw);
	void hardwareAnalogueControlChanged(unsigned char control, unsigned char newValue);
	void hardwareRotaryEncoderChanged(unsigned char rotary, unsigned char newValue, bool clockwise);
	void hardwareMidiChannelChanged(unsigned char channel);
	void hardwareMidiReceived(unsigned char data);
	void hardwareMidiError(unsigned char errorType);
	void engineFunctionChanged(unsigned char func, unsigned char val, bool opt);
protected:
private:
	Ody( const Ody &c );
	Ody& operator=( const Ody &c );

}; //Ody

#endif //__ODY_H__
