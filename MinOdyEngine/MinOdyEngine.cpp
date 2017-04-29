//MinOdyEngine.cpp  Odytron audio engine singleton class
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

#include "MinOdyEngine.h"
extern void writeMemory(const void* data, void* startAddr, size_t size);
extern void readMemory(void* data, const void* startAddr, size_t size);


// default constructor
MinOdyEngine::MinOdyEngine(MinOdyEngineBase* base)
{
	construct(base);
} //MinOdyEngine

// default destructor
MinOdyEngine::~MinOdyEngine()
{
	//if(audio_!=NULL)
	//{
	//delete audio_;
	//}
	if(patch_!=NULL)
	{
		delete patch_;
	}
	if(midi_!=NULL)
	{
		delete midi_;
	}
	if(sequencer_!=NULL)
	{
		delete sequencer_;
	}
} //~MinOdyEngine

void MinOdyEngine::construct(MinOdyEngineBase* base)
{
	base_ = base;
	patch_ = new AtmPatch(this);
	setIntBpm(intBpm_);
	midi_ = new Midi(this,SYSEX_PROD_ID);
	sequencer_ = new StepSequencer(this);
}

void MinOdyEngine::initialize()
{

	audio_.initialize();
	midi_->setChannel(MIDI_CHANNEL);
	patch_->readPatch(PATCH_NUM);
	setFunction(FUNC_WAVE);
	setBank(LOW);

	//arEnvelope_.setSustain(32767);  not for mini port  //it's only an AR env, so force these to full
	//arEnvelope_.setDecay(32767);
	portamento_[0].setProportional(true);
	portamento_[1].setProportional(true);
	pitch_[0].setFmBSource(OdyPitch::ADSR);
	pitch_[1].setFmBSource(OdyPitch::ADSR);
	filter_.setFmASource(OdyFilter::SINE);
	oscillator_[1].setPwmSource(OdyOscillator::LFO);
}
void MinOdyEngine::initPatch()
{
	patch_->setFunctionValue(FUNC_WAVE,0);
	patch_->setFunctionValue(FUNC_FILT,2);
	patch_->setFunctionValue(FUNC_FENV,1);
	patch_->setFunctionValue(FUNC_AENV,4);
	patch_->setFunctionValue(FUNC_LFOTYPE,0);
	patch_->setFunctionValue(FUNC_LFOSPEED,8);
	patch_->setFunctionValue(FUNC_CRUSHPORTA,0);
	patch_->setFunctionValue(FUNC_PATTERN,1);
	patch_->setFunctionValue(FUNC_BPM,6);
	patch_->setCtrlValue(LOW,CTRL_FILT,64);
	patch_->setCtrlValue(LOW,CTRL_ENV,255);
	patch_->setCtrlValue(LOW,CTRL_LFO,0);
	patch_->setCtrlValue(LOW,CTRL_AMP,0);
	patch_->setCtrlValue(LOW,CTRL_FX,255);
	patch_->setCtrlValue(LOW,CTRL_VOLUME,255);
	patch_->setCtrlValue(HIGH,CTRL_FILT,50);
	patch_->setCtrlValue(HIGH,CTRL_ENV,0);
	patch_->setCtrlValue(HIGH,CTRL_LFO,0);
	patch_->setCtrlValue(HIGH,CTRL_AMP,0);
	patch_->setCtrlValue(HIGH,CTRL_FX,0);
	patch_->setCtrlValue(HIGH,CTRL_VOLUME,127);
}
void MinOdyEngine::poll(unsigned char ticksPassed)
{
	unsigned char i;
	char fmA,fmB;
	char filtA, filtB,am;

	if(midi_->getClockRunning()==true)
	{
		masterClock_.refresh(midi_->getClockTicksPassed());
	}
	else
	{
		masterClock_.refresh(ticksPassed);
	}
	noise_.refresh();
	lfo_.refresh(ticksPassed);
	sequencer_->refresh(masterClock_.getOutput());
	adsrEnvelope_.refresh(ticksPassed);
	arEnvelope_.refresh(ticksPassed);
	
	for(i=0;i<2;++i)
	{
		oscillator_[1].refresh(adsrEnvelope_.getOutput(),lfo_.getOutput(OdyLfo::SINE));
	}
	
	if(filter_.getFmASource()==OdyFilter::S_AND_H)
	{
		filtA = lfo_.getOutput(OdyLfo::S_AND_H);
	}
	else
	{
		filtA = lfo_.getOutput(OdyLfo::SINE);
	}
	if(filter_.getFmBSource()==OdyFilter::ADSR)
	{
		filtB = adsrEnvelope_.getExpOutput();
	}
	else
	{
		filtB = arEnvelope_.getExpOutput();
	}
	
	filter_.refresh(noteHighest_,filtA,filtB);
	
	for(i=0;i<2;++i)
	{
		portamento_[i].refresh(ticksPassed);
		pitch_[i].setInput(portamento_[i].getOutput());
		if(pitch_[i].getFmASource()==OdyPitch::SINE)
		{
			fmA = lfo_.getOutput(OdyLfo::SINE);
		}
		else
		{
			fmA = lfo_.getOutput(OdyLfo::SQUARE);
		}
		if(pitch_[i].getFmBSource()==OdyPitch::S_AND_H)
		{
			fmB = lfo_.getOutput(OdyLfo::S_AND_H);
		}
		else
		{
			fmB = adsrEnvelope_.getExpOutput();
		}
		pitch_[i].refresh(pitchBend_,fmA,fmB);
		audio_.setSampleFreq(i,(unsigned long)pitch_[i].getOutput()*(WAVE_LENGTH>>2));  //Finally scale freqs back down again (/4)
	}
	
	if(amplifier_.getAmSource()==OdyAmplifier::AR)
	{
		am = arEnvelope_.getExpOutput();
	}
	else
	{
		am = adsrEnvelope_.getExpOutput();
	}
	amplifier_.refresh(am);
}

