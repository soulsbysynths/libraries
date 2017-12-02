//MulAtmegaAtmEngine.cpp  Audio engine for Atmegatron
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

#include "MulAtmegaAtmEngine.h"

extern void writeMemory(const void* data, void* startAddr, size_t size);
extern void readMemory(void* data, const void* startAddr, size_t size);

// default constructor
MulAtmegaAtmEngine::MulAtmegaAtmEngine(MulAtmegaAtmEngineBase* base)
{
	construct(base);
} //MulAtmegaAtmEngine

// default destructor
MulAtmegaAtmEngine::~MulAtmegaAtmEngine()
{
	if(oscillator_!=NULL)
	{
		delete oscillator_;
	}
	if(patch_!=NULL)
	{
		delete patch_;
	}
	if(midi_!=NULL)
	{
		delete midi_;
	}
	if(arpeggiator_!=NULL)
	{
		delete arpeggiator_;
	}
} //~MulAtmegaAtmEngine

void MulAtmegaAtmEngine::construct(MulAtmegaAtmEngineBase* base)
{
	base_ = base;
	patch_ = new MulAtmegaAtmPatch(this);
	oscillator_ = new AtmOscillator(WAVE_LENGTH,patch_->PATCH_SIZE*16);
	masterClock_.setTicksPerCycle(BPM_TICKS);
	midi_ = new Midi(this,SYSEX_PROD_ID);
	arpeggiator_ = new Arpeggiator(this,midi_->getNoteOnPtr());
}

void MulAtmegaAtmEngine::initialize()
{
	unsigned char i,j,k;
	
	//load vanilla
	//for(i=0;i<VOICES;++i)
	//{
		//for(j=0;j<2;++j)
		//{
			//for (k=0;k<8;++k)
			//{
				//patchCtrlChanged(i,j,k,patch_->getCtrlValue(i,j,k));
			//}
		//}
		//for(j=0;j<16;++j)
		//{
			//patchValueChanged(i,j,patch_->getFunctionValue(i,j));
			//patchOptionChanged(i,j,patch_->getOptionValue(i,j));
		//}
	//}
			//for(j=0;j<2;++j)
			//{
				//for (k=0;k<8;++k)
				//{
					//patchCtrlChanged(0,j,k,patch_->getCtrlValue(0,j,k));
				//}
			//}
			//for(j=0;j<16;++j)
			//{
				//patchValueChanged(0,j,patch_->getFunctionValue(0,j));
				//patchOptionChanged(0,j,patch_->getOptionValue(0,j));
			//}
	setVoice(0);
	setFunction(FUNC_WAVE);
	setBank(LOW);
}
void MulAtmegaAtmEngine::poll(unsigned char ticksPassed)
{
	unsigned char i;


	//static unsigned int tickNote = 0;
	//static unsigned char note = 48;
	//static bool trigger = true;
	//static unsigned char block = 0;
	//tickNote += ticksPassed;
	//if(tickNote >= 500)
	//{
	//tickNote = 0;
	//if(trigger)
	//{
	//
	//note++;
	//if(note>72)
	//{
	//note = 48;
	//}
	//triggerNote(note);
	//}
	//else
	//{
	//releaseNote();
	//
	//
	//
	////block++;
	////if(block>7)
	////{
	////txWave(0,false,0,block);
	////block = 0;
	////}
	//}
	//trigger = !trigger;
	//
	//}

	if(midi_->getClockRunning()==true)
	{
		masterClock_.refresh(midi_->getClockTicksPassed());
	}
	else
	{
		masterClock_.refresh(ticksPassed);
	}

	arpeggiator_->refresh(masterClock_.getOutput());

}
void MulAtmegaAtmEngine::setVoice(unsigned char newVoice)
{
	voice_ = newVoice;
	base_->engineVoiceChanged(voice_);
	base_->engineValueChanged(patch_->getFunctionValue(voice_,function_));
	base_->engineOptionChanged(patch_->getOptionValue(voice_,function_));

}
void MulAtmegaAtmEngine::setFunction(MulAtmegaAtmEngine::Func newFunc)
{
	function_ = newFunc;
	base_->engineFunctionChanged((unsigned char)function_);
		base_->engineValueChanged(patch_->getFunctionValue(voice_,function_));
		base_->engineOptionChanged(patch_->getOptionValue(voice_,function_));
}

