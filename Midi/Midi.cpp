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
Midi::Midi(MidiBase* base, unsigned char sysex_prod_id)
{
	base_ = base;
	sysexProductId_ = sysex_prod_id;
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
void Midi::setChannel(unsigned char new_channel)
{
	if(new_channel>15)
	{
		channel_ = 0;
	}
	else
	{
		channel_ = new_channel;
	}
}
void Midi::reset()
{
	for(unsigned char i=0;i<128;++i)
	{
		base_->midiNoteOffReceived(i);
	}
}

void Midi::read(unsigned char in_byte)
{
	static unsigned char note_byte=0;
	static unsigned char vel_byte=0;
	static unsigned char status_buffer=0;
	static bool first_byte;
	static unsigned char sysex_rx_mnftr = 255;
	static unsigned char sysex_rx_prodid = 255;

	if (in_byte>=CLOCK)
	{
		switch (in_byte)
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
	else if (in_byte>=SYSEX_START)
	{
		status_buffer = 0;
		if (in_byte==SYSEX_START)
		{
			sysexRx_ = true;
			sysexIndex_ = 0;
			sysex_rx_mnftr = 255;
			sysex_rx_prodid = 255;
			if(sysexEnable_==true)
			{
				base_->midiSysexStartReceived();
			}
			
		}
		if (in_byte==SYSEX_STOP && sysexRx_==true)
		{
			sysexRx_ = false;
			if(sysexEnable_==true)
			{
				base_->midiSysexStopReceived();
			}
		}
	}
	else if (in_byte>=NOTE_OFF)
	{
		status_buffer = in_byte;
		first_byte = true;
		note_byte = 0;
		vel_byte = 0;
		sysexRx_ = false;
	}
	else if (sysexRx_==true)
	{
		sysexIndex_++;
		if(sysexIndex_==1)
		{
			sysex_rx_mnftr = in_byte;
		}
		else if(sysexIndex_==2)
		{
			sysex_rx_prodid = in_byte;
		}
		else if (sysex_rx_mnftr==SYSEX_MANUFACTURER && sysex_rx_prodid==sysexProductId_)
		{
			if(sysexEnable_==true)
			{
				base_->midiSysexDataReceived(sysexIndex_-3,in_byte);
			}
		}
	}
	else if (status_buffer!=0)
	{
		if (first_byte == true)
		{
			note_byte = in_byte;
			first_byte = false;
		}
		else
		{
			vel_byte = in_byte;
			if (status_buffer == (NOTE_ON | channel_) && vel_byte > 0)
			{
				base_->midiNoteOnReceived(note_byte,vel_byte);
			}
			else if (status_buffer == (NOTE_OFF | channel_) || (status_buffer == (NOTE_ON | channel_) && vel_byte == 0))
			{
				base_->midiNoteOffReceived(note_byte);
			}
			else if (status_buffer == (PITCH_BEND | channel_))
			{
				if(vel_byte==0)
				{
					base_->midiPitchBendReceived(-127);
				}
				else if (vel_byte==127)
				{
					base_->midiPitchBendReceived(127);
				}
				else
				{
					base_->midiPitchBendReceived((vel_byte-64)<<1);
				}
			}
			else if (status_buffer == (CONTROL_CHANGE | channel_))
			{
				base_->midiControlChangeReceived(note_byte,vel_byte);
			}
			note_byte = 0;
			vel_byte = 0;
			first_byte = true;
		}
	}
}
void Midi::writeSysex(unsigned char* sys_mess, unsigned int size)
{
	base_->midiSysexWrite(SYSEX_START);
	base_->midiSysexWrite(SYSEX_MANUFACTURER);
	base_->midiSysexWrite(sysexProductId_);
	for(unsigned int i=0;i<size;++i)
	{
		base_->midiSysexWrite(sys_mess[i]);
	}	
	base_->midiSysexWrite(SYSEX_STOP);
}
