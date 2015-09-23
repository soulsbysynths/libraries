//AtmHardwareTester.cpp  Atmegatron Hardware Tester. Used by assembly factory.
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

#include "AtmHardwareTester.h"

// default constructor
AtmHardwareTester::AtmHardwareTester() : hardware_(AtmHardware::getInstance())
{
	hardware_.construct(this);
	audio_ = new AtmAudio(WAVE_LEN);
} //AtmHardwareTester

// default destructor
AtmHardwareTester::~AtmHardwareTester()
{
} //~AtmHardwareTester

void AtmHardwareTester::init()
{
	hardware_.beginMidi(MIDI_UBRR);
	audio_->initialize();
	playSound(false);
}
void AtmHardwareTester::playSound(bool way)
{
	Wavetable wavetable(WAVE_LEN);
	for(unsigned char i=0;i<WAVE_LEN;++i)
	{
		if(way==true)
		{
			wavetable.setSample(i,sine_wave[i]);
		}
		else
		{
			wavetable.setSample(i,0);
		}
	}
	audio_->pasteWavetable(wavetable);
	if(way==true)
	{
		hardware_.writeMidi(NOTE_ON);
		hardware_.writeMidi(TEST_NOTE);
		hardware_.writeMidi(127);
	}	
	else
	{
		hardware_.writeMidi(NOTE_OFF);
		hardware_.writeMidi(TEST_NOTE);
		hardware_.writeMidi(0);
	}
}
void AtmHardwareTester::tick()
{
	unsigned char i;
	for(i=0;i<2;++i)
	{
		if(circTicking_[i]==true)
		{
			hardware_.getLedCircular(i).select(tick_);
		}
	}
	for(i=0;i<3;++i)
	{
		if(swTicking_[i]==true)
		{
			if(tick_%2==0)
			{
				hardware_.getLedSwitch(i).setColour(LedRgb::RED);
			}
			else
			{
				hardware_.getLedSwitch(i).setColour(LedRgb::GREEN);
			}
		}
	}
	tick_++;
	if(tick_>15)
	{
		tick_ = 0;
	}
}

void AtmHardwareTester::poll(unsigned char ticksPassed)
{
	
	hardware_.pollSwitches(ticksPassed);
	hardware_.pollRotEncoders(ticksPassed);
	hardware_.pollAnlControls(ticksPassed);
	hardware_.pollMidi();

	if(circTicking_[1]==false)
	{
		for(unsigned char i=0;i<6;++i)
		{
			if(hardware_.getAnlControl(i).getMoving()!=lastMoving_[i])
			{
				lastMoving_[i] = hardware_.getAnlControl(i).getMoving();
				if(lastMoving_[i]==true)
				{
					hardware_.getLedCircular(1).setSegment(i,true);
				}
				else
				{
					hardware_.getLedCircular(1).setSegment(i,false);
				}
			}

		}		
	}
	hardware_.refreshFlash(ticksPassed);
	hardware_.refreshLeds();
}

void AtmHardwareTester::hardwareAnalogueControlChanged(unsigned char control, unsigned char newValue)
{
	circTicking_[0] = false;
	circTicking_[1] = false;
	hardware_.getLedCircular(0).fill(newValue>>4);
	audio_->setSampleFreq((unsigned long)newValue*16*WAVE_LEN);
}

void AtmHardwareTester::hardwareSwitchChanged(unsigned char sw, unsigned char newValue)
{
	swTicking_[sw] = false;
	if(newValue==HIGH)
	{
		hardware_.getLedSwitch(sw).setColour(LedRgb::GREEN);
		playSound(true);
	}
	else
	{
		hardware_.getLedSwitch(sw).setColour(LedRgb::RED);
		playSound(false);
	}
}
void AtmHardwareTester::hardwareSwitchHeld(unsigned char sw)
{
	swTicking_[sw] = false;
	hardware_.getLedSwitch(sw).setColour(LedRgb::YELLOW);
}
void AtmHardwareTester::hardwareRotaryEncoderChanged(unsigned char rotary, unsigned char newValue, bool clockwise)
{
	circTicking_[rotary] = false;
	hardware_.getLedCircular(rotary).select(newValue);
}
void AtmHardwareTester:: hardwareMidiReceived(unsigned char data)
{
	hardware_.getLedMidi().flash(1,1,1,true);  //do at this level, not in hardware
	if(data==NOTE_ON)
	{
		playSound(true);
	}
	if(data==NOTE_OFF)
	{
		playSound(false);
	}
}