void MulAtmegaAtmEngine::setBank(unsigned char newBank)
{
	bank_ = newBank;
	base_->engineBankChanged(bank_);
}

void MulAtmegaAtmEngine::tieOptions(Func minFunc, Func maxFunc, bool newOpt)
{
	for(unsigned char i=minFunc;i<=maxFunc;++i)
	{
		if(patch_->getOptionValue(voice_,i)!=newOpt)
		{
			patch_->setOptionValue(voice_,i,newOpt);
		}
	}
}
void MulAtmegaAtmEngine::tieControls(unsigned char bank, unsigned char ctrl)
{
	if(patch_->getCtrlValue(voice_,HIGH-bank,ctrl)!=patch_->getCtrlValue(voice_,bank,ctrl))
	{
		patch_->setCtrlValue(voice_,HIGH-bank,ctrl,patch_->getCtrlValue(voice_,bank,ctrl));
	}
}
void MulAtmegaAtmEngine::triggerNote(unsigned char note)
{
	unsigned char i;
	if(mono_==true)
	{
		txTriggerNote(VOICES_ALL,note,false);
	}
	else
	{
		//try and find an empty osc
		if(midi_->getTotNotesOn()<=VOICES)// || arpeggiator_->getType()>0)
		{
			curOsc_++;
			if(curOsc_>=VOICES)
			{
				curOsc_ = 0;
			}
			for(i=0;i<VOICES;++i)
			{
				if(oscNoteOrder_[curOsc_]==0)
				{
					break;
				}
				else
				{
					curOsc_++;
					if(curOsc_>=VOICES)
					{
						curOsc_ = 0;
					}
				}
			}
		}
		else
		{
			unsigned char fst = 128;
			for(i=0;i<VOICES;++i)
			{
				if(oscNoteOrder_[i]<fst && oscNoteOrder_[i]>0)
				{
					fst = oscNoteOrder_[i];
					curOsc_ = i;
				}
			}
		}

		//but still trigger even if you can't
		oscNotes_[curOsc_] = note;
		oscNoteOrder_[curOsc_] = midi_->getTotNotesOn();
		txTriggerNote(curOsc_+1,note,false);  //curosc is from 0 to VOICES, so +1
	}
}

