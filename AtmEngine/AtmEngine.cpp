//AtmEngine.cpp  Audio engine for Atmegatron
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

#include "AtmEngine.h"

extern void writeMemory(const void* data, void* startAddr, size_t size);
extern void readMemory(void* data, const void* startAddr, size_t size);

// default constructor
AtmEngine::AtmEngine(AtmEngineBase* base)
{
	construct(base);
} //AtmEngine

// default destructor
AtmEngine::~AtmEngine()
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
	if(arpeggiator_!=NULL)
	{
		delete arpeggiator_;
	}
} //~AtmEngine

void AtmEngine::construct(AtmEngineBase* base)
{
	base_ = base;
	patch_ = new AtmPatch(this);
	oscillator_ = new AtmOscillator(WAVE_LENGTH,patch_->PATCH_SIZE*16);
	audio_ = new AtmAudio(WAVE_LENGTH);
	flanger_ = new Flanger(WAVE_LENGTH);
	masterClock_.setTicksPerCycle(BPM_TICKS);
	midi_ = new Midi(this,SYSEX_PROD_ID);
	arpeggiator_ = new Arpeggiator(this,midi_->getNoteOnPtr());
	pwm_ = new Pwm(WAVE_LENGTH);
}

void AtmEngine::initialize()
{
	unsigned char i,j;
	
	audio_->initialize();
	//load vanilla
	for(i=0;i<6;++i)
	{
		for(j=0;j<2;++j)
		{
			patchCtrlChanged(j,i,patch_->getCtrlValue(j,i));
		}
	}
	for(i=0;i<16;++i)
	{
		patchValueChanged(i,patch_->getFunctionValue(i));
		patchOptionChanged(i,patch_->getOptionValue(i));
	}
	
	setFunction(FUNC_WAVE);
	setBank(LOW);
}
void AtmEngine::poll(unsigned char ticksPassed)
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
	arpeggiator_->refresh(masterClock_.getOutput());
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
	audio_->setSampleFreq((unsigned long)pitch_.getOutput() * (WAVE_LENGTH >> 2));
	
	Wavetable working_buffer_(WAVE_LENGTH);
	oscillator_->copyWavetable(working_buffer_);
	pwm_->processWavetable(working_buffer_,0,lfo);
	if(patch_->getOptionValue(FUNC_BITCRUSH)==true)
	{
		wavecrusher_.processWavetable(working_buffer_);
	}
	filter_.processWavetable(working_buffer_);
	
	#ifdef EXP_ENVELOPES==1
	amplifier_.processWavetable(working_buffer_, ampEnvelope_.getExpOutput(), lfo);
	#else
	amplifier_.processWavetable(working_buffer_, ampEnvelope_.getOutput(), lfo);
	#endif
	
	if(ampEnvelope_.getState()==Envelope::IDLE)
	{
		flanger_->clearBuffer();
	}
	else
	{
		flanger_->processWavetable(working_buffer_,0,lfo);
	}
	if(patch_->getOptionValue(FUNC_BITCRUSH)==false)
	{
		wavecrusher_.processWavetable(working_buffer_);
	}
	clipdistortion_.processWavetable(working_buffer_);
	audio_->pasteWavetable(working_buffer_);

}

void AtmEngine::setFunction(AtmEngine::Func newFunc)
{
	function_ = newFunc;
	base_->engineFunctionChanged((unsigned char)function_,patch_->getFunctionValue(function_),patch_->getOptionValue(function_));
}

void AtmEngine::setBank(unsigned char newBank)
{
	bank_ = newBank;
	base_->engineBankChanged(bank_);
}

void AtmEngine::triggerNote(unsigned char note)
{
	//frequency_ = pgm_read_word(&(MIDI_FREQS[note]));
	portamento_.setInput(pgm_read_word(&(MIDI_FREQS[note])));
	ampEnvelope_.trigger();
	filtEnvelope_.trigger();
}
void AtmEngine::releaseNote()
{
	ampEnvelope_.release();
	filtEnvelope_.release();
}

