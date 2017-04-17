/*
* ArpAtarpEngine.h
*
* Created: 12/04/2017 20:09:11
* Author: paulsoulsby
*/


#ifndef __ARPATARPENGINE_H__
#define __ARPATARPENGINE_H__

#include "Midi.h"
#include "Midi/MidiBase.h"
#include "MasterClock.h"
#include "ArpAtarpEngineBase.h"
#include "Arpeggiator/Arpeggiator.h"
#include "Arpeggiator/ArpeggiatorBase.h"
#include "ChordGen/ChordGen.h"

#define BPM 120
#define BPM_TICKS 3840000UL/BPM

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
		FUNC_NOTESOURCE,
		FUNC_CLOCKSOURCE,
		FUNC_MIDICHANNEL,
		FUNC_RECORD,
		FUNC_RESET
	};
	enum ClockSource : unsigned char
	{
		CS_INTERNAL = 0,
		CS_EXTERNAL = 1
	};
	enum NoteSource : unsigned char
	{
		NS_MIDI = 0,
		NS_CV = 1
	};
	static ArpAtarpEngine& getInstance()
	{
		static ArpAtarpEngine instance; // Guaranteed to be destroyed.
		// Instantiated on first use.
		return instance;
	}
	static const unsigned char FUNCS = 8;
	protected:
	private:
	static const unsigned char SYSEX_PROD_ID = 0;
	static const unsigned int MIDI_TICKSPERCYCLE = 1536;
	ArpAtarpEngineBase* base_ = NULL;
	MasterClock masterClock_;
	Midi* midi_;
	Arpeggiator* arpeggiator_;
	ChordGen chordGen_;
	unsigned int output_;
	unsigned char totNotesOnLast_ = 0;
	Function func_ = FUNC_PATTERN;
	unsigned char value_[FUNCS];
	NoteSource noteSource_;
	ClockSource clockSource_;
	//functions
	public:
	void construct(ArpAtarpEngineBase* base);
	void initialize();
	void poll(unsigned char ticksPassed);
	const Arpeggiator* getArpeggiatorPtr() const { return  arpeggiator_; }
	Arpeggiator* getArpeggiatorPtr() { return arpeggiator_; }
	const Midi* getMidiPtr() const { return  midi_; }
	Midi* getMidiPtr() { return midi_; }
	MasterClock& getMasterClock() { return masterClock_; }
	const MasterClock& getMasterClock() const { return masterClock_; }
	ChordGen& getChordGen() {return chordGen_;}
	const ChordGen& getChordGen() const {return chordGen_;}
	void setFunction(Function newFunction);
	Function getFunction(){return func_;}
	void setValue(Function func, unsigned char newValue);
	unsigned char getValue(Function func){return value_[func];}
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