void MulAtmegaAtmEngine::releaseNote(unsigned char note)
{
	unsigned char i,j;
	if(mono_==true)
	{
		if(midi_->getTotNotesOn()<=0)
		{
			for(i=0;i<VOICES;++i)
			{
				oscNotes_[i] = 0;
				oscNoteOrder_[i] = 0;		
			}
			txReleaseNote(VOICES_ALL);
		}
	}
	else
	{
		for (i=0; i<VOICES; ++i)
		{
			if (oscNotes_[i]==note)
			{
				oscNotes_[i] = 0;
				oscNoteOrder_[i] = 0;
				if(midi_->getTotNotesOn()<VOICES)// || arpeggiator_->getType()>0)
				{
					txReleaseNote(i+1);	//curosc is from 0 to VOICES, so +1
				}
				else
				{
					for(j=1;j<128;++j)
					{
						if(midi_->getNoteOn(j)==true && j!=oscNotes_[0] && j!=oscNotes_[1] && j!=oscNotes_[2] && j!=oscNotes_[3] && j!=oscNotes_[4] && j!=oscNotes_[5])
						{
							oscNotes_[i] = j;
							oscNoteOrder_[i] = 1;
							txTriggerNote(i+1,oscNotes_[i],true);		//curosc is from 0 to VOICES, so +1
							break;
						}
					}
				}
				break;
			}
		}
	}
}
void MulAtmegaAtmEngine::txTriggerNote(unsigned char voice, unsigned char note, bool jumpSustain)
{
	unsigned char txMsg[4];
	txMsg[0] = MESS_TRIGGER;
	txMsg[1] = pgm_read_word(&(MIDI_FREQS[note])) >> 8;
	txMsg[2] = pgm_read_word(&(MIDI_FREQS[note])) & 0xFF;
	txMsg[3] = (unsigned char)jumpSustain;
	base_->engineSlaveTransmit(voice,txMsg,sizeof(txMsg));
	base_->engineVoiceTriggered(voice,true);
}
void MulAtmegaAtmEngine::txReleaseNote(unsigned char voice)
{
	unsigned char txMsg[2];
	txMsg[0] = MESS_RELEASE;
	txMsg[1] = 0;  //must have 1 byte of data
	base_->engineSlaveTransmit(voice,txMsg,sizeof(txMsg));
	base_->engineVoiceTriggered(voice,false);
}
void MulAtmegaAtmEngine::txWave(unsigned char voice, bool bank, unsigned char table)
{
	unsigned char i,j;
	
	unsigned char samp = 0;
	const unsigned char SAMPS_PER_BLOCK = 8;
	unsigned char data[SAMPS_PER_BLOCK + 2];
	const unsigned char blocks = WAVE_LENGTH / SAMPS_PER_BLOCK;
	
	oscillator_->setBank(bank);
	oscillator_->setTable(table);
	data[0] = MESS_WAVE;
	for(i=0;i<blocks;++i)
	{
		data[1] = samp;
		for(j=0;j<SAMPS_PER_BLOCK;++j)
		{
			data[j+2] = static_cast<unsigned char>(oscillator_->getSample(samp));
			samp++;
		}
		base_->engineSlaveTransmit(voice,data,sizeof(data));
	}
}

void MulAtmegaAtmEngine::txOsccalib(unsigned char voice, bool up)
{
	unsigned char txMsg[2];
	txMsg[0] = MESS_OSCCAL;
	txMsg[1] = (unsigned char)up;  
	base_->engineSlaveTransmit(voice,txMsg,sizeof(txMsg));
}

void MulAtmegaAtmEngine::txIntMess(unsigned char voice, unsigned char mess, int val)
{
	unsigned char txMsg[3];
	txMsg[0] = mess;
	txMsg[1] = (unsigned int)val >> 8;
	txMsg[2] = (unsigned int)val & 0xFF;
	base_->engineSlaveTransmit(voice,txMsg,sizeof(txMsg));
}

void MulAtmegaAtmEngine::txUintMess(unsigned char voice, unsigned char mess, unsigned int val)
{
	unsigned char txMsg[3];
	txMsg[0] = mess;
	txMsg[1] = val >> 8;
	txMsg[2] = val & 0xFF;
	base_->engineSlaveTransmit(voice,txMsg,sizeof(txMsg));
}

//***********************MIDI events********************************************
void MulAtmegaAtmEngine::midiNoteOnReceived(unsigned char note, unsigned char velocity)
{
	triggerNote(note);

	//#if NOTE_PRIORITY == NP_LOW
	//if(note<noteLowest_)
	//{
	//noteLowest_ = note;
	//}
	//#elif NOTE_PRIORITY == NP_HIGH
	//if(note>noteHighest_)
	//{
	//noteHighest_ = note;
	//}
	//#elif NOTE_PRIORITY == NP_LAST
	//noteOrder_[note] = midi_->getTotNotesOn();
	//noteLast_ = note;
	//#endif
	//
	//if(arpeggiator_->getType()==0)
	//{
	//#if LEGATO==1
	//if(midi_->getTotNotesOn()>1)
	//{
	////portamento_.setInput(pgm_read_word(&(MIDI_FREQS[note])));
	//}
	//else
	//{
	//triggerNote(note);
	//}
	//#else
	//triggerNote(note);
	//#endif
	//}
	//else
	//{
	//arpeggiator_->buildNoteOrder();
	//}
	//if (arpeggiator_->getType()>0 && totNotesOnLast_==0 && midi_->getTotNotesOn()==1 && midi_->getClockRunning()==false)
	//{
	//masterClock_.reset();
	//arpeggiator_->reset();
	//}
	totNotesOnLast_ = midi_->getTotNotesOn();
}

