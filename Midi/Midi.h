//Midi.h  Midi processing class
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

#ifndef __MIDI_H__
#define __MIDI_H__

#include "MidiBase.h"

class Midi
{
	//variables
	public:
	enum MidiType
	{
		INVALID_TYPE           = 0x00,    ///< For notifying errors
		NOTE_OFF               = 0x80,    ///< Note Off
		NOTE_ON                = 0x90,    ///< Note On
		AFTER_TOUCH_POLY        = 0xA0,    ///< Polyphonic AfterTouch
		CONTROL_CHANGE         = 0xB0,    ///< Control Change / Channel Mode
		PROGRAM_CHANGE         = 0xC0,    ///< Program Change
		AFTER_TOUCH_CHANNEL     = 0xD0,    ///< Channel (monophonic) AfterTouch
		PITCH_BEND             = 0xE0,    ///< Pitch Bend
		SYSEX_START            = 0xF0,    ///< System Exclusive start
		TIMECODE_QUARTER_FRAME  = 0xF1,    ///< System Common - MIDI Time Code Quarter Frame
		SONG_POSITION          = 0xF2,    ///< System Common - Song Position Pointer
		SONG_SELECT            = 0xF3,    ///< System Common - Song select
		TUNE_REQUEST           = 0xF6,    ///< System Common - Tune Request
		SYSEX_STOP             = 0xF7,    ///< System Exclusive stop
		CLOCK                 = 0xF8,    ///< System Real Time - Timing CLOCK
		START                 = 0xFA,    ///< System Real Time - START
		CONTINUE              = 0xFB,    ///< System Real Time - CONTINUE
		STOP                  = 0xFC,    ///< System Real Time - STOP
		ACTIVE_SENSING         = 0xFE,    ///< System Real Time - Active Sensing
		SYSTEM_RESET           = 0xFF,    ///< System Real Time - System reset
	};
	const unsigned char SYSEX_MANUFACTURER = 0x7D;
	protected:
	private:
	unsigned char channel_ = 0;
	MidiBase* base_;
	bool clockRunning_ = false;
	bool sysexRx_ = false;
	unsigned char sysexIndex_ = 0;
	unsigned char sysexProductId_ = 0;
	bool sysexEnable_ = false;
	unsigned int clockTicks_ = 0;
	unsigned int clockTicksLast_ = 0; 
	//functions
	public:
	Midi(MidiBase* base, unsigned char sysex_prod_id);
	~Midi();
	void read(unsigned char in_byte);
	bool getClockRunning(){return clockRunning_;}
	void setSysexEnable(bool newValue){sysexEnable_ = newValue;}
	bool getSysexEnable(){return sysexEnable_;}
	unsigned char getChannel(){return channel_;}
	void setChannel(unsigned char new_channel);
	void reset();
	void writeSysex(unsigned char* sys_mess, unsigned int size);
	unsigned char getClockTicksPassed();
	protected:
	private:
	Midi() {}
	Midi( const Midi &c );
	Midi& operator=( const Midi &c );

}; //Midi

#endif //__MIDI_H__