void AtmEngine::tieOptions(Func minFunc, Func maxFunc, bool newOpt)
{
	for(unsigned char i=minFunc;i<=maxFunc;++i)
	{
		if(patch_->getOptionValue(i)!=newOpt)
		{
			patch_->setOptionValue(i,newOpt);
		}
	}
}
void AtmEngine::tieControls(unsigned char bank, unsigned char ctrl)
{
	if(patch_->getCtrlValue(HIGH-bank,ctrl)!=patch_->getCtrlValue(bank,ctrl))
	{
		patch_->setCtrlValue(HIGH-bank,ctrl,patch_->getCtrlValue(bank,ctrl));
	}
}
void AtmEngine::writeSysexPatch(unsigned char patchNum)
{
	unsigned char sys_mess[AtmPatch::SYSEX_SIZE + 1];
	AtmPatch patch;
	patch.readPatch(patchNum);
	sys_mess[0] = SYSEX_PATCH;
	for(unsigned char i=0;i<AtmPatch::SYSEX_SIZE;++i)
	{
		sys_mess[i+1] = patch.getSysexByte(i);
	}
	midi_->writeSysex(sys_mess,sizeof(sys_mess));
}
void AtmEngine::writeSysexUserWave(unsigned char waveNum)
{
	unsigned char sys_mess[32 + 1];
	unsigned char samp;
	AtmOscillator osc(WAVE_LENGTH,oscillator_->getUserWaveStartAddress());
	osc.readUserWave(waveNum);
	sys_mess[0] = SYSEX_WAVE;
	for(unsigned char i=0;i<32;++i)
	{
		samp = osc.getUserWavetableSample(i) + 128;
		samp >>= 1;
		sys_mess[i+1] = samp;
	}
	midi_->writeSysex(sys_mess,sizeof(sys_mess));
}
void AtmEngine::writeSysexMemDump()
{
	unsigned char sys_mess[1];
	unsigned char i;
	sys_mess[0] = SYSEX_MEM;
	midi_->writeSysex(sys_mess,sizeof(sys_mess));
}
void AtmEngine::writeSysexCallPatch(unsigned char patchNum)
{
	unsigned char sys_mess[2] = {0};
	sys_mess[0] = SYSEX_CALLPATCH;
	sys_mess[1] = patchNum;
	midi_->writeSysex(sys_mess,sizeof(sys_mess));
}
void AtmEngine::writeSysexCallUserWave(unsigned char waveNum)
{
	unsigned char sys_mess[2] = {0};
	sys_mess[0] = SYSEX_CALLWAVE;
	sys_mess[1] = waveNum;
	midi_->writeSysex(sys_mess,sizeof(sys_mess));
}
void AtmEngine::refreshSysex()
{
	if (sysexComplete_==true)
	{
		sysexComplete_ = false;
		switch(sysexMesstype_)
		{
			case SYSEX_PATCH:
			if(sysexMemdumping_==true)
			{
				patch_->writePatch(sysexMemdumpnum_);
				sysexMemdumpnum_++;
				if(sysexMemdumpnum_<16)
				{
					writeSysexCallPatch(sysexMemdumpnum_);
				}
				else
				{
					sysexMemdumpnum_ = 0;  //reset counter for user waves
					writeSysexCallUserWave(sysexMemdumpnum_);
				}
			}
			else
			{
				base_->engineSysexComplete();
			}
			break;
			case SYSEX_WAVE:
			if(sysexMemdumping_==true)
			{
				oscillator_->writeUserWave(sysexMemdumpnum_);
				sysexMemdumpnum_++;
				if(sysexMemdumpnum_<16)
				{
					writeSysexCallUserWave(sysexMemdumpnum_);
				}
				else
				{
					sysexMemdumping_ = false;
					base_->engineSysexComplete();
				}
			}
			else
			{
				oscillator_->writeUserWave(patch_->getFunctionValue(FUNC_WAVE));
				base_->engineSysexComplete();
			}
			break;
			case SYSEX_CALLPATCH:
			writeSysexPatch(sysexMemdumpnum_);
			break;
			case SYSEX_CALLWAVE:
			writeSysexUserWave(sysexMemdumpnum_);
			break;
			case SYSEX_MEM:
			sysexMemdumpnum_ = 0;
			sysexMemdumping_ = true;
			writeSysexCallPatch(sysexMemdumpnum_);
			break;
		}
	}
}
//***********************MIDI events********************************************
void AtmEngine::midiNoteOnReceived(unsigned char note, unsigned char velocity)
{
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
	noteOrder_[note] = midi_->getTotNotesOn();
	noteLast_ = note;
	#endif
	if(arpeggiator_->getType()==0)
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
		arpeggiator_->buildNoteOrder();
	}
	if (arpeggiator_->getType()>0 && totNotesOnLast_==0 && midi_->getTotNotesOn()==1 && midi_->getClockRunning()==false)
	{
		masterClock_.reset();
		arpeggiator_->reset();
	}
	totNotesOnLast_ = midi_->getTotNotesOn();
}