void MulAtmegaAtmEngine::midiNoteOffReceived(unsigned char note)
{
	releaseNote(note);
	//#if NOTE_PRIORITY == NP_LOW
	//noteLowest_ = 127;
	//for(unsigned char i=0;i<128;++i)
	//{
	//if(midi_->getNoteOn(i)==true)
	//{
	//noteLowest_ = i;
	//break;
	//}
	//}
	//#elif NOTE_PRIORITY == NP_HIGH
	//if(note==noteHighest_)
	//{
	//noteHighest_ = 0;
	//for(unsigned char i=127;i!=0;--i)
	//{
	//if(midi_->getNoteOn(i)==true)
	//{
	//noteHighest_ = i;
	//break;
	//}
	//}
	//}
	//#elif NOTE_PRIORITY == NP_LAST
	//noteOrder_[note] = 0;
	//if(note==noteLast_)
	//{
	//noteLast_ = 0;
	//unsigned char last = 0;
	//for(unsigned char i=0;i<128;++i)
	//{
	//if(noteOrder_[i]>last)
	//{
	//last = noteOrder_[i];
	//noteLast_ = i;
	//}
	//}
	//
	//}
	//#endif
	//if(midi_->getTotNotesOn()==0 && arpeggiator_->getType()==0)
	//{
	//releaseNote();
	//}
	//else if(arpeggiator_->getType()==0)
	//{
	//#if NOTE_PRIORITY == NP_LOW
	////portamento_.setInput(pgm_read_word(&(MIDI_FREQS[noteLowest_])));
	//#elif NOTE_PRIORITY == NP_HIGH
	////portamento_.setInput(pgm_read_word(&(MIDI_FREQS[noteHighest_])));
	//#elif NOTE_PRIORITY == NP_LAST
	////portamento_.setInput(pgm_read_word(&(MIDI_FREQS[noteLast_])));
	//#endif
	//}
	//else
	//{
	//arpeggiator_->buildNoteOrder();
	//}
	totNotesOnLast_ = midi_->getTotNotesOn();
}

void MulAtmegaAtmEngine::midiClockStartReceived()
{
	masterClock_.reset();
	masterClock_.setTicksPerCycle(MIDI_TICKSPERCYCLE);
	arpeggiator_->reset();
}
void MulAtmegaAtmEngine::midiClockStopReceived()
{
	masterClock_.reset();
	masterClock_.setTicksPerCycle(BPM_TICKS);
	arpeggiator_->reset();
}
void MulAtmegaAtmEngine::midiControlChangeReceived(unsigned char cc, unsigned char val)
{
	//switch ((MidiCC)cc)
	//{
	//case CC_PITCHLFO:
	//patch_->setCtrlValue(HIGH,CTRL_LFO,val>>1);
	//break;
	//case CC_PORTAMENTO:
	//patch_->setFunctionValue(FUNC_PORTA,val>>3);
	//break;
	//case CC_FILTERENV:
	//patch_->setCtrlValue(LOW,CTRL_ENV,val<<1);
	//break;
	//case CC_DISTORTION:
	//patch_->setCtrlValue(LOW,CTRL_FX,val<<1);
	//break;
	//case CC_FILTCUTOFF:
	//patch_->setCtrlValue(LOW,CTRL_FILT,val<<1);
	//break;
	//case CC_AMPENVR:
	//patch_->setFunctionValue(FUNC_AENVR,val>>3);
	//break;
	//case CC_AMPENVA:
	//patch_->setFunctionValue(FUNC_AENVA,val>>3);
	//break;
	//case CC_FILTRES:
	//patch_->setCtrlValue(LOW,CTRL_Q,val<<1);
	//break;
	//case CC_AMPENVD:
	//patch_->setFunctionValue(FUNC_AENVD,val>>3);
	//break;
	//case CC_LFOCLOCKDIV:
	//patch_->setFunctionValue(FUNC_LFOSPEED,val>>3);
	//break;
	//case CC_PWM:
	//patch_->setCtrlValue(HIGH,CTRL_FX,val<<1);
	//break;
	//case CC_AMPLFO:
	//patch_->setCtrlValue(LOW,CTRL_AMP,val<<1);
	//break;
	//case CC_FILTLFO:
	//patch_->setCtrlValue(LOW,CTRL_LFO,val<<1);
	//break;
	//case CC_PITCHENV:
	//patch_->setCtrlValue(HIGH,CTRL_ENV,val<<1);
	//break;
	//case CC_FLANGE:
	//patch_->setCtrlValue(HIGH,CTRL_FX,val<<1);
	//break;
	//case CC_ALLNOTESOFF:
	//midi_->reset();
	//break;
	//}
}



