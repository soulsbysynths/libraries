//MinEngine.cpp  Audio engine for miniAtmegatron
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

#include "MinEngine.h"

extern void writeMemory(const void* data, void* startAddr, size_t size);
extern void readMemory(void* data, const void* startAddr, size_t size);

// default constructor
MinEngine::MinEngine(MinEngineBase* base)
{
	construct(base);
} //MinEngine

// default destructor
MinEngine::~MinEngine()
{
	if(oscillator_!=NULL)
	{
		delete oscillator_;
	}
	if(audio_!=NULL)
	{
		delete audio_;
	}
	if(patch_!=NULL)
	{
		delete patch_;
	}
	if(midi_!=NULL)
	{
		delete midi_;
	}
	if(flanger_!=NULL)
	{
		delete flanger_;
	}
	if(sequencer_!=NULL)
	{
		delete sequencer_;
	}
} //~MinEngine

void MinEngine::construct(MinEngineBase* base)
{
	base_ = base;
	patch_ = new AtmPatch(this);
	oscillator_ = new AtmOscillator(WAVE_LENGTH,patch_->PATCH_SIZE*16);
	audio_ = new AtmAudio(WAVE_LENGTH);
	flanger_ = new Flanger(WAVE_LENGTH);
	setIntBpm(intBpm_);
	midi_ = new Midi(this,SYSEX_PROD_ID);
	sequencer_ = new StepSequencer(this);
	pwm_ = new Pwm(WAVE_LENGTH);
}

void MinEngine::initialize()
{
	unsigned char i,j;
	audio_->initialize();
	midi_->setChannel(MIDI_CHANNEL);
	patch_->readPatch(PATCH_NUM);
	setFunction(FUNC_WAVE);
	setBank(LOW);
}
void MinEngine::initPatch()
{
	patch_->setFunctionValue(FUNC_WAVE,0);
	patch_->setFunctionValue(FUNC_FILT,1);
	patch_->setFunctionValue(FUNC_FENV,1);
	patch_->setFunctionValue(FUNC_AENV,2);
	patch_->setFunctionValue(FUNC_LFOTYPE,0);
	patch_->setFunctionValue(FUNC_LFOSPEED,5);
	patch_->setFunctionValue(FUNC_CRUSHPORTA,0);
	patch_->setFunctionValue(FUNC_PATTERN,1);
	patch_->setFunctionValue(FUNC_BPM,6);
	patch_->setCtrlValue(LOW,CTRL_FILT,255);
	patch_->setCtrlValue(LOW,CTRL_ENV,255);
	patch_->setCtrlValue(LOW,CTRL_LFO,0);
	patch_->setCtrlValue(LOW,CTRL_AMP,0);
	patch_->setCtrlValue(LOW,CTRL_FX,0);
	patch_->setCtrlValue(LOW,CTRL_VOLUME,255);
	patch_->setCtrlValue(HIGH,CTRL_FILT,140);
	patch_->setCtrlValue(HIGH,CTRL_ENV,0);
	patch_->setCtrlValue(HIGH,CTRL_LFO,0);
	patch_->setCtrlValue(HIGH,CTRL_AMP,0);
	patch_->setCtrlValue(HIGH,CTRL_FX,0);
	patch_->setCtrlValue(HIGH,CTRL_VOLUME,60);
}
void MinEngine::poll(unsigned char ticksPassed)
{
	unsigned char i;
	
	if(midi_->getClockRunning()==true)
	{
		masterClock_.refresh(midi_->getClockTicksPassed());
	}
	else
	{
		masterClock_.refresh(ticksPassed);
	}
	lfo_.refresh(masterClock_.getOutput());
	sequencer_->refresh(masterClock_.getOutput());
	ampEnvelope_.refresh(ticksPassed);
	filtEnvelope_.refresh(ticksPassed);

	char lfo = lfo_.getOutput();
	#if EXP_ENVELOPES==1
	char env = filtEnvelope_.getExpOutput();
	#else
	char env = filtEnvelope_.getOutput();
	#endif

	filter_.refresh(audio_->getSampleFreq(),lfo,env);
	portamento_.refresh(ticksPassed);
	
	frequency_ = portamento_.getOutput();
	pitch_.setInput(frequency_);
	pitch_.refresh(lfo,env,pitchBend_);
	audio_->setSampleFreq((unsigned long)pitch_.getOutput() * WAVE_LENGTH);
	
	Wavetable working_buffer_(WAVE_LENGTH);
	oscillator_->copyWavetable(working_buffer_);
	pwm_->processWavetable(working_buffer_,0,lfo);
	filter_.processWavetable(working_buffer_);
	if(ampEnvelope_.getState()==Envelope::IDLE)
	{
		flanger_->clearBuffer();
	}
	else
	{
		flanger_->processWavetable(working_buffer_,0,lfo);
	}
	wavecrusher_.processWavetable(working_buffer_);
	clipdistortion_.processWavetable(working_buffer_);
	#if EXP_ENVELOPES==1
	amplifier_.processWavetable(working_buffer_, ampEnvelope_.getExpOutput(), lfo);
	#else
	amplifier_.processWavetable(working_buffer_, ampEnvelope_.getOutput(), lfo);
	#endif
	audio_->pasteWavetable(working_buffer_);
}

