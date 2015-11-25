/* 
* OdyEngine.h
*
* Created: 23/09/2015 13:15:25
* Author: Paul Soulsby
*/


#ifndef __ODYENGINE_H__
#define __ODYENGINE_H__

#include "OdyAudio.h"
#include "OdyOscillator.h"
#include "OdyNoise.h"
#include "OdyPatch.h"
#include "OdyPatchBase.h"
#include "Envelope.h"
#include "OdyAmplifier.h"
#include "OdyEngineProgmem.h"
#include "OdyEngineBase.h"
//#include "OdyEngineSettings.h"
#include "OdyFilter.h"
#include "OdyHpf.h"
#include "OdyLfo.h"
#include "Portamento.h"
#include "OdyPitch.h"
#include "Midi.h"
#include "MidiBase.h"
#include "SsHelpers.h"



class OdyEngine : public MidiBase, OdyPatchBase
{
//variables
public:
	static OdyEngine& getInstance()
	{
		static OdyEngine instance; // Guaranteed to be destroyed.
		// Instantiated on first use.
		return instance;
	}
	enum Ctrl : unsigned char
	{
		CTRL_VCO1,
		CTRL_VCO2,
		CTRL_LFO,
		CTRL_FC,
		CTRL_Q,
		CTRL_HPF
	};
	enum Func : unsigned char
	{
		FUNC_OSC0FMA,
		FUNC_OSC0FMB,
		FUNC_OSC1FMA,
		FUNC_OSC1FMB,
		FUNC_OSC1PW,
		FUNC_OSC1PWM,
		FUNC_OSCLEVELFX,
		FUNC_OSCLEVEL0,
		FUNC_OSCLEVEL1,
		FUNC_FILTFMA,
		FUNC_FILTFMB,
		FUNC_ENVR,
		FUNC_ENVS,
		FUNC_ENVD,
		FUNC_ENVA,
		FUNC_PORTAMENTO,
		FUNC_ENVR2,
		FUNC_ENVA2,
		FUNC_MEM,
		FUNC_FILTTYPE
	};
	enum MidiCC : unsigned char
	{
		CC_MODWHEEL = 1,
		CC_PORTAMENTO = 5,
		CC_FILTRES = 71,
		CC_ENVR = 72,
		CC_ENVA = 73,
		CC_FILTCUTOFF = 74,
		CC_ENVD = 75,
		CC_LFOSPEED = 76,
		CC_FILTLFO = 77,
		CC_FILTENV = 78,
		CC_OSC1PITCH = 79,
		CC_OSC2PITCH = 80,
		CC_HPF = 81,
		CC_OSC1FMB = 82,
		CC_OSC2FMB = 83,
		CC_ALLNOTESOFF = 123
	};
	enum FxSource : unsigned char
	{
		FX_NOISE,
		FX_RINGMOD
	};
	static const unsigned int WAVE_LENGTH = 256;//64;
protected:
private:
	static const unsigned char SYSEX_PROD_ID = 0;
	static const unsigned int MIDI_TICKSPERCYCLE = 1536;
	
	
	FxSource fxSource_ = FX_NOISE;
	unsigned char fxLevel_ = 7;
	
	//MasterClock masterClock_;
	Midi* midi_;
	OdyPatch* patch_;
	OdyOscillator oscillator_[2];
	OdyNoise noise_;
	Func function_ = FUNC_OSC0FMA;
	OdyEngineBase* base_ = NULL;
	OdyAudio audio_;
	Envelope adsrEnvelope_;
	Envelope arEnvelope_;
	OdyAmplifier amplifier_;
	OdyFilter filter_;
	OdyHpf hpf_;
	OdyLfo lfo_;
	Portamento portamento_[2];
	OdyPitch pitch_[2];
	unsigned char totNotesOn_ = 0;
	unsigned char totNotesOnLast_ = 0;
	char pitchBend_ = 0;
	bool noteOn_[128] = {false};
	unsigned char noteLowest_ = 127;
	unsigned char noteHighest_ = 0;

	//functions
	public:
	void construct(OdyEngineBase* base);
	const OdyPatch* getPatchPtr() const { return  patch_; }
	OdyPatch* getPatchPtr() { return patch_; }
	const Midi* getMidiPtr() const { return  midi_; }
	Midi* getMidiPtr() { return midi_; }
	OdyFilter& getFilter() { return filter_; }
	const OdyFilter& getFilter() const { return filter_; }
	OdyHpf& getHPF() { return hpf_; }
	const OdyHpf& getHPF() const { return hpf_; }
	OdyAmplifier& getAmp() { return amplifier_; }
	const OdyAmplifier& getAmp() const { return amplifier_; }
	OdyOscillator& getOsc(unsigned char oscNum) { return oscillator_[oscNum]; }
	const OdyOscillator& getOsc(unsigned char oscNum) const { return oscillator_[oscNum]; }
	void initialize();
	void poll(unsigned char ticksPassed);
	void setFunction(OdyEngine::Func new_func);
	OdyEngine::Func getFunction(){return function_;}
	void setBank(unsigned char newBank);
	FxSource getFxSource(){return fxSource_;}
	unsigned char getFxLevel(){return fxLevel_;}
	void patchValueChanged(unsigned char func, unsigned char newValue);
	void patchOptionChanged(unsigned char func, bool new_opt);
	void patchCtrlChanged(unsigned char anlControl_, unsigned char newValue);
	void midiControlChangeReceived(unsigned char anlControl_, unsigned char val);
	void midiNoteOnReceived(unsigned char note, unsigned char velocity);
	void midiNoteOffReceived(unsigned char note);
	void midiClockStartReceived(void){}
	void midiClockStopReceived(void){}
	void midiSysexStartReceived(void){}
	void midiSysexDataReceived(unsigned char index, unsigned char data){}
	void midiSysexStopReceived(void){}
	void midiSysexWrite(unsigned char data){}
	void midiChannelChanged(unsigned char channel);
	void midiPitchBendReceived(char bend);
	protected:
	private:
	OdyEngine(OdyEngineBase* base);
	OdyEngine() {}
	OdyEngine( const OdyEngine &c );
	~OdyEngine();
	OdyEngine& operator=( const OdyEngine &c );
	void triggerNote(unsigned char note);
	void releaseNote();
	
}; //OdyEngine

#endif //__ODYENGINE_H__
