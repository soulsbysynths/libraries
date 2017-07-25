//ArpHardwareTester.h  AT-ARP hardware tester
//Copyright (C) 2017  Paul Soulsby info@soulsbysynths.com
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


#ifndef __ARPHARDWARETESTER_H__
#define __ARPHARDWARETESTER_H__

#include "ArpHardware.h"
#include "ArpHardwareBase.h"
#include "Midi.h"
#include "Midi/MidiBase.h"
#ifndef HIGH
#define HIGH 0x1
#endif

class ArpHardwareTester : public ArpHardwareBase, MidiBase 
{
	//variables
	public:
	enum CalibMode : unsigned char
	{
		CB_ROOT = 0,
		CB_CHORD = 1,
		CB_OUTPUT = 2
	};
	protected:
	private:
	static const unsigned char MIDI_UBRR = 63; //(F_CPU / (MIDI_BAUD*8)) - 1
	static const unsigned char SYSEX_PROD_ID = 0;
	ArpHardware& hardware_;
	Midi* midi_;
	CalibMode calibMode_ = CB_OUTPUT;
	unsigned char midiNote_ = 0;
	//functions
	public:
	ArpHardwareTester();
	~ArpHardwareTester();
	void init();
	void initMemory();
	void poll(unsigned char ticksPassed);
	void hardwareSwitchChanged(unsigned char sw, unsigned char newValue);
	void hardwareSwitchHeld(unsigned char sw);
	void hardwareJackDetectChanged(unsigned char jack, bool newValue);
	void hardwareCvInputChanged(unsigned char input, unsigned int newValue);
	void hardwareGateInputChanged(bool newValue);
	void hardwareMidiChannelChanged(unsigned char channel);
	void hardwareMidiReceived(unsigned char data);
	void hardwareMidiError(unsigned char errorType);
	void midiControlChangeReceived(unsigned char cc, unsigned char val){}
	void midiNoteOnReceived(unsigned char note, unsigned char velocity);
	void midiNoteOffReceived(unsigned char note);
	void midiClockStartReceived(void){}
	void midiClockStopReceived(void){}
	void midiSysexStartReceived(void){}
	void midiSysexDataReceived(unsigned char index, unsigned char data){}
	void midiSysexStopReceived(void){}
	void midiSysexWrite(unsigned char data){}
	void midiPitchBendReceived(char bend){}
	protected:
	private:
	ArpHardwareTester( const ArpHardwareTester &c );
	ArpHardwareTester& operator=( const ArpHardwareTester &c );

}; //ArpHardwareTester

#endif //__ARPHARDWARETESTER_H__