void MulAtmegaAtmEngine::midiPitchBendReceived(char bend)
{
	pitchBend_ = bend;
	base_->engineMidiTransmit(bend+64);
}
//*********************Arpeggiator events***************************
void MulAtmegaAtmEngine::arpeggiatorNoteEvent(unsigned char lastNote, unsigned char newNote)
{
	//if(lastNote>0)
	//{
	//releaseNote();
	//}
	//if(newNote>0)
	//{
	//triggerNote(newNote);
	//}
}

//****************************************patch events********************************************
void MulAtmegaAtmEngine::patchValueChanged(unsigned char voice, unsigned char func, unsigned char newValue)
{
	unsigned char i,j;
	switch (func)
	{
		case FUNC_WAVE:
		txWave(voice,patch_->getOptionValue(voice,FUNC_WAVE),newValue);
		//txWave(0,false,0);
		//txWave(0,false,0,newValue&0x03);
		//oscillator_->setTable(newValue);
		break;
		case FUNC_FILT:
		//filter_.setType((BiquadFilter::FiltType) newValue);
		break;
		case FUNC_FENVA:
		//filtEnvelope_.setAttack(pgm_read_word(&(ENV_ADR_INC[newValue])));
		txUintMess(voice,MESS_FENVA,pgm_read_word(&(ENV_ADR_INC[newValue])));
		break;
		case FUNC_FENVDR:
		//filtEnvelope_.setDecay(pgm_read_word(&(ENV_ADR_INC[newValue])));
		//filtEnvelope_.setRelease(pgm_read_word(&(ENV_ADR_INC[newValue])));
		txUintMess(voice,MESS_FENVDR,pgm_read_word(&(ENV_ADR_INC[newValue])));
		break;
		case FUNC_FENVS:
		//filtEnvelope_.setSustain(pgm_read_word(&(ENV_S_LEVEL[newValue])));
		txIntMess(voice,MESS_FENVS,pgm_read_word(&(ENV_S_LEVEL[newValue])));
		break;
		case FUNC_AENVA:
		//ampEnvelope_.setAttack(pgm_read_word(&(ENV_ADR_INC[newValue])));
		txUintMess(voice,MESS_AENVA,pgm_read_word(&(ENV_ADR_INC[newValue])));
		break;
		case FUNC_AENVD:
		//ampEnvelope_.setDecay(pgm_read_word(&(ENV_ADR_INC[newValue])));
		txUintMess(voice,MESS_AENVD,pgm_read_word(&(ENV_ADR_INC[newValue])));
		break;
		case FUNC_AENVS:
		//ampEnvelope_.setSustain(pgm_read_word(&(ENV_S_LEVEL[newValue])));
		txIntMess(voice,MESS_AENVS,pgm_read_word(&(ENV_S_LEVEL[newValue])));
		break;
		case FUNC_AENVR:
		//ampEnvelope_.setRelease(pgm_read_word(&(ENV_ADR_INC[newValue])));
		txUintMess(voice,MESS_AENVR,pgm_read_word(&(ENV_ADR_INC[newValue])));
		break;
		case FUNC_LFOTYPE:
		//lfo_.setTable(newValue);
		break;
		case FUNC_LFOSPEED:
		//lfo_.setDivision(newValue);
		break;
		case FUNC_MINLENGTH:
		//arpeggiator_->setType(newValue);
		break;
		case FUNC_QUANT:
		//arpeggiator_->setDivision(newValue);
		break;
		case FUNC_PORTA:
		//portamento_.setSpeed(pgm_read_word(&(PORTA_SPEED[newValue])));
		break;
		case FUNC_FMAMT:
		//wavecrusher_.setType(newValue);
		break;
	}
	if(voice==voice_ && func==function_)
	{
		base_->engineValueChanged(newValue);
	}
}

