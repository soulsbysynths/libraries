//MinHardware.h  Hardware control class for miniAtmegatron
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


#ifndef __MINHARDWARE_H__
#define __MINHARDWARE_H__

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <avr/eeprom.h>
#include "MinHardwareBase.h"
#include "SsHelpers.h"
#include "LedRgb.h"
#include "AnalogueControl.h"
#include "Switch.h"

#ifndef bitRead
#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#endif
#ifndef bitSet
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#endif
#ifndef bitClear
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#endif
#ifndef bitWrite
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))
#endif

#define NULL 0

#define HIGH 0x1
#define LOW  0x0

#define MIDI_BUFFER_SIZE 64

class MinHardware
{
	//variables
	public:
	static const unsigned char LED_FUNC = 0;
	static const unsigned char LED_VALUE = 1;
	static const unsigned char SW_PLAY = 0;
	static const unsigned char SW_FUNC_DEC = 1;
	static const unsigned char SW_FUNC_INC = 2;
	static const unsigned char SW_VAL_DEC = 3;
	static const unsigned char SW_VAL_INC = 4;
	static const unsigned char SW_BANK = 5;
	static const unsigned int HOLD_EVENT_TICKS = 2000;
	static const unsigned char SYSEX_PRODUCT = 0;
	
	enum MidiError : unsigned char
	{
		MIDIERR_NONE,
		MIDIERR_FRAME,
		MIDIERR_OVERRUN,
		MIDIERR_PARITY,
		MIDIERR_BUFFERFULL
	};
	protected:
	private:
	MinHardwareBase* base_;
	AnalogueControl anlControl_[6];
	Switch switch_[6];
	LedRgb led_[2];
	static const unsigned int FIRST_BOOT_ADDR = 1023;
	static const unsigned int FIRST_BOOT_VAL = 127;
	//functions
	public:
	static MinHardware& getInstance()
	{
		static MinHardware instance; // Guaranteed to be destroyed.
		return instance;  // Instantiated on first use.
	}
	void construct(MinHardwareBase* base);
	AnalogueControl& getAnlControl(unsigned char index) { return anlControl_[index]; }
	const AnalogueControl& getAnlControl(unsigned char index) const { return anlControl_[index]; }
	Switch& getSwitch(unsigned char index) { return switch_[index]; }
	const Switch& getSwitch(unsigned char index) const { return switch_[index]; }
	LedRgb& getLed(unsigned char index) { return led_[index]; }
	const LedRgb& getLed(unsigned char index) const { return led_[index]; }
	bool getFirstBoot();
	void setFirstBoot(bool newValue);
	void refreshLEDs();
	bool refreshFlash(unsigned char ticksPassed);
	void pollAnlControls(unsigned char ticksPassed);
	void pollSwitches(unsigned char ticksPassed);
	void beginMidi(unsigned int ubrr);
	void pollMidi();
	void writeMidi(unsigned char data);
	static void flushMidi();
	protected:
	private:
	MinHardware() {}
	MinHardware(MinHardwareBase* base);
	~MinHardware();
	MinHardware( const MinHardware &c );
	MinHardware& operator=( const MinHardware &c );

}; //MinHardware

#endif //__MINHARDWARE_H__
