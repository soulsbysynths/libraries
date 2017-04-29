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
#include <string.h>
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
	enum MidiCC
	{
		CC_BANKSELECT = 0,
		CC_MODWHEEL = 1,
		CC_BREATH = 2,
		CC_FOOTCONTROLLER = 4,
		CC_PORTAMENTO = 5,
		CC_DATAENTRYMSB = 6,
		CC_VOLUME = 7,
		CC_BALANCE = 8,
		CC_PAN = 10,
		CC_EXPRESSION = 11,
		CC_EFFECT1 = 12,
		CC_EFFECT2 = 13,
		CC_GP1 = 16,
		CC_GP2 = 17,
		CC_GP3 = 18,
		CC_GP4 = 19,
		CC_BANKSELECTLSB = 32,
		CC_MODWHEELLSB = 33,
		CC_BREATHLSB = 34,
		CC_FOOTCONTROLLERLSB = 36,
		CC_PORTAMENTOLSB = 37,
		CC_DATAENTRYLSB = 38,
		CC_VOLUMELSB = 39,
		CC_BALANCELSB = 40,
		CC_GP1LSB = 48,
		CC_GP2LSB = 49,
		CC_GP3LSB = 50,
		CC_GP4LSB = 51,
		CC_SUSTAIN = 64,
		CC_PORTAMENTOON = 65,
		CC_SOSTENUTO = 66,
		CC_SOFTPEDAL = 67,
		CC_LEGATO = 68,
		CC_HOLD2 = 69,
		CC_SOUNDVARIATION = 70,
		CC_TIMBRE = 71,
		CC_RELEASE = 72,
		CC_ATTACK = 73,
		CC_BRIGHTNESS = 74,
		CC_DECAY = 75,
		CC_VIBRATO = 76,
		CC_VIBDEPTH = 77,
		CC_VIBDELAY = 78,
		CC_GP5 = 80,
		CC_GP6 = 81,
		CC_GP7 = 82,
		CC_GP8 = 83,
		CC_PORTAMENOCONTROL = 84,
		CC_REVERB = 91,
		CC_TREMOLO = 92,
		CC_CHORUS = 93,
		CC_DETUNE = 94,
		CC_PHASER = 95,
		CC_ALLNOTESOFF = 123
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
	unsigned char totNotesOn_ = 0;
	bool noteOn_[128] = {false};
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
	void setNoteOn(unsigned char note, bool on){noteOn_[note] = on;}
	bool getNoteOn(unsigned char note){return noteOn_[note];}
	const bool* getNoteOnPtr() const { return  noteOn_; }
	bool* getNoteOnPtr() { return noteOn_; }
	unsigned char getTotNotesOn(){return totNotesOn_;}
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