void MulAtmegaAtmEngine::patchOptionChanged(unsigned char voice, unsigned char func, bool newOpt)
{
	unsigned char i;
	switch (func)
	{
		case FUNC_WAVE:
		txWave(voice,newOpt,patch_->getFunctionValue(voice,FUNC_WAVE));
		//oscillator_->setBank((unsigned char)newOpt);
		break;
		case FUNC_FILT:
		//filter_.setGainAdj(newOpt);
		break;
		case FUNC_FENVA:
		case FUNC_FENVDR:
		case FUNC_FENVS:
		//filtEnvelope_.setInvert(newOpt);
		tieOptions(FUNC_FENVA,FUNC_FENVS,newOpt);
		break;
		case FUNC_AENVD:
		break;
		case FUNC_AENVS:
		//oscillator_->setUserMode(newOpt);
		break;
		case FUNC_LFOTYPE:
		case FUNC_LFOSPEED:
		//lfo_.setInvert(newOpt);
		tieOptions(FUNC_LFOTYPE,FUNC_LFOSPEED,newOpt);
		break;
		case FUNC_MINLENGTH:
		break;
		case FUNC_QUANT:
		break;
		//arpeggiator_->setPingPong(newOpt);
		//tieOptions(FUNC_ARPTYPE,FUNC_ARPSPEED,newOpt);
		break;
		case FUNC_PORTA:
		//portamento_.setProportional(newOpt);
		break;
		case FUNC_FMAMT:
		break;
	}
	if(voice==voice_ && func==function_)
	{
		base_->engineOptionChanged(newOpt);
	}
}

void MulAtmegaAtmEngine::patchCtrlChanged(unsigned char voice, unsigned char bank, unsigned char ctrl, unsigned char newValue)
{
	switch (ctrl)
	{
		case CTRL_FILT:
		//filter_.setFc(newValue);
		tieControls(bank,ctrl);
		break;
		case CTRL_Q:
		//filter_.setQ(newValue);
		tieControls(bank,ctrl);
		break;
		case CTRL_ENV:
		if(bank==LOW)
		{
			//filter_.setEnvAmount(newValue);
		}
		else
		{
			//pitch_.setEnvAmount(newValue);
		}
		break;
		case CTRL_LFO:
		if(bank==LOW)
		{
			//filter_.setLfoAmount(newValue);
		}
		else
		{
			//pitch_.setLfoAmount(newValue);
		}
		break;
		case CTRL_AMP:
		if(bank==LOW)
		{
			//amplifier_.setLfoAmount(newValue);
		}
		else
		{
			//pwm_->setLfoAmount(newValue);
		}
		break;
		case CTRL_FX:
		if(bank==LOW)
		{
			//clipdistortion_.setDistLevel(newValue);
		}
		else
		{
			//flanger_->setLfoAmount(newValue);
		}
		break;
	}
}