void MinEngine::setFunction(MinEngine::Func newFunc)
{
	bool col;
	function_ = newFunc;
	base_->engineValueChanged((unsigned char)function_,patch_->getFunctionValue(function_),LOW);
}

void MinEngine::setBank(unsigned char newBank)
{
	bank_ = newBank;
	base_->engineBankChanged(bank_);
}

void MinEngine::triggerNote(unsigned char note)
{
	//frequency_ = pgm_read_word(&(midiFreqs[note]));
	portamento_.setInput(pgm_read_word(&(midiFreqs[note])));
	ampEnvelope_.trigger();
	filtEnvelope_.trigger();
}
void MinEngine::releaseNote()
{
	ampEnvelope_.release();
	filtEnvelope_.release();
}
void MinEngine::setIntBpm(unsigned char newBPM)
{
	intBpm_ = newBPM;
	unsigned int b = ((unsigned int)newBPM * 10) + 60;
	masterClock_.setTicksPerCycle(3840000UL/b);
}
//***********************MIDI events********************************************
void MinEngine::midiNoteOnReceived(unsigned char note, unsigned char velocity)
{
	if(noteOn_[note]==false)
	{
		totNotesOnLast_ = totNotesOn_;
		totNotesOn_++;
		noteOn_[note] = true;
		#if NOTE_PRIORITY == NP_LOW
		if(note<noteLowest_)
		{
			noteLowest_ = note;
		}
		#elif NOTE_PRIORITY == NP_HIGH
		if(note>noteHighest_)
		{
			noteHighest_ = note;
		}
		#elif NOTE_PRIORITY == NP_LAST
		noteOrder_[note] = totNotesOn_;
		noteLast_ = note;
		#endif
		if(sequencer_->getPattern()==0)
		{
			#if LEGATO==1
			if(totNotesOn_>1)
			{
				portamento_.setInput(pgm_read_word(&(midiFreqs[note])));
			}
			else
			{
				triggerNote(note);
			}
			#else
			triggerNote(note);
			#endif
		}
	}
}

void MinEngine::midiNoteOffReceived(unsigned char note)
{
	if(noteOn_[note]==true)
	{
		totNotesOnLast_ = totNotesOn_;
		totNotesOn_--;
		noteOn_[note] = false;
		#if NOTE_PRIORITY == NP_LOW
		noteLowest_ = 127;
		for(unsigned char i=0;i<128;++i)
		{
			if(noteOn_[i]==true)
			{
				noteLowest_ = i;
				break;
			}
		}
		#elif NOTE_PRIORITY == NP_HIGH
		if(note==noteHighest_)
		{
			noteHighest_ = 0;
			for(unsigned char i=127;i!=0;--i)
			{
				if(noteOn_[i]==true)
				{
					noteHighest_ = i;
					break;
				}
			}
		}
		#elif NOTE_PRIORITY == NP_LAST
		noteOrder_[note] = 0;
		if(note==noteLast_)
		{
			noteLast_ = 0;
			unsigned char last = 0;
			for(unsigned char i=0;i<128;++i)
			{
				if(noteOrder_[i]>last)
				{
					last = noteOrder_[i];
					noteLast_ = i;
				}
			}
			
		}
		#endif
		if(totNotesOn_==0 && sequencer_->getPattern()==0)
		{
			releaseNote();
		}
		else if(sequencer_->getPattern()==0)
		{
			#if NOTE_PRIORITY == NP_LOW
			portamento_.setInput(pgm_read_word(&(midiFreqs[noteLowest_])));
			#elif NOTE_PRIORITY == NP_HIGH
			portamento_.setInput(pgm_read_word(&(midiFreqs[noteHighest_])));
			#elif NOTE_PRIORITY == NP_LAST
			portamento_.setInput(pgm_read_word(&(midiFreqs[noteLast_])));
			#endif
		}
	}

}
void MinEngine::midiClockStartReceived()
{
	masterClock_.reset();
	masterClock_.setTicksPerCycle(MIDI_TICKSPERCYCLE);
	sequencer_->reset();
}
void MinEngine::midiClockStopReceived()
{
	masterClock_.reset();
	setIntBpm(intBpm_);
	sequencer_->reset();
}
void MinEngine::midiControlChangeReceived(unsigned char anlControl_, unsigned char val)
{
	switch ((MidiCC)anlControl_)
	{
		case CC_PITCHLFO:
		patch_->setCtrlValue(HIGH,CTRL_LFO,val>>1);
		break;
		case CC_FILTERENV:
		patch_->setCtrlValue(LOW,CTRL_ENV,val<<1);
		break;
		case CC_DISTORTION:
		patch_->setCtrlValue(LOW,CTRL_FX,val<<1);
		break;
		case CC_FILTCUTOFF:
		patch_->setCtrlValue(LOW,CTRL_FILT,val<<1);
		break;
		case CC_FILTRES:
		patch_->setCtrlValue(HIGH,CTRL_FILT,val<<1);
		break;
		case CC_LFOCLOCKDIV:
		patch_->setFunctionValue(FUNC_LFOSPEED,val>>3);
		break;
		case CC_PWM:
		patch_->setCtrlValue(HIGH,CTRL_FX,val<<1);
		break;
		case CC_AMPLFO:
		patch_->setCtrlValue(LOW,CTRL_AMP,val<<1);
		break;
		case CC_FILTLFO:
		patch_->setCtrlValue(LOW,CTRL_LFO,val<<1);
		break;
		case CC_PITCHENV:
		patch_->setCtrlValue(HIGH,CTRL_ENV,val<<1);
		break;
		case CC_FLANGE:
		patch_->setCtrlValue(HIGH,CTRL_FX,val<<1);
		break;
		case CC_ALLNOTESOFF:
		midi_->reset();
		break;
	}
}

