/*
* ArpAtarpEngine.cpp
*
* Created: 12/04/2017 20:09:11
* Author: paulsoulsby
*/


#include "ArpAtarpEngine.h"

// default constructor
ArpAtarpEngine::ArpAtarpEngine(ArpAtarpEngineBase* base)
{
	construct(base);
} //ArpAtarpEngine

// default destructor
ArpAtarpEngine::~ArpAtarpEngine()
{
	if(midi_!=NULL)
	{
		delete midi_;
	}
} //~ArpAtarpEngine

void ArpAtarpEngine::construct(ArpAtarpEngineBase* base)
{
	base_ = base;
	masterClock_.setTicksPerCycle(BPM_TICKS);
	midi_ = new Midi(this,SYSEX_PROD_ID);
	arpeggiator_ = new Arpeggiator(this,midi_->getNoteOnPtr());
}
void ArpAtarpEngine::initialize()
{
	setFunction(FUNC_PATTERN);
	setValue(FUNC_DIV,8);
	setValue(FUNC_PATTERN,1);
}
void ArpAtarpEngine::poll(unsigned char ticksPassed)
{
	if(clockSource_==CS_INTERNAL)
	{
		masterClock_.refresh(ticksPassed);
	}
	else
	{
		masterClock_.refresh(midi_->getClockTicksPassed());
	}
	arpeggiator_->refresh(masterClock_.getOutput());
}

void ArpAtarpEngine::triggerNote(unsigned char note)
{
	unsigned int newOut;
	if(note >= 36 && note < 96)
	{
		newOut = (((unsigned long)note - 36) << 10) / 15;
		if(newOut!=output_)
		{
			output_ = newOut;
			base_->engineOutputChanged(newOut);
		}
	}
}

void ArpAtarpEngine::setFunction(Function newFunction)
{
	func_ = newFunction;
	base_->engineFunctionChanged(newFunction);
	if(newFunction==FUNC_RECORD || newFunction==FUNC_RESET)
	{
		setValue(newFunction,0);
	}
}

void ArpAtarpEngine::setValue(Function func, unsigned char newValue)
{
	value_[func] = newValue;
	switch(func)
	{
		case FUNC_PATTERN:
		if(newValue>0x0F)
		{
			arpeggiator_->setUserMode(true);
			base_->engineLoadUserPattern(newValue - 16);
		}
		else
		{
			arpeggiator_->setUserMode(false);
			arpeggiator_->setType(newValue);
		}
		break;
		case FUNC_RECORD:
		break;
		case FUNC_RESET:
		if (newValue==2)
		{
			unsigned char p = value_[FUNC_PATTERN] & 0x0F;
			arpeggiator_->resetUserPattern(p);
			for(unsigned char i=0;i<16;++i)
			{
				base_->engineSaveUserPatternStep(p,i,arpeggiator_->getUserPattern(i));
			}
			setFunction(FUNC_PATTERN);
		}
		break;
		case FUNC_PINGPONG:
		arpeggiator_->setPingPong((bool)newValue);
		break;
		case FUNC_DIV:
		arpeggiator_->setDivision(newValue);
		break;
		case FUNC_NOTESOURCE:
		noteSource_ = newValue;
		if(noteSource_==NS_CV)
		{
			txChordGenMidi();
		}
		else
		{
			midi_->reset();
		}
		break;
		case FUNC_CLOCKSOURCE:
		clockSource_ = newValue;
		break;
		case FUNC_MIDICHANNEL:
		midi_->setChannel(newValue);
		break;
	}
	if (func==func_)
	{
		base_->engineValueChanged(newValue);
	}
}

void ArpAtarpEngine::txChordGenMidi()
{
	if (noteSource_==NS_MIDI || func_==FUNC_RECORD)
	{
		return;
	}
	unsigned char i;
	unsigned char c = midi_->getChannel();
	unsigned char off = Midi::NOTE_OFF | c;
	unsigned char on = Midi::NOTE_ON | c;
	for(i=0;i<127;++i)
	{
		if(midi_->getNoteOn(i)==true)
		{
			midi_->read(off);
			midi_->read(i);
			midi_->read(0);
		}
	}
	for(i=0;i<chordGen_.getChordSize();++i)
	{
		midi_->read(on);
		midi_->read(chordGen_.getNoteNumbers(i));
		midi_->read(127);
	}
}

//***********************MIDI events********************************************
void ArpAtarpEngine::midiNoteOnReceived(unsigned char note, unsigned char velocity)
{
	if(func_==FUNC_RECORD)
	{
		char rec = (char)note - 60;
		arpeggiator_->setUserPattern(value_[FUNC_RECORD],rec);
		base_->engineSaveUserPatternStep(value_[FUNC_PATTERN]-16,value_[FUNC_RECORD],rec);
		if(value_[FUNC_RECORD]==15)
		{
			setFunction(FUNC_PATTERN);
		}
		else
		{
			setValue(FUNC_RECORD,value_[FUNC_RECORD]+1);
		}
	}
	else
	{
		if(arpeggiator_->getType()==0)
		{
			triggerNote(note);
		}
		arpeggiator_->buildNoteOrder();
		if (arpeggiator_->getType()>0 && totNotesOnLast_==0 && midi_->getTotNotesOn()==1 && midi_->getClockRunning()==false)
		{
			masterClock_.reset();
			arpeggiator_->reset();
		}
		totNotesOnLast_ = midi_->getTotNotesOn();
	}
}
void ArpAtarpEngine::midiNoteOffReceived(unsigned char note)
{
	arpeggiator_->buildNoteOrder();
	totNotesOnLast_ = midi_->getTotNotesOn();
}

void ArpAtarpEngine::midiClockStartReceived()
{
	clockSource_ = CS_EXTERNAL;
	masterClock_.reset();
	masterClock_.setTicksPerCycle(MIDI_TICKSPERCYCLE);
	arpeggiator_->reset();
}
void ArpAtarpEngine::midiClockStopReceived()
{
	masterClock_.reset();
	masterClock_.setTicksPerCycle(BPM_TICKS);
	arpeggiator_->reset();
}
void ArpAtarpEngine::midiControlChangeReceived(unsigned char cc, unsigned char val)
{
	switch ((MidiCC)cc)
	{
		case CC_ALLNOTESOFF:
		midi_->reset();
		break;
	}
}
//*********************Arpeggiator events***************************
void ArpAtarpEngine::arpeggiatorNoteEvent(unsigned char lastNote, unsigned char newNote)
{
	if(newNote>0)
	{
		triggerNote(newNote);
	}
}