void MinOdyEngine::setFunction(MinOdyEngine::Func newFunc)
{
	bool col;
	function_ = newFunc;
	base_->engineValueChanged((unsigned char)function_,patch_->getFunctionValue(function_),LOW);
}

void MinOdyEngine::setBank(unsigned char newBank)
{
	bank_ = newBank;
	base_->engineBankChanged(bank_);
}

void MinOdyEngine::triggerNote(unsigned char note)
{
	portamento_[0].setInput(pgm_read_word(&(MIDI_FREQS[note])));
	portamento_[1].setInput(pgm_read_word(&(MIDI_FREQS[note])));
	adsrEnvelope_.trigger();
	arEnvelope_.trigger();
	//lfo_.reset();  //maybe not right for mini
}
void MinOdyEngine::releaseNote()
{
	adsrEnvelope_.release();
	arEnvelope_.release();
}
void MinOdyEngine::setIntBpm(unsigned char newBPM)
{
	intBpm_ = newBPM;
	unsigned int b = ((unsigned int)newBPM * 10) + 60;
	masterClock_.setTicksPerCycle(3840000UL/b);
}
//***********************MIDI events********************************************
void MinOdyEngine::midiNoteOnReceived(unsigned char note, unsigned char velocity)
{

	totNotesOnLast_ = midi_->getTotNotesOn();
	if(note<noteLowest_)
	{
		noteLowest_ = note;
	}
	if(note>noteHighest_)
	{
		noteHighest_ = note;
	}
	if(sequencer_->getPattern()==0)
	{
		#if LEGATO==1
		if(midi_->getTotNotesOn()>1)
		{
			portamento_.setInput(pgm_read_word(&(MIDI_FREQS[note])));
		}
		else
		{
			triggerNote(note);
		}
		#else
		triggerNote(note);
		#endif
	}
	else
	{
		seqBaseNote_ = note;
	}
}

void MinOdyEngine::midiNoteOffReceived(unsigned char note)
{
	totNotesOnLast_ = midi_->getTotNotesOn();
	if(note==noteLowest_)
	{
		noteLowest_ = 127;
		for(unsigned char i=0;i<128;++i)
		{
			if(midi_->getNoteOn(i)==true)
			{
				noteLowest_ = i;
				break;
			}
		}
	}

	if(note==noteHighest_)
	{
		noteHighest_ = 0;
		for(unsigned char i=127;i!=0;--i)
		{
			if(midi_->getNoteOn(i)==true)
			{
				noteHighest_ = i;
				break;
			}
		}
	}

	if(midi_->getTotNotesOn()==0 && sequencer_->getPattern()==0)
	{
		releaseNote();
	}
	else if(sequencer_->getPattern()==0)
	{
		portamento_[0].setInput(pgm_read_word(&(MIDI_FREQS[noteLowest_])));
		portamento_[1].setInput(pgm_read_word(&(MIDI_FREQS[noteHighest_])));
	}
}

