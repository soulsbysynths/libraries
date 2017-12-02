//MulAtmegaAtm.h  Atmegatron class (add this object to Arduino sketch)
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


#ifndef __MULATMEGAATM_H__
#define __MULATMEGAATM_H__

#include "MulAtmegaAtmEngineBase.h"
#include "MulAtmegaHardwareBase.h"
#include "MulAtmegaAtmEngine.h"
#include "MulAtmegaHardware.h"

class MulAtmegaAtm : public MulAtmegaHardwareBase, MulAtmegaAtmEngineBase 
{
//variables
public:
protected:
private:
	MulAtmegaHardware& hardware_;
	MulAtmegaAtmEngine& engine_;
	const unsigned char IS_FUNC_FILL[2] = {0xFC,0x21};
	const unsigned char IS_SHIFT_HOLD[2] = {0x20,0x81};
	const unsigned char LED_FLASH_TICKS = 4;
	static const unsigned char MIDI_UBRR = 63; //(F_CPU / (MIDI_BAUD*8)) - 1
	unsigned char testNote_ = 60;
	unsigned char curVoice_ = 0;
//functions
public:
	MulAtmegaAtm();
	~MulAtmegaAtm();
	void initialize();
	void poll(unsigned char ticksPassed);
	void hardwareSwitchChanged(unsigned char sw, unsigned char newValue);
	void hardwareSwitchHeld(unsigned char sw);
	void hardwareAnalogueControlChanged(unsigned char control, unsigned char newValue);
	void hardwareRotaryEncoderChanged(unsigned char rotary, unsigned char newValue, bool clockwise);
	void hardwareMidiChannelChanged(unsigned char channel);
	void hardwareMidiReceived(unsigned char data);
	void hardwareMidiError(unsigned char errorType);
	void engineVoiceChanged(unsigned char voice);
	void engineFunctionChanged(unsigned char func);
	void engineValueChanged(unsigned char val);
	void engineOptionChanged(bool opt);
	void engineBankChanged(unsigned char bank);
	void engineMidiTransmit(unsigned char data);
	void engineSysexComplete();
	void engineSlaveTransmit(unsigned char id, unsigned char * data, size_t length);
	void engineVoiceTriggered(unsigned char voice, bool on);
protected:
private:
	MulAtmegaAtm( const MulAtmegaAtm &c );
	MulAtmegaAtm& operator=( const MulAtmegaAtm &c );
	bool isFuncFill(MulAtmegaAtmEngine::Func func) const;
	bool isShiftHold(MulAtmegaAtmEngine::Func func);
}; //MulAtmegaAtm

#endif //__MULATMEGAATM_H__
