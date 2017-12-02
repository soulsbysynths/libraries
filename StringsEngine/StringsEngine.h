//StringsEngine.h  Audio engine for Strings
//Copyright (C) 2016  Paul Soulsby info@soulsbysynths.com
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


#ifndef __STRINGSENGINE_H__
#define __STRINGSENGINE_H__

//#include <avr/io.h>
#include <avr/interrupt.h>
#include <math.h>

#include "AtmPatch.h"
#include "AtmPatchBase.h"
#include "StringsEngineProgmem.h"
//#include "StringsEngineSettings.h"
#include "StringsEngineBase.h"
#include "Midi.h"
#include "MidiBase.h"
#include "Envelope.h"
#include "SsHelpers.h"
#include "Lfo.h"
#include "MasterClock.h"
#include "Arpeggiator.h"
#include "ArpeggiatorBase.h"

#define OSCS 4
#define BPM 120
#define BPM_TICKS 3840000UL/BPM
#ifndef HIGH
#define  HIGH 0x01
#endif
#ifndef LOW
#define LOW 0x00
#endif

class StringsEngine : public MidiBase, AtmPatchBase, ArpeggiatorBase
{
	//variables
	public:
	static StringsEngine& getInstance()
	{
		static StringsEngine instance; // Guaranteed to be destroyed.
		// Instantiated on first use.
		return instance;
	}
	enum Ctrl : unsigned char
	{
		CTRL_DETUNE,
		CTRL_PITCHLFODEPTH,
		CTRL_PITCHENVDEPTH,
		CTRL_PHASESPEED,
		CTRL_PHASERDEPTH,
		CTRL_PHASERSOURCE,
	};
	enum Func : unsigned char
	{
		FUNC_WAVEA,
		FUNC_WAVEB,
		FUNC_WAVEC,
		FUNC_WAVED,
		FUNC_CYCLER,
		FUNC_ATTACK,
		FUNC_DECAY,
		FUNC_SUSTAIN,
		FUNC_RELEASE,
		FUNC_PITCHLFOWAVE,
		FUNC_PITCHLFOSPEED,
		FUNC_ARPPATTERN,
		FUNC_ARPSPEED,
		FUNC_PHASELFOWAVE,
		FUNC_PHASELFOSPEED,
		FUNC_MEM
	};
	protected:
	private:
	static const unsigned char SYSEX_PROD_ID = 0;
	static const unsigned int MIDI_TICKSPERCYCLE = 1536;
	const float MAX_PITCH = 0.1155245298177;  //ln 12root2 , -1. *2
	const unsigned char MULT_PITCH = 228;
	//const unsigned char BS_PITCH = 7;
	Func function_ = FUNC_WAVEA;
	Midi* midi_;
	AtmPatch* patch_;
	StringsEngineBase* base_ = NULL;
	Envelope env_[OSCS];
	Lfo lfoPhase_;
	Lfo lfoPitch_;
	Lfo lfoCycle_;
	MasterClock masterClock_;
	Arpeggiator* arpeggiator_;
	unsigned int freqBase_[OSCS]={0};
	float pitchLfoAmt_ = 0;
	float pitchEnvAmt_ = 0;
	char pitchBend_ = 0;
	unsigned char phaserSource_ = 0;
	unsigned char oscNotes_[OSCS];
	unsigned char oscNoteOrder_[OSCS];
	unsigned long detuneAmt_ = 0;  //Osc spread or detuneAmt_
	unsigned char curOsc_ = 0;
	unsigned char lfoSpeed_ = 0;
	bool mono_ = false;
	bool trig_ = false;  //MIDItrig 1=note ON
	unsigned char totNotesOnLast_ = 0;

	//functions
	public:
	void construct(StringsEngineBase* base);
	const AtmPatch* getPatchPtr() const { return  patch_; }
	AtmPatch* getPatchPtr() { return patch_; }
	const Midi* getMidiPtr() const { return  midi_; }
	Midi* getMidiPtr() { return midi_; }
	void initialize();
	void poll(unsigned char ticksPassed);
	void setFunction(StringsEngine::Func new_func);
	StringsEngine::Func getFunction(){return function_;}
	void setMono(bool newValue);
	bool getMono(){return mono_;}
	void patchValueChanged(unsigned char func, unsigned char newValue);
	void patchOptionChanged(unsigned char func, bool newOpt);
	void patchCtrlChanged(unsigned char bank, unsigned char ctrl, unsigned char newValue);
	void midiControlChangeReceived(unsigned char cc, unsigned char val);
	void midiNoteOnReceived(unsigned char note, unsigned char velocity);
	void midiNoteOffReceived(unsigned char note);
	void midiClockStartReceived(void);
	void midiClockStopReceived(void);
	void midiSysexStartReceived(void){}
	void midiSysexDataReceived(unsigned char index, unsigned char data){}
	void midiSysexStopReceived(void){}
	void midiSysexWrite(unsigned char data){}
	void midiPitchBendReceived(char bend);
	void midiProgramChangeReceived(unsigned char patchNum);
	void arpeggiatorNoteEvent(unsigned char lastNote, unsigned char newNote);
	protected:
	private:
	StringsEngine(StringsEngineBase* base);
	StringsEngine(){}
	StringsEngine( const StringsEngine &c );
	~StringsEngine();
	StringsEngine& operator=( const StringsEngine &c );
	void refreshFreq(unsigned char index);
	unsigned char calcIntegratorInc(unsigned char note);
	void tieOptions(Func minFunc, Func maxFunc, bool newOpt);
	void tieValues(Func minFunc, Func maxFunc, unsigned char newVal);
	void triggerNote(unsigned char note);
	void releaseNote(unsigned char note);
	void cycleWaves();
}; //StringsEngine

#endif //__STRINGSENGINE_H__
