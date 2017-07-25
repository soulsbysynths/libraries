//ArpAtarpEngine.h  AT-ARP data processing engine
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

#ifndef __ARPATARPENGINE_H__
#define __ARPATARPENGINE_H__

#include "Midi.h"
#include "Midi/MidiBase.h"
#include "MasterClock.h"
#include "ArpAtarpEngineBase.h"
#include "Arpeggiator/Arpeggiator.h"
#include "Arpeggiator/ArpeggiatorBase.h"
#include "ChordGen/ChordGen.h"


class ArpAtarpEngine : MidiBase, ArpeggiatorBase
{
	//variables
	public:
	enum MidiCC : unsigned char
	{
		CC_ALLNOTESOFF = 123
	};
	enum Function : unsigned char
	{
		FUNC_PATTERN = 0,
		FUNC_PINGPONG,
		FUNC_DIV,
		FUNC_CVOCTAVE,
		FUNC_CLOCKPPQN,
		FUNC_MIDICHANNEL,
		FUNC_RECORD,
		FUNC_RESET
	};
	enum InputSource : unsigned char
	{
		IS_MIDI = 0,
		IS_CV = 1
	};
	static ArpAtarpEngine& getInstance()
	{
		static ArpAtarpEngine instance; // Guaranteed to be destroyed.
		// Instantiated on first use.
		return instance;
	}
	static const unsigned char FUNCS = 8;
	static const unsigned char ROOT_NOTE_OFFSET = 36;   //so incoming CV is between midi note 36 and 96
	protected:
	private:
	static const unsigned char SYSEX_PROD_ID = 0;
	static const unsigned int MIDI_TICKSPERCYCLE = 3072;  //  1536;  //for24ppqn
	
	ArpAtarpEngineBase* base_ = NULL;
	MasterClock masterClock_;
	Midi* midi_;
	Arpeggiator* arpeggiator_;
	ChordGen chordGen_;
	unsigned int output_;
	unsigned char totNotesOnLast_ = 0;
	Function func_ = FUNC_PATTERN;
	unsigned char value_[FUNCS];
	InputSource rootSource_;
	InputSource chordSource_;
	InputSource clockSource_;
	//functions
	public:
	void construct(ArpAtarpEngineBase* base);
	void initialize();
	void poll(unsigned int midiClksPassed);
	const Arpeggiator* getArpeggiatorPtr() const { return  arpeggiator_; }
	Arpeggiator* getArpeggiatorPtr() { return arpeggiator_; }
	const Midi* getMidiPtr() const { return  midi_; }
	Midi* getMidiPtr() { return midi_; }
	MasterClock& getMasterClock() { return masterClock_; }
	const MasterClock& getMasterClock() const { return masterClock_; }
	void setFunction(Function newFunction);
	Function getFunction(){return func_;}
	void setValue(Function func, unsigned char newValue);
	unsigned char getValue(Function func){return value_[func];}
	void setRootNote(unsigned char newNote);
	void setChordNote(unsigned char newNote);
	void setRootSource(InputSource newSource);
	InputSource getRootSource(){return rootSource_;}
	void setChordSource(InputSource newSource);
	InputSource getChordSource(){return chordSource_;}
	InputSource getClockSource(){return clockSource_;}
	void txChordGenMidi();
	void midiControlChangeReceived(unsigned char cc, unsigned char val);
	void midiNoteOnReceived(unsigned char note, unsigned char velocity);
	void midiNoteOffReceived(unsigned char note);
	void midiClockStartReceived(void);
	void midiClockStopReceived(void);
	void midiSysexStartReceived(void){}
	void midiSysexDataReceived(unsigned char index, unsigned char data){}
	void midiSysexStopReceived(void){}
	void midiSysexWrite(unsigned char data){}
	void midiPitchBendReceived(char bend){}  //poss add this as nice feature
	void arpeggiatorNoteEvent(unsigned char lastNote, unsigned char newNote);
	protected:
	private:
	ArpAtarpEngine(ArpAtarpEngineBase* base);
	ArpAtarpEngine() {}
	ArpAtarpEngine( const ArpAtarpEngine &c );
	~ArpAtarpEngine();
	ArpAtarpEngine& operator=( const ArpAtarpEngine &c );
	void triggerNote(unsigned char note);
}; //ArpAtarpEngine

#endif //__ARPATARPENGINE_H__
