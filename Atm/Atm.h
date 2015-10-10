//Atm.h  Atmegatron class (add this object to Arduino sketch)
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


#ifndef __ATM_H__
#define __ATM_H__

#include "AtmEngineBase.h"
#include "AtmHardwareBase.h"
#include "AtmEngine.h"
#include "AtmHardware.h"

class Atm : public AtmHardwareBase, AtmEngineBase 
{
//variables
public:
protected:
private:
	AtmHardware& hardware_;
	AtmEngine& engine_;
	const unsigned char IS_FUNC_FILL[2] = {0xFC,0x21};
	const unsigned char IS_SHIFT_HOLD[2] = {0x20,0x81};
	const unsigned char LED_FLASH_TICKS = 1;
	static const unsigned char MIDI_UBRR = 63; //(F_CPU / (MIDI_BAUD*8)) - 1
	unsigned char testNote_ = 60;
//functions
public:
	Atm();
	~Atm();
	void initialize();
	void poll(unsigned char ticksPassed);
	void hardwareSwitchChanged(unsigned char sw, unsigned char newValue);
	void hardwareSwitchHeld(unsigned char sw);
	void hardwareAnalogueControlChanged(unsigned char control, unsigned char newValue);
	void hardwareRotaryEncoderChanged(unsigned char rotary, unsigned char newValue, bool clockwise);
	void hardMidiChannelChanged(unsigned char channel);
	void hardwareMidiReceived(unsigned char data);
	void hardwareMidiError(unsigned char errorType);
	void engineFunctionChanged(unsigned char func, unsigned char val, bool opt);
	void engineBankChanged(unsigned char bank);
	void engineMidiTransmit(unsigned char data);
	void engineSysexComplete();
protected:
private:
	Atm( const Atm &c );
	Atm& operator=( const Atm &c );
	bool isFuncFill(AtmEngine::Func func) const;
	bool isShiftHold(AtmEngine::Func func);
}; //Atm

#endif //__ATM_H__
