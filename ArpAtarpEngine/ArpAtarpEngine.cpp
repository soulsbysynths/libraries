//ArpAtarpEngine.cpp  AT-ARP data processing engine
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
	midi_ = new Midi(this,SYSEX_PROD_ID);
	arpeggiator_ = new Arpeggiator(this,midi_->getNoteOnPtr());
}
void ArpAtarpEngine::initialize()
{
	masterClock_.setTicksPerCycle(MIDI_TICKSPERCYCLE);
	setFunction(FUNC_PATTERN);
	setValue(FUNC_DIV,8);
	setValue(FUNC_PATTERN,1);
}
void ArpAtarpEngine::poll(unsigned int midiClksPassed)
{
	if (midiClksPassed>255)  //ridic scenario for 1ppqn and 30bpm!
	{
		while(midiClksPassed>255)
		{
			masterClock_.refresh(255);
			midiClksPassed -= 255;
		}
		masterClock_.refresh(midiClksPassed);
	}
	else
	{
		masterClock_.refresh((unsigned char)midiClksPassed);
	}
	arpeggiator_->refresh(masterClock_.getOutput());
}

void ArpAtarpEngine::triggerNote(unsigned char note)
{
	unsigned int newOut;
	char shift = ((char)value_[FUNC_CVOCTAVE] - 3) * 12;
	unsigned char min = ROOT_NOTE_OFFSET + shift;
	unsigned char max = min + 60;

	if(note >= min && note <= max)
	{
		newOut = (((unsigned long)note - min) << 10) / 15;
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
		case FUNC_CVOCTAVE:
		break;
		
		case FUNC_CLOCKPPQN:

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

void ArpAtarpEngine::setRootSource(InputSource newSource)
{

}

void ArpAtarpEngine::setChordSource(InputSource newSource)
{

}

void ArpAtarpEngine::setRootNote(unsigned char newNote)
{
	if(newNote!=chordGen_.getRootNote())
	{
		chordGen_.setRootNote(newNote);
		txChordGenMidi();
	}
}

void ArpAtarpEngine::setChordNote(unsigned char newNote)
{
	if(newNote!=chordGen_.getChordNote())
	{
		chordGen_.setChordNote(newNote);
		txChordGenMidi();
	}
}

void ArpAtarpEngine::txChordGenMidi()
{
	if ((rootSource_==IS_MIDI && chordSource_==IS_MIDI)  || func_==FUNC_RECORD)
	{
		return;
	}
	unsigned char i;
	unsigned char c = midi_->getChannel();
	const unsigned char off = Midi::NOTE_OFF | c;
	const unsigned char on = Midi::NOTE_ON | c;
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
		if(rootSource_==IS_MIDI && chordSource_==IS_MIDI)
		{
			if(arpeggiator_->getType()==0)
			{
				triggerNote(note);
			}
			arpeggiator_->buildNoteOrder();
			//if (arpeggiator_->getType()>0 && totNotesOnLast_==0 && midi_->getTotNotesOn()==1 && midi_->getClockRunning()==false)
			//{
			//masterClock_.reset();
			//arpeggiator_->reset();
			//}
			//totNotesOnLast_ = midi_->getTotNotesOn();
		}
		else if (rootSource_==IS_MIDI)
		{
			setRootNote(note);
		}
		else if (chordSource_==IS_MIDI)
		{
			setChordNote(note);
		}
	}
}
void ArpAtarpEngine::midiNoteOffReceived(unsigned char note)
{
	arpeggiator_->buildNoteOrder();
	totNotesOnLast_ = midi_->getTotNotesOn();
}

void ArpAtarpEngine::midiClockStartReceived()
{
	masterClock_.reset();
	arpeggiator_->reset();
}
void ArpAtarpEngine::midiClockStopReceived()
{
	masterClock_.reset();
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
