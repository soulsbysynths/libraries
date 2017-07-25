//ArpAtarp.h  AT-ARP main class
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


#ifndef __ARPATARP_H__
#define __ARPATARP_H__

#include "ArpHardware.h"
#include "ArpHardwareBase.h"
#include "ArpAtarpEngine.h"
#include "ArpAtarpEngineBase.h"

#ifndef LOW
#define  LOW 0
#endif
#ifndef HIGH
#define  HIGH 1
#endif

class ArpAtarp : public ArpHardwareBase, ArpAtarpEngineBase
{
//variables
public:
protected:
private:
	static const unsigned char MIDI_UBRR = 63; //(F_CPU / (MIDI_BAUD*8)) - 1
	const char FUNC_DISPLAY[ArpAtarpEngine::FUNCS] = {'P','G','D','O','C','M','R','E'};
	const unsigned char FUNC_MAX[ArpAtarpEngine::FUNCS] = {31,1,15,5,2,15,15,2};
	const unsigned char PPQN_DISPLAY[3] = {1,2,4};
	const unsigned char PPQN_VALUE[3] = {4,24,48};
	static const char LED_FLASH_TICKS = 4;
	static const char LED_SHORT_FLASH_TICKS = 2;
	static const unsigned int EEPROM_PATCH_OFFSET = 256;
	ArpHardware& hardware_;
	ArpAtarpEngine& engine_;
	bool funcSelect_ = false;
	bool swHoldBlock_ = false;
	unsigned char gateTimeOut_ = 0;
//functions
public:
	ArpAtarp();
	~ArpAtarp();
	void init();
	void poll();
protected:
private:
	ArpAtarp( const ArpAtarp &c );
	ArpAtarp& operator=( const ArpAtarp &c );
	void hardwareSwitchChanged(unsigned char sw, unsigned char newValue);
	void hardwareSwitchHeld(unsigned char sw);
	void hardwareJackDetectChanged(unsigned char jack, bool newValue);
	void hardwareCvInputChanged(unsigned char input, unsigned int newValue);
	void hardwareGateInputChanged(bool newValue);
	void hardwareMidiChannelChanged(unsigned char channel);
	void hardwareMidiReceived(unsigned char data);
	void hardwareMidiError(unsigned char errorType);
	void engineOutputChanged(unsigned int newOutput);
	void engineFunctionChanged(unsigned char newFunction);
	void engineValueChanged(unsigned char newValue);
	void engineLoadUserPattern(unsigned char patternNum);
	void engineSaveUserPatternStep(unsigned char patternNum, unsigned char step, char value);
}; //ArpAtarp

#endif //__ARPATARP_H__
