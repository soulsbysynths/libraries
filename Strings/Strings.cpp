//Strings.cpp  Strings top level class (add this object to Arduino sketch)
//Copyright (C) 2016  Paul Soulsby info@soulsbysynths.com
//
//This program is free software: you can redistribute it and/or modAmt_ify
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
#include "Strings.h"

// default constructor
Strings::Strings()  : engine_(StringsEngine::getInstance()), hardware_(AtmHardware::getInstance())
{
	engine_.construct(this);
	hardware_.construct(this);
} //Strings

// default destructor
Strings::~Strings()
{

} //~Strings

void Strings::initialize()
{
	hardware_.beginMidi(MIDI_UBRR);
	
	if(hardware_.getMidiChannelSelectMode()==false)
	{
		engine_.initialize();
		engine_.getMidiPtr()->setChannel(hardware_.getMidiChannel());
		engine_.setFunction(StringsEngine::FUNC_WAVEA);
		hardware_.getLedSwitch(AtmHardware::BANK).setColour(LedRgb::RED);  //mono off
		hardware_.getLedSwitch(AtmHardware::FUNCTION).flash(5,LED_FLASH_TICKS,LED_FLASH_TICKS,LedRgb::RED,LedRgb::GREEN,true);  //5 because default patch is green
		hardware_.getLedSwitch(AtmHardware::VALUE).flash(4,LED_FLASH_TICKS,LED_FLASH_TICKS,LedRgb::RED,LedRgb::GREEN,true);  
	}
}
void Strings::poll(unsigned char ticksPassed)
{
	hardware_.pollMidi();
	hardware_.pollAnlControls(ticksPassed);
	hardware_.pollSwitches(ticksPassed);
	hardware_.pollRotEncoders(ticksPassed);
	hardware_.refreshFlash(ticksPassed);
	hardware_.refreshLeds();
	if(hardware_.getMidiChannelSelectMode()==false)
	{
		engine_.poll(ticksPassed);
	}
}
bool Strings::isFuncFill(StringsEngine::Func func) const
{
	unsigned char i = func>>3;
	return (bool)bitRead(IS_FUNC_FILL[i],func-(i*8));
}

//**************************hardware events************************************
void Strings::hardwareAnalogueControlChanged(unsigned char control, unsigned char newValue)
{
	engine_.getPatchPtr()->setCtrlValue(LOW,control,newValue);
}

