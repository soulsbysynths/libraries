/*
//ArpHardwareTester.cpp  AT-ARP hardware tester
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
*/


#include "ArpHardwareTester.h"

// default constructor
ArpHardwareTester::ArpHardwareTester() : hardware_(ArpHardware::getInstance())
{
	hardware_.construct(this);
	midi_ = new Midi(this,SYSEX_PROD_ID);
} //ArpHardwareTester

// default destructor
ArpHardwareTester::~ArpHardwareTester()
{
} //~ArpHardwareTester

void ArpHardwareTester::init()
{
	hardware_.beginMidi(MIDI_UBRR);
}

void ArpHardwareTester::initMemory()
{
	hardware_.writeEepromByte(ArpHardware::EEPROM_MIDI_CHANNEL,0);
	hardware_.writeEepromUInt(ArpHardware::EEPROM_OUTPUT_LOW,1024); //outmax = 3072 = 3.75V, outmin = 1024 = 1.25V, max-min = 2048 (<<11)
	hardware_.writeEepromUInt(ArpHardware::EEPROM_OUTPUT_HIGH,3072);
	hardware_.writeEepromUInt(ArpHardware::EEPROM_CHORD_LOW,1024); //outmax = 3072 = 3.75V, outmin = 1024 = 1.25V, max-min = 2048 (<<11)
	hardware_.writeEepromUInt(ArpHardware::EEPROM_CHORD_HIGH,3072);
	hardware_.writeEepromUInt(ArpHardware::EEPROM_ROOT_LOW,1024);
	hardware_.writeEepromUInt(ArpHardware::EEPROM_ROOT_HIGH,3072);

}

void ArpHardwareTester::poll(unsigned char ticksPassed)
{
	//static unsigned int check = 0;
	//static unsigned char checkChar = 0;
	//check += ticksPassed;
	//if(check>=1000)
	//{
	//check -= 1000;
	//checkChar++;
	//checkChar &= 0x0F;
	//hardware_.getLed().displayHex(checkChar);
	//}
	hardware_.pollCalibCvInputs();
	hardware_.pollDigiPins(ticksPassed);
	hardware_.pollSwitches(ticksPassed);
	hardware_.pollMidi();
	hardware_.refreshLeds();
	hardware_.refreshFlash(ticksPassed);
}

//*****************  Hardware events **********************
void ArpHardwareTester::hardwareCvInputChanged(unsigned char input, unsigned int newValue)
{
	static unsigned char cv[2] = {0};
	unsigned char newVal = newValue >> 8;
	if(newVal!=cv[input])
	{
		cv[input] = newVal;
		hardware_.getLed().displayHex(newVal);
		if(input==0)
		{
			hardware_.getLed().setDp(false);
		}
		else
		{
			hardware_.getLed().setDp(true);
		}
		
	}
	if(hardware_.getJackDetect(input)==true)
	{
		hardware_.setOutput(newValue);
	}
}

void ArpHardwareTester::hardwareGateInputChanged(bool newValue)
{
	hardware_.getLed().displayChar('K');
	hardware_.getLed().setDp(newValue);
}

void ArpHardwareTester::hardwareJackDetectChanged(unsigned char jack, bool newValue)
{
	hardware_.getLed().displayHex(jack);
	hardware_.getLed().setDp(newValue);
	if(newValue==true)
	{
		calibMode_ = jack;
	}
	else
	{
		calibMode_ = CB_OUTPUT;
	}
}

void ArpHardwareTester::hardwareMidiChannelChanged(unsigned char channel)
{

}

void ArpHardwareTester::hardwareMidiError(unsigned char errorType)
{
	
}

void ArpHardwareTester::hardwareMidiReceived(unsigned char data)
{
	midi_->read(data);
}

void ArpHardwareTester::hardwareSwitchChanged(unsigned char sw, unsigned char newValue)
{
	if(sw==ArpHardware::SW_UP)
	{
		hardware_.getLed().displayChar('U');
		if(newValue==HIGH)
		{
			if(calibMode_==CB_OUTPUT)
			{
				if(midiNote_==51)
				{
					hardware_.setOutCalib(0,hardware_.getOutCalib(0)+1);
				}
				else if(midiNote_==81)
				{
					hardware_.setOutCalib(1,hardware_.getOutCalib(1)+1);
				}
			}
		}
	}
	else
	{
		hardware_.getLed().displayChar('D');
		if(newValue==HIGH)
		{
			if(calibMode_==CB_OUTPUT)
			{
				if(midiNote_==51)
				{
					hardware_.setOutCalib(0,hardware_.getOutCalib(0)-1);
				}
				else if(midiNote_==81)
				{
					hardware_.setOutCalib(1,hardware_.getOutCalib(1)-1);
				}
			}
		}
	}
	hardware_.getLed().setDp((bool)newValue);
}

void ArpHardwareTester::hardwareSwitchHeld(unsigned char sw)
{
	switch (calibMode_)
	{
		case CB_ROOT:
		if(sw==ArpHardware::SW_DOWN)
		{
			hardware_.calcCvCalib(ArpHardware::EEPROM_ROOT_LOW);
		}
		else
		{
			hardware_.calcCvCalib(ArpHardware::EEPROM_ROOT_HIGH);
		}
		break;
		case CB_CHORD:
		if(sw==ArpHardware::SW_DOWN)
		{
			hardware_.calcCvCalib(ArpHardware::EEPROM_CHORD_LOW);
		}
		else
		{
			hardware_.calcCvCalib(ArpHardware::EEPROM_CHORD_HIGH);
		}
		break;
		case CB_OUTPUT:
		if(sw==ArpHardware::SW_DOWN)
		{
			hardware_.writeOutCalib(ArpHardware::EEPROM_OUTPUT_LOW);
		}
		else
		{
			hardware_.writeOutCalib(ArpHardware::EEPROM_OUTPUT_HIGH);
		}
		break;
	}
	hardware_.getLed().flash(true,5,4,4,true);
}

//***************** MIDI EVENTS *****************************
void ArpHardwareTester::midiNoteOnReceived(unsigned char note, unsigned char velocity)
{
	if(hardware_.getJackDetect(ArpHardware::CV_ROOT)==false && hardware_.getJackDetect(ArpHardware::CV_CHORD)==false && note >= 36 && note < 96)
	{
		midiNote_ = note;
		hardware_.setOutput((((unsigned long)note - 36) << 10) / 15);
		hardware_.getLed().setDp(true);
	}
}
void ArpHardwareTester::midiNoteOffReceived(unsigned char note)
{
	hardware_.getLed().setDp(false);
}