void MinEngine::midiPitchBendReceived(char bend)
{
	pitchBend_ = bend;
}
//**********************sequencer event**************************
void MinEngine::stepseqNoteEvent(unsigned char lastStep, unsigned char newStep)
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
}

//****************************************patch events********************************************
void MinEngine::patchValueChanged(unsigned char func, unsigned char newValue)
{
	switch (func)
	{
		case FUNC_WAVE:
		oscillator_->setBank(pgm_read_byte(&(osc_preset_bank[newValue])));
		oscillator_->setTable(pgm_read_byte(&(osc_preset_table[newValue])));
		break;
		case FUNC_FILT:
		filter_.setType((BiquadFilter::FiltType) newValue);
		break;
		case FUNC_FENV:
		filtEnvelope_.setAttack(pgm_read_word(&(envADR_inc[pgm_read_byte(&(envA_preset[newValue]))])));
		filtEnvelope_.setDecay(pgm_read_word(&(envADR_inc[pgm_read_byte(&(envDR_preset[newValue]))])));
		filtEnvelope_.setSustain(pgm_read_word(&(envS_level[pgm_read_byte(&(envS_preset[newValue]))])));
		filtEnvelope_.setRelease(pgm_read_word(&(envADR_inc[pgm_read_byte(&(envDR_preset[newValue]))])));
		break;
		case FUNC_AENV:
		ampEnvelope_.setAttack(pgm_read_word(&(envADR_inc[pgm_read_byte(&(envA_preset[newValue]))])));
		ampEnvelope_.setDecay(pgm_read_word(&(envADR_inc[pgm_read_byte(&(envDR_preset[newValue]))])));
		ampEnvelope_.setSustain(pgm_read_word(&(envS_level[pgm_read_byte(&(envS_preset[newValue]))])));
		ampEnvelope_.setRelease(pgm_read_word(&(envADR_inc[pgm_read_byte(&(envDR_preset[newValue]))])));
		break;
		case FUNC_LFOTYPE:
		lfo_.setTable(newValue);
		break;
		case FUNC_LFOSPEED:
		lfo_.setDivision(newValue);
		break;
		case FUNC_CRUSHPORTA:
		if(newValue<8)
		{
			portamento_.setSpeed(pgm_read_word(&(porta_speed[0])));
			wavecrusher_.setType(newValue<<1);
		}
		else
		{
			portamento_.setSpeed(pgm_read_word(&(porta_speed[((newValue-8)<<1)+1])));
			wavecrusher_.setType(0);
		}
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

void MinEngine::patchCtrlChanged(unsigned char bank, unsigned char anlControl_, unsigned char newValue)
{
	switch (anlControl_)
	{
		case CTRL_VOLUME:
		if(bank==LOW)
		{
			amplifier_.setEnvAmount(newValue);
		}
		else
		{
			bool testNoteOn = false;
			if(noteOn_[testNote_]==true)
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
			filter_.setEnvAmount(newValue);
		}
		else
		{
			pitch_.setEnvAmount(newValue);
		}
		break;
		case CTRL_LFO:
		if(bank==LOW)
		{
			filter_.setLfoAmount(newValue);
		}
		else
		{
			pitch_.setLfoAmount(newValue);
		}
		break;
		case CTRL_AMP:
		if(bank==LOW)
		{
			amplifier_.setLfoAmount(newValue);
		}
		else
		{
			pwm_->setLfoAmount(newValue);
		}
		break;
		case CTRL_FX:
		if(bank==LOW)
		{
			clipdistortion_.setDistLevel(newValue);
		}
		else
		{
			flanger_->setLfoAmount(newValue);
		}
		break;
	}
}