void Strings::hardwareSwitchChanged(unsigned char sw, unsigned char newValue)
{
	bool opt;

	if(sw==AtmHardware::FUNCTION)
	{
		if(engine_.getFunction()==StringsEngine::FUNC_MEM)
		{
			if(newValue==HIGH)
			{
				hardware_.getLedSwitch(AtmHardware::FUNCTION).setColour(LedRgb::GREEN);
			}
			else
			{
				hardware_.getLedSwitch(AtmHardware::FUNCTION).flashStop();
				hardware_.getLedSwitch(AtmHardware::FUNCTION).setColour(LedRgb::RED);
				if(hardware_.getSwitch(sw).getHoldTime()>AtmHardware::HOLD_EVENT_TICKS)
				{
					engine_.getPatchPtr()->writePatch(engine_.getPatchPtr()->getFunctionValue(StringsEngine::FUNC_MEM));
				}
				else
				{
					engine_.getPatchPtr()->setOptionValue(StringsEngine::FUNC_WAVEA,false);  //a horrible bodge to stop waves being tied together when loaded
					engine_.getPatchPtr()->readPatch(engine_.getPatchPtr()->getFunctionValue(StringsEngine::FUNC_MEM));
				}
			}
		}
		else if(newValue==HIGH)
		{
			opt = !engine_.getPatchPtr()->getOptionValue(engine_.getFunction());
			engine_.getPatchPtr()->setOptionValue(engine_.getFunction(),opt);
			if(opt==true)
			{
				hardware_.getLedSwitch(AtmHardware::FUNCTION).setColour(LedRgb::GREEN);
			}
			else
			{
				hardware_.getLedSwitch(AtmHardware::FUNCTION).setColour(LedRgb::RED);
			}
		}
	}

	
	if(sw==AtmHardware::VALUE)
	{
		if(newValue==HIGH)
		{
			hardware_.getRotEncoder(AtmHardware::VALUE).setUpdateVal(false);
			hardware_.getLedSwitch(AtmHardware::VALUE).setColour(LedRgb::GREEN);
			engine_.midiNoteOnReceived(testNote_,127);
		}
		else
		{
			hardware_.getRotEncoder(AtmHardware::VALUE).setUpdateVal(true);
			hardware_.getLedSwitch(AtmHardware::VALUE).setColour(LedRgb::RED);
			engine_.midiNoteOffReceived(testNote_);
		}
	}
	
	if(sw==AtmHardware::BANK)
	{
		if(newValue==HIGH)
		{
			engine_.setMono(!engine_.getMono());
			if(engine_.getMono()==true)
			{
				hardware_.getLedSwitch(AtmHardware::BANK).setColour(LedRgb::GREEN);
			}
			else
			{
				hardware_.getLedSwitch(AtmHardware::BANK).setColour(LedRgb::RED);
			}
		}
	}
}
void Strings::hardwareSwitchHeld(unsigned char sw)
{

	if(sw==AtmHardware::FUNCTION)
	{
		hardware_.getLedSwitch(AtmHardware::FUNCTION).flash(8,LED_FLASH_TICKS,LED_FLASH_TICKS,LedRgb::GREEN,LedRgb::RED,true);
	}
}
void Strings::hardwareRotaryEncoderChanged(unsigned char rotary, unsigned char newValue, bool clockwise)
{
	if(rotary==AtmHardware::FUNCTION)
	{
		engine_.setFunction((StringsEngine::Func)newValue);
	}

	if(rotary==AtmHardware::VALUE)
	{
		if(hardware_.getSwitch(AtmHardware::VALUE).getState()==HIGH)
		{
			if(testNote_<127 && testNote_>0)
			{
				engine_.midiNoteOffReceived(testNote_);
				if(clockwise==true)
				{
					testNote_++;
				}
				else
				{
					testNote_--;
				}
				engine_.midiNoteOnReceived(testNote_,127);
			}
		}
		else
		{
			engine_.getPatchPtr()->setFunctionValue(engine_.getFunction(),newValue);
		}
	}
}
void Strings::hardwareMidiChannelChanged(unsigned char channel)
{
	engine_.getMidiPtr()->setChannel(channel);
}
void Strings::hardwareMidiReceived(unsigned char data)
{
	hardware_.getLedMidi().flash(1,LED_FLASH_TICKS,LED_FLASH_TICKS,true);  //do at this level, not in hardware
	engine_.getMidiPtr()->read(data);
}
void Strings::hardwareMidiError(unsigned char errorType)
{
	hardware_.getLedSwitch(AtmHardware::VALUE).setColour((LedRgb::LedRgbColour)(3-(unsigned char)hardware_.getLedSwitch(AtmHardware::VALUE).getColour()));
	hardware_.getLedCircular(AtmHardware::VALUE).fill(errorType);
}
//********************Engine Events**************************************
void Strings::engineFunctionChanged(unsigned char func, unsigned char val)
{
	bool col;
	
	hardware_.getRotEncoder(AtmHardware::VALUE).setValue((char)val);
	if(isFuncFill((StringsEngine::Func)func)==true)
	{
		hardware_.getRotEncoder(AtmHardware::VALUE).setContinuous(false);
		hardware_.getLedCircular(AtmHardware::VALUE).fill(val);
	}
	else
	{
		hardware_.getRotEncoder(AtmHardware::VALUE).setContinuous(true);
		hardware_.getLedCircular(AtmHardware::VALUE).select(val);
	}
	hardware_.getLedCircular(AtmHardware::FUNCTION).select((unsigned char)func);
}
void Strings::engineOptionChanged(unsigned char opt, bool val)
{
	if(val==true)
	{
		hardware_.getLedSwitch(AtmHardware::FUNCTION).setColour(LedRgb::GREEN);
	}
	else
	{
		hardware_.getLedSwitch(AtmHardware::FUNCTION).setColour(LedRgb::RED);
	}
}