void MinOdyEngine::midiControlChangeReceived(unsigned char ctrl, unsigned char val)
{

	switch ((MidiCC)ctrl)
	{
		case CC_PITCHLFOMOD:
		patch_->setCtrlValue(HIGH,CTRL_LFO,val>>1);
		break;
		case CC_PORTAMENTO:
		patch_->setFunctionValue(FUNC_CRUSHPORTA, val>>3);
		break;
		case CC_FILTERENV:
		patch_->setCtrlValue(LOW,CTRL_ENV,val<<1);
		break;
		case CC_OSC1FREQ:
		patch_->setCtrlValue(LOW,CTRL_FX,val<<1);
		break;
		case CC_WAVEFORM:
		patch_->setFunctionValue(FUNC_WAVE,val>>3);
		break;
		case CC_FILTTYPE:
		patch_->setFunctionValue(FUNC_FILT,val>>3);
		break;
		case CC_FILTENVSHAPE:
		patch_->setFunctionValue(FUNC_FENV,val>>3);
		break;
		case CC_AMPENVSHAPE:
		patch_->setFunctionValue(FUNC_AENV,val>>3);
		break;
		case CC_SEQUENCE:
		patch_->setFunctionValue(FUNC_PATTERN,val>>3);
		break;
		case CC_FILTCUTOFF:
		patch_->setCtrlValue(LOW,CTRL_FILT,val<<1);
		break;
		case CC_FILTRES:
		patch_->setCtrlValue(HIGH,CTRL_FILT,val<<1);
		break;
		case CC_MODPRESET:
		patch_->setFunctionValue(FUNC_LFOTYPE,val>>3);
		break;
		case CC_PITCHLFO:
		patch_->setCtrlValue(HIGH,CTRL_LFO,val<<1);
		break;
		case CC_LFOSPEED:
		patch_->setFunctionValue(FUNC_LFOSPEED,val>>3);
		break;
		case CC_PWM:
		patch_->setCtrlValue(HIGH,CTRL_AMP,val<<1);
		break;
		case CC_GAIN:
		patch_->setCtrlValue(LOW,CTRL_AMP,val<<1);
		break;
		case CC_FILTLFO:
		patch_->setCtrlValue(LOW,CTRL_LFO,val<<1);
		break;
		case CC_PITCHENV:
		patch_->setCtrlValue(HIGH,CTRL_ENV,val<<1);
		break;
		case CC_OSC2FREQ:
		patch_->setCtrlValue(HIGH,CTRL_FX,val<<1);
		break;
		case CC_ALLNOTESOFF:
		midi_->reset();
		break;
	}

}

void MinOdyEngine::midiPitchBendReceived(char bend)
{
	pitchBend_ = bend;
}
void MinOdyEngine::midiClockStartReceived()
{
	masterClock_.reset();
	masterClock_.setTicksPerCycle(MIDI_TICKSPERCYCLE);
	sequencer_->reset();
}
void MinOdyEngine::midiClockStopReceived()
{
	masterClock_.reset();
	setIntBpm(intBpm_);
	sequencer_->reset();
}

//****************************************patch events********************************************
void MinOdyEngine::patchValueChanged(unsigned char func, unsigned char newValue)
{
	switch (func)
	{
		case FUNC_WAVE:
		if((newValue & 0x01)>0)
		{
			oscillator_[0].setWaveform(OdyOscillator::PULSE);
		}
		else
		{
			oscillator_[0].setWaveform(OdyOscillator::SAW);
		}
		if((newValue & 0x02)>0)
		{
			oscillator_[1].setWaveform(OdyOscillator::PULSE);
		}
		else
		{
			oscillator_[1].setWaveform(OdyOscillator::SAW);
		}
		if((newValue & 0x04)>0)
		{
			fxSource_ = FX_RINGMOD;
		}
		else
		{
			fxSource_ = FX_NOISE;
		}
		oscillator_[0].setLevel(pgm_read_byte(&(OSC0_PRESET[newValue])));
		oscillator_[1].setLevel(pgm_read_byte(&(OSC1_PRESET[newValue])));
		noise_.setLevel(pgm_read_byte(&(FX_PRESET[newValue])));
		fxLevel_ = pgm_read_byte(&(FX_PRESET[newValue]));
		break;
		case FUNC_FILT:
		filter_.setType((OdyFilter::FiltType)(newValue % 3));
		break;
		case FUNC_FENV:
		adsrEnvelope_.setAttack(pgm_read_word(&(ENV_A_INC[pgm_read_byte(&(ENV_A_PRESET[newValue]))])));
		adsrEnvelope_.setDecay(pgm_read_word(&(ENV_D_INC[pgm_read_byte(&(ENV_DR_PRESET[newValue]))])));
		adsrEnvelope_.setSustain(pgm_read_word(&(ENV_S_LEVEL[pgm_read_byte(&(ENV_S_PRESET[newValue]))])));
		adsrEnvelope_.setRelease(pgm_read_word(&(ENV_R_INC[pgm_read_byte(&(ENV_DR_PRESET[newValue]))])));
		break;
		case FUNC_AENV:
		arEnvelope_.setAttack(pgm_read_word(&(ENV_A_INC[pgm_read_byte(&(ENV_A_PRESET[newValue]))])));
		arEnvelope_.setDecay(pgm_read_word(&(ENV_D_INC[pgm_read_byte(&(ENV_DR_PRESET[newValue]))])));
		arEnvelope_.setSustain(pgm_read_word(&(ENV_S_LEVEL[pgm_read_byte(&(ENV_S_PRESET[newValue]))])));
		arEnvelope_.setRelease(pgm_read_word(&(ENV_R_INC[pgm_read_byte(&(ENV_DR_PRESET[newValue]))])));
		break;
		case FUNC_LFOTYPE:
		if((newValue & 0x01)>0)
		{
			pitch_[0].setFmASource(OdyPitch::SINE);
		}
		else
		{
			pitch_[0].setFmASource(OdyPitch::SQUARE);
		}
		if((newValue & 0x02)>0)
		{
			pitch_[1].setFmASource(OdyPitch::SINE);
		}
		else
		{
			pitch_[1].setFmASource(OdyPitch::SQUARE);
		}
		if((newValue & 0x04)>0)
		{
			oscillator_[1].setPwmSource(OdyOscillator::LFO);
		}
		else
		{
			oscillator_[1].setPwmSource(OdyOscillator::ENV);
		}
		if((newValue & 0x08)>0)
		{
			filter_.setFmASource(OdyFilter::S_AND_H);
		}
		else
		{
			filter_.setFmASource(OdyFilter::SINE);
		}
		break;
		case FUNC_LFOSPEED:
		lfo_.setFreq(pgm_read_byte(&(LFO_EXP_KNOB[(newValue<<3)])));
		break;
		case FUNC_CRUSHPORTA:
		portamento_[0].setSpeed(pgm_read_word(&(PORTA_SPEED[newValue])));
		portamento_[1].setSpeed(pgm_read_word(&(PORTA_SPEED[newValue])));
		break;
		case FUNC_PATTERN:
		sequencer_->setPattern(newValue);
		break;
		case FUNC_BPM:
		setIntBpm(newValue);
		break;
	}
	if(func==function_)
	{
		base_->engineValueChanged(func,newValue,LOW);
	}
}

