//Ody.cpp  Odytron class (add this object to Arduino sketch)
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


#include "Ody.h"

// default constructor
Ody::Ody()  : engine_(OdyEngine::getInstance()), hardware_(AtmHardware::getInstance())
{
	engine_.construct(this);
	hardware_.construct(this);
} //Ody

// default destructor
Ody::~Ody()
{
} //~Ody

void Ody::initialize()
{
	engine_.initialize();
	hardware_.getRotEncoder(AtmHardware::FUNCTION).setContinuous(true);
	hardware_.getRotEncoder(AtmHardware::VALUE).setContinuous(false);
	hardware_.beginMidi(MIDI_UBRR);
	engine_.getMidiPtr()->setChannel(hardware_.readMidiChannel());
	if(hardware_.getMidiChannelSelectMode()==true)
	{
		hardware_.getLedSwitch(AtmHardware::FUNCTION).setColour(LedRgb::YELLOW);
		hardware_.getLedSwitch(AtmHardware::VALUE).setColour(LedRgb::OFF);
		hardware_.getLedSwitch(AtmHardware::BANK).setColour(LedRgb::OFF);
		hardware_.getLedCircular(AtmHardware::VALUE).setState(0);
		hardware_.getRotEncoder(AtmHardware::FUNCTION).setValue((char)engine_.getMidiPtr()->getChannel());
	}
	else
	{
		for(unsigned char i=0;i<2;++i)
		{
			hardware_.getLedSwitch(i).flash(4,LED_FLASH_TICKS,LED_FLASH_TICKS,LedRgb::YELLOW,LedRgb::RED,true);
		}
	}
}

void Ody::poll(unsigned char ticksPassed)
{
	hardware_.pollMidi();
	hardware_.pollAnlControls(ticksPassed);
	hardware_.pollSwitches(ticksPassed);
	hardware_.pollRotEncoders(ticksPassed);
	hardware_.refreshFlash(ticksPassed);
	hardware_.refreshLeds();
	engine_.poll(ticksPassed);
}

void Ody::processSample()
{
	static int mix_out = 0;
	static unsigned char filt_out = 0;
	static unsigned char stage = 0;

	if (stage==0)
	{
		mix_out = engine_.getMixerOutput();
	}
	else
	{
		filt_out = engine_.filterOutput(mix_out);
	}
	OCR2B = filt_out;
	stage = 1 - stage;
}

//***********engine events*********************
void Ody::engineFunctionChanged(unsigned char func, unsigned char val, bool opt)
{
	if(hardware_.getMidiChannelSelectMode()==true)
	{
		hardware_.getLedCircular(AtmHardware::FUNCTION).select(val);
	}
	else
	{
		if(func==OdyEngine::FUNC_FILTTYPE)
		{
			switch (val)
			{
				case 0:
				hardware_.getLedSwitch(AtmHardware::BANK).setColour(LedRgb::YELLOW);
				break;
				case 1:
				hardware_.getLedSwitch(AtmHardware::BANK).setColour(LedRgb::RED);
				break;
				case 2:
				hardware_.getLedSwitch(AtmHardware::BANK).setColour(LedRgb::GREEN);
				break;
			}  	
		}
		else
		{
			hardware_.getRotEncoder(AtmHardware::VALUE).setValue((char)val);
			hardware_.getLedCircular(AtmHardware::VALUE).fill(val);
			if(func==OdyEngine::FUNC_ENVA2)
			{
				hardware_.getLedCircular(AtmHardware::FUNCTION).select((unsigned char)OdyEngine::FUNC_ENVA);
			}
			else if(func==OdyEngine::FUNC_ENVR2)
			{
				hardware_.getLedCircular(AtmHardware::FUNCTION).select((unsigned char)OdyEngine::FUNC_ENVR);
			}
			else
			{
				hardware_.getLedCircular(AtmHardware::FUNCTION).select((unsigned char)func);
			}
			if(opt==true)
			{
				hardware_.getLedSwitch(AtmHardware::FUNCTION).setColour(LedRgb::RED);
			}
			else
			{
				hardware_.getLedSwitch(AtmHardware::FUNCTION).setColour(LedRgb::YELLOW);
			}		
		}
	}

}

//**************************hardware events************************************
void Ody::hardwareAnalogueControlChanged(unsigned char control, unsigned char newValue)
{
	if(hardware_.getMidiChannelSelectMode()==true)
	{
		return;
	}
	engine_.getPatchPtr()->setCtrlValue(control,newValue);
}

void Ody::hardwareSwitchChanged(unsigned char sw, unsigned char newValue)
{
	bool opt;
	if(hardware_.getMidiChannelSelectMode()==true)
	{
		return;
	}
	if(sw==AtmHardware::FUNCTION)
	{
		if(engine_.getFunction()==OdyEngine::FUNC_PORTAMENTO)
		{
			if(newValue==HIGH)
			{
				hardware_.getLedSwitch(AtmHardware::FUNCTION).setColour(LedRgb::RED);
			}
			else
			{
				hardware_.getLedSwitch(AtmHardware::FUNCTION).flashStop();
				hardware_.getLedSwitch(AtmHardware::FUNCTION).setColour(LedRgb::RED);
				if(hardware_.getSwitch(sw).getHoldTime()>AtmHardware::HOLD_EVENT_TICKS)
				{
					engine_.getPatchPtr()->writePatch(0);
				}
				else
				{
					switch(engine_.getFunction())
					{
						case OdyEngine::FUNC_PORTAMENTO:
						engine_.getPatchPtr()->readPatch(0);
						break;
					}
				}
			}
		}
		else if(newValue==HIGH)
		{
			opt = !engine_.getPatchPtr()->getOptionValue(engine_.getFunction());
			engine_.getPatchPtr()->setOptionValue(engine_.getFunction(),opt);  //the LEDs are set by engine call back
		}
	}

	
	if(sw==AtmHardware::VALUE)
	{
		if(newValue==HIGH)
		{
			hardware_.getRotEncoder(AtmHardware::VALUE).setUpdateVal(false);
			hardware_.getLedSwitch(AtmHardware::VALUE).setColour(LedRgb::RED);
			engine_.midiNoteOnReceived(testNote_,127);
		}
		else
		{
			hardware_.getRotEncoder(AtmHardware::VALUE).setUpdateVal(true);
			hardware_.getLedSwitch(AtmHardware::VALUE).setColour(LedRgb::YELLOW);
			engine_.midiNoteOffReceived(testNote_);
		}
	}
	
	if(sw==AtmHardware::BANK)
	{
		if(newValue==HIGH)
		{
			unsigned char filt = (unsigned char)engine_.getFilter().getType();
			filt++;
			if(filt>2)
			{
				filt = 0;
			}
			engine_.getPatchPtr()->setFunctionValue(OdyEngine::FUNC_FILTTYPE,filt);
		}
	}
}
void Ody::hardwareSwitchHeld(unsigned char sw)
{
	if(hardware_.getMidiChannelSelectMode()==true)
	{
		return;
	}
	if(sw==AtmHardware::FUNCTION && engine_.getFunction()==OdyEngine::FUNC_PORTAMENTO)
	{
		hardware_.getLedSwitch(AtmHardware::FUNCTION).flash(8,LED_FLASH_TICKS,LED_FLASH_TICKS,LedRgb::RED,LedRgb::YELLOW,true);
	}
	//if(sw==AtmHardware::BANK)
	//{
		//if(engine_.getBank()==LOW)
		//{
			//hardware_.getLedSwitch(AtmHardware::BANK).flash(8,LED_FLASH_TICKS,LED_FLASH_TICKS,LedRgb::RED,LedRgb::YELLOW,true);
		//}
		//else
		//{
			//hardware_.getLedSwitch(AtmHardware::BANK).flash(8,LED_FLASH_TICKS,LED_FLASH_TICKS,LedRgb::GREEN,LedRgb::YELLOW,true);
		//}
		//engine_.getPatchPtr()->easterEgg(engine_.getMasterClock().getOutput());
	//}
}
void Ody::hardwareRotaryEncoderChanged(unsigned char rotary, unsigned char newValue, bool clockwise)
{
	if(hardware_.getMidiChannelSelectMode()==true)
	{
		if(rotary==AtmHardware::FUNCTION)
		{
			hardware_.writeMidiChannel(newValue);
			engine_.getMidiPtr()->setChannel(newValue);
		}
	}
	else
	{
		if(rotary==AtmHardware::FUNCTION)
		{
			engine_.setFunction((OdyEngine::Func)newValue);
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

}
void Ody::hardwareMidiReceived(unsigned char data)
{
	hardware_.getLedMidi().flash(1,LED_FLASH_TICKS,LED_FLASH_TICKS,true);  //do at this level, not in hardware
	engine_.getMidiPtr()->read(data);
}
void Ody::hardwareMidiError(unsigned char errorType)
{
	hardware_.getLedSwitch(AtmHardware::VALUE).setColour((LedRgb::LedRgbColour)(3-(unsigned char)hardware_.getLedSwitch(AtmHardware::VALUE).getColour()));
	hardware_.getLedCircular(AtmHardware::VALUE).fill(errorType);
}