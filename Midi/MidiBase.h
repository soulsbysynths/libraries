//MidiBase.h  Midi processing base class
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


#ifndef MIDI_BASE_H_
#define MIDI_BASE_H_

class MidiBase
{
	public:
	virtual void midiNoteOnReceived(unsigned char note, unsigned char velocity) = 0;
	virtual void midiNoteOffReceived(unsigned char note) = 0;
	virtual void midiClockStartReceived(void) = 0;
	virtual void midiClockStopReceived(void) = 0;
	virtual void midiSysexStartReceived(void) = 0;
	virtual void midiSysexDataReceived(unsigned char index, unsigned char data) = 0;
	virtual void midiSysexStopReceived(void) = 0;
	virtual void midiSysexWrite(unsigned char data) = 0;
	virtual void midiControlChangeReceived(unsigned char cc, unsigned char val) = 0;
	virtual void midiPitchBendReceived(char bend) = 0;
};

#endif /* MIDI_BASE_H_ */