void MinOdyEngine::patchCtrlChanged(unsigned char bank, unsigned char ctrl, unsigned char newValue)
{
	switch (ctrl)
	{
		case CTRL_VOLUME:
		if(bank==LOW)
		{
			amplifier_.setAmAmount((newValue>>4)+1);
		}
		else
		{
			bool testNoteOn = false;
			if(midi_->getNoteOn(testNote_)==true)
			{
				testNoteOn = true;
			}
			testNote_ = (newValue>>2) + 24;
			seqBaseNote_ = testNote_;
			if(testNoteOn==true)
			{
				midiNoteOnReceived(testNote_,127);
			}
		}
		break;
		case CTRL_FILT:
		if(bank==LOW)
		{
			filter_.setFc(newValue);
		}
		else
		{
			filter_.setQ(newValue);
		}
		break;
		case CTRL_ENV:
		if(bank==LOW)
		{
			filter_.setFmBAmount(newValue>>4);
		}
		else
		{
			pitch_[0].setFmBAmount(newValue>>4);
			pitch_[1].setFmBAmount(newValue>>4);
		}
		break;
		case CTRL_LFO:
		if(bank==LOW)
		{
			filter_.setFmAAmount(newValue>>4);
		}
		else
		{
			pitch_[0].setFmAAmount(newValue>>4);
			pitch_[1].setFmAAmount(newValue>>4);
		}
		break;
		case CTRL_AMP:
		if(bank==LOW)
		{
			amplifier_.setLevel(newValue>>1);
		}
		else
		{
			oscillator_[1].setPwmAmout(newValue);
		}
		break;
		case CTRL_FX:
		if(bank==LOW)
		{
			pitch_[0].setOffset(pgm_read_byte(&(PITCH_OFFSET[128-(newValue>>1)])));
		}
		else
		{
			pitch_[1].setOffset(pgm_read_byte(&(PITCH_OFFSET[(newValue>>1)+128])));
		}
		break;
	}
}

//**********************sequencer event**************************
void MinOdyEngine::stepseqNoteEvent(unsigned char lastStep, unsigned char newStep)
{
	if(lastStep!=StepSequencer::REST && newStep!=StepSequencer::HOLD)
	{
		releaseNote();
	}
	if(newStep!=StepSequencer::REST && newStep!=StepSequencer::HOLD)
	{
		unsigned char note = (newStep&0x3F)+seqBaseNote_;
		if(note<128)
		{
			triggerNote(note);
		}
	}
	#if SEQUENCE==XMAS
	if(sequencer_->getStep()==15 && sequencer_->getPattern()!=0)
	{
		if(sequencer_->getPattern()==15)
		{
			patch_->setFunctionValue(FUNC_PATTERN,0);
		}
		else
		{
			patch_->setFunctionValue(FUNC_PATTERN,sequencer_->getPattern()+1);
		}
	}
	#endif
}