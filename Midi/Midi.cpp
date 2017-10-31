//Midi.cpp  Midi processing class
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

#include "Midi.h"

// default constructor
Midi::Midi(MidiBase* base, unsigned char sysexProdId)
{
	base_ = base;
	sysexProductId_ = sysexProdId;
} //Midi

// default destructor
Midi::~Midi()
{
} //~Midi

unsigned char Midi::getClockTicksPassed()
{
	unsigned char ticksPassed = clockTicks_ - clockTicksLast_;
	clockTicksLast_ = clockTicks_;
	return ticksPassed;
}
void Midi::reset()
{
	for(unsigned char i=0;i<128;++i)
	{
		base_->midiNoteOffReceived(i);
	}
}

void Midi::read(unsigned char inByte)
{
	static unsigned char noteByte=0;
	static unsigned char velByte=0;
	static unsigned char statusBuffer=0;
	static bool firstByte;
	static unsigned char sysexRxMnftr = 255;
	static unsigned char sysexRxProdid = 255;

	if (inByte>=CLOCK)
	{
		switch (inByte)
		{
			case CLOCK:
			clockTicks_++;
			break;
			case START:
			case CONTINUE:  //currently implemented same as start
			clockTicks_ = 0;
			clockTicksLast_ = 0;
			clockRunning_ = true;
			base_->midiClockStartReceived();
			break;
			case STOP:
			clockRunning_ = false;
			base_->midiClockStopReceived();
			break;
		}
	}
	else if (inByte>=SYSEX_START)
	{
		statusBuffer = 0;
		if (inByte==SYSEX_START)
		{
			sysexRx_ = true;
			sysexIndex_ = 0;
			sysexRxMnftr = 255;
			sysexRxProdid = 255;
			if(sysexEnable_==true)
			{
				base_->midiSysexStartReceived();
			}
			
		}
		if (inByte==SYSEX_STOP && sysexRx_==true)
		{
			sysexRx_ = false;
			if(sysexEnable_==true)
			{
				base_->midiSysexStopReceived();
			}
		}
	}
	else if (inByte>=NOTE_OFF)
	{
		statusBuffer = inByte;
		firstByte = true;
		noteByte = 0;
		velByte = 0;
		sysexRx_ = false;
	}
	else if (sysexRx_==true)
	{
		sysexIndex_++;
		if(sysexIndex_==1)
		{
			sysexRxMnftr = inByte;
		}
		else if(sysexIndex_==2)
		{
			sysexRxProdid = inByte;
		}
		else if (sysexRxMnftr==SYSEX_MANUFACTURER && sysexRxProdid==sysexProductId_)
		{
			if(sysexEnable_==true)
			{
				base_->midiSysexDataReceived(sysexIndex_-3,inByte);
			}
		}
	}
	else if (statusBuffer!=0)
	{
		if (firstByte == true)
		{
			noteByte = inByte;
			firstByte = false;
			if (progChangeEnable_==true && statusBuffer==(PROGRAM_CHANGE | channel_))
			{
				base_->midiProgramChangeReceived(noteByte);
			}
		}
		else
		{
			velByte = inByte;
			if (statusBuffer == (NOTE_ON | channel_) && velByte > 0)
			{
				if(noteOn_[noteByte]==false)
				{
					noteOn_[noteByte] = true;
					totNotesOn_++;
					base_->midiNoteOnReceived(noteByte,velByte);
				}

			}
			else if (statusBuffer == (NOTE_OFF | channel_) || (statusBuffer == (NOTE_ON | channel_) && velByte == 0))
			{
				if(noteOn_[noteByte]==true)
				{
					noteOn_[noteByte] = false;
					totNotesOn_--;
					base_->midiNoteOffReceived(noteByte);
				}
			}
			else if (statusBuffer == (CONTROL_CHANGE | channel_))
			{
				base_->midiControlChangeReceived(noteByte,velByte);
			}
			else if (statusBuffer == (PITCH_BEND | channel_))
			{
				if(velByte==0)
				{
					base_->midiPitchBendReceived(-127);
				}
				else if (velByte==127)
				{
					base_->midiPitchBendReceived(127);
				}
				else
				{
					base_->midiPitchBendReceived((velByte-64)<<1);
				}
			}

			noteByte = 0;
			velByte = 0;
			firstByte = true;
		}
	}
}
void Midi::writeSysex(unsigned char* sysMess, unsigned int size)
{
	base_->midiSysexWrite(SYSEX_START);
	base_->midiSysexWrite(SYSEX_MANUFACTURER);
	base_->midiSysexWrite(sysexProductId_);
	for(unsigned int i=0;i<size;++i)
	{
		base_->midiSysexWrite(sysMess[i]);
	}
	base_->midiSysexWrite(SYSEX_STOP);
}
