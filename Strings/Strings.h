//Strings.h  Strings top level class (add this object to Arduino sketch)
//Copyright (C) 2016  Paul Soulsby info@soulsbysynths.com
//
//This program is free software: you can redistribute it and/or modAmt_ify
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


#ifndef __STRINGS_H__
#define __STRINGS_H__

#include "StringsEngineBase.h"
#include "AtmHardwareBase.h"
#include "StringsEngine.h"
#include "AtmHardware.h"

class Strings : public AtmHardwareBase, StringsEngineBase
{
//variables
public:
protected:
private:
	AtmHardware& hardware_;
	StringsEngine& engine_;
	const unsigned char IS_FUNC_FILL[2] = {0xE0,0x01};
	const unsigned char LED_FLASH_TICKS = 4;
	static const unsigned char MIDI_UBRR = 63; //(F_CPU / (MIDI_BAUD*8)) - 1
	unsigned char testNote_ = 60;
	
//functions
public:
	Strings();
	~Strings();
	void initialize();
	void poll(unsigned char ticksPassed);
	void hardwareSwitchChanged(unsigned char sw, unsigned char newValue);
	void hardwareSwitchHeld(unsigned char sw);
	void hardwareAnalogueControlChanged(unsigned char control, unsigned char newValue);
	void hardwareRotaryEncoderChanged(unsigned char rotary, unsigned char newValue, bool clockwise);
	void hardwareMidiChannelChanged(unsigned char channel);
	void hardwareMidiReceived(unsigned char data);
	void hardwareMidiError(unsigned char errorType);
	void engineFunctionChanged(unsigned char func, unsigned char val);
	void engineOptionChanged(unsigned char opt, bool val);
protected:
private:
	Strings( const Strings &c );
	Strings& operator=( const Strings &c );
	bool isFuncFill(StringsEngine::Func func) const;
}; //Strings

#endif //__STRINGS_H__
