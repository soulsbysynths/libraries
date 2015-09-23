//Min.h  miniAtmegatron class (add this object to Arduino sketch)
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

#ifndef __MIN_H__
#define __MIN_H__

#include "MinEngineBase.h"
#include "MinHardwareBase.h"
#include "MinEngine.h"
#include "MinHardware.h"
#include "LedRgb.h"

class Min : public MinHardwareBase, MinEngineBase 
{
//variables
public:
protected:
private:
	MinEngine& engine_;
	MinHardware& hardware_;
	static const unsigned char MIDI_UBRR = 63; //(F_CPU / (MIDI_BAUD*8)) - 1
//functions
public:
	Min();
	~Min();
	void initialize();
	void poll(unsigned char ticksPassed);
	void hardwareSwitchChanged(unsigned char switch_, unsigned char newValue);
	void hardwareSwitchHeld(unsigned char switch_);
	void hardwareAnalogueControlChanged(unsigned char control, unsigned char newValue);
	void hardwareMidiReceived(unsigned char data);
	void hardwareMidiError(unsigned char errorType){}
	void engineValueChanged(unsigned char func, unsigned char val, bool opt);
	void engineBankChanged(unsigned char bank);
	void engineMidiTransmit(unsigned char data);
protected:
private:
	Min( const Min &c );
	Min& operator=( const Min &c );
	void refreshValueLed();
}; //Min

#endif //__MIN_H__