void AtmEngine::midiNoteOffReceived(unsigned char note)
{
	#if NOTE_PRIORITY == NP_LOW
	noteLowest_ = 127;
	for(unsigned char i=0;i<128;++i)
	{
		if(midi_->getNoteOn(i)==true)
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
			if(midi_->getNoteOn(i)==true)
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
	if(midi_->getTotNotesOn()==0 && arpeggiator_->getType()==0)
	{
		releaseNote();
	}
	else if(arpeggiator_->getType()==0)
	{
		#if NOTE_PRIORITY == NP_LOW
		portamento_.setInput(pgm_read_word(&(MIDI_FREQS[noteLowest_])));
		#elif NOTE_PRIORITY == NP_HIGH
		portamento_.setInput(pgm_read_word(&(MIDI_FREQS[noteHighest_])));
		#elif NOTE_PRIORITY == NP_LAST
		portamento_.setInput(pgm_read_word(&(MIDI_FREQS[noteLast_])));
		#endif
	}
	else
	{
		arpeggiator_->buildNoteOrder();
	}
	totNotesOnLast_ = midi_->getTotNotesOn();
}

void AtmEngine::midiClockStartReceived()
{
	masterClock_.reset();
	masterClock_.setTicksPerCycle(MIDI_TICKSPERCYCLE);
	arpeggiator_->reset();
}
void AtmEngine::midiClockStopReceived()
{
	masterClock_.reset();
	masterClock_.setTicksPerCycle(BPM_TICKS);
	arpeggiator_->reset();
}
void AtmEngine::midiControlChangeReceived(unsigned char cc, unsigned char val)
{
	switch ((MidiCC)cc)
	{
		case CC_PITCHLFO:
		patch_->setCtrlValue(HIGH,CTRL_LFO,val>>1);
		break;
		case CC_PORTAMENTO:
		patch_->setFunctionValue(FUNC_PORTA,val>>3);
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
		case CC_AMPENVR:
		patch_->setFunctionValue(FUNC_AENVR,val>>3);
		break;
		case CC_AMPENVA:
		patch_->setFunctionValue(FUNC_AENVA,val>>3);
		break;
		case CC_FILTRES:
		patch_->setCtrlValue(LOW,CTRL_Q,val<<1);
		break;
		case CC_AMPENVD:
		patch_->setFunctionValue(FUNC_AENVD,val>>3);
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

void AtmEngine::midiSysexDataReceived(unsigned char index, unsigned char data)
{
	int samp = 0;
	if(index==0)
	{
		sysexMesstype_ = data;
	}
	else
	{
		switch(sysexMesstype_)
		{
			case SYSEX_PATCH:
			patch_->setSysexByte(index-1,data);
			break;
			case SYSEX_WAVE:
			samp = (int)data;
			samp *= 2;  //convert from -63 to +63 to -127 to + 127;
			samp -= 126;
			oscillator_->setUserWavetableSample(index-1,(char)samp);
			break;
			case SYSEX_CALLPATCH:
			case SYSEX_CALLWAVE:
			sysexMemdumpnum_ = data;
			break;
		}
	}
}
void AtmEngine::midiSysexStopReceived()
{
	sysexComplete_ = true;
}
void AtmEngine::midiSysexWrite(unsigned char data)
{
	base_->engineMidiTransmit(data);
}

void AtmEngine::midiPitchBendReceived(char bend)
{
	pitchBend_ = bend;
}
//*********************Arpeggiator events***************************
void AtmEngine::arpeggiatorNoteEvent(unsigned char lastNote, unsigned char newNote)
{
	if(lastNote>0)
	{
		releaseNote();
	}
	if(newNote>0)
	{
		triggerNote(newNote);
	}
}


//****************************************patch events********************************************
void AtmEngine::patchValueChanged(unsigned char func, unsigned char newValue)
{
	switch (func)
	{
		case FUNC_WAVE:
		oscillator_->setTable(newValue);
		break;
		case FUNC_FILT:
		filter_.setType((BiquadFilter::FiltType) newValue);
		break;
		case FUNC_FENVA:
		filtEnvelope_.setAttack(pgm_read_word(&(ENV_ADR_INC[newValue])));
		break;
		case FUNC_FENVDR:
		filtEnvelope_.setDecay(pgm_read_word(&(ENV_ADR_INC[newValue])));
		filtEnvelope_.setRelease(pgm_read_word(&(ENV_ADR_INC[newValue])));
		break;
		case FUNC_FENVS:
		filtEnvelope_.setSustain(pgm_read_word(&(ENV_S_LEVEL[newValue])));
		break;
		case FUNC_AENVA:
		ampEnvelope_.setAttack(pgm_read_word(&(ENV_ADR_INC[newValue])));
		break;
		case FUNC_AENVD:
		ampEnvelope_.setDecay(pgm_read_word(&(ENV_ADR_INC[newValue])));
		break;
		case FUNC_AENVS:
		ampEnvelope_.setSustain(pgm_read_word(&(ENV_S_LEVEL[newValue])));
		break;
		case FUNC_AENVR:
		ampEnvelope_.setRelease(pgm_read_word(&(ENV_ADR_INC[newValue])));
		break;
		case FUNC_LFOTYPE:
		lfo_.setTable(newValue);
		break;
		case FUNC_LFOSPEED:
		lfo_.setDivision(newValue);
		break;
		case FUNC_ARPTYPE:
		arpeggiator_->setType(newValue);
		break;
		case FUNC_ARPSPEED:
		arpeggiator_->setDivision(newValue);
		break;
		case FUNC_PORTA:
		portamento_.setSpeed(pgm_read_word(&(PORTA_SPEED[newValue])));
		break;
		case FUNC_BITCRUSH:
		wavecrusher_.setType(newValue);
		break;
	}
	if(func==function_)
	{
		base_->engineFunctionChanged(func,newValue,patch_->getOptionValue(func));
	}
}

void AtmEngine::patchOptionChanged(unsigned char func, bool newOpt)
{
	unsigned char i;
	switch (func)
	{
		case FUNC_WAVE:
		oscillator_->setBank((unsigned char)newOpt);
		break;
		case FUNC_FILT:
		filter_.setGainAdj(newOpt);
		break;
		case FUNC_FENVA:
		case FUNC_FENVDR:
		case FUNC_FENVS:
		filtEnvelope_.setInvert(newOpt);
		tieOptions(FUNC_FENVA,FUNC_FENVS,newOpt);
		break;
		case FUNC_AENVD:
		break;
		case FUNC_AENVS:
		oscillator_->setUserMode(newOpt);
		break;
		case FUNC_LFOTYPE:
		case FUNC_LFOSPEED:
		lfo_.setInvert(newOpt);
		tieOptions(FUNC_LFOTYPE,FUNC_LFOSPEED,newOpt);
		break;
		case FUNC_ARPTYPE:
		case FUNC_ARPSPEED:
		arpeggiator_->setPingPong(newOpt);
		tieOptions(FUNC_ARPTYPE,FUNC_ARPSPEED,newOpt);
		break;
		case FUNC_PORTA:
		portamento_.setProportional(newOpt);
		break;
	}
}

void AtmEngine::patchCtrlChanged(unsigned char bank, unsigned char ctrl, unsigned char newValue)
{
	switch (ctrl)
	{
		case CTRL_FILT:
		filter_.setFc(newValue);
		tieControls(bank,ctrl);
		break;
		case CTRL_Q:
		filter_.setQ(newValue);
		tieControls(bank,ctrl);
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