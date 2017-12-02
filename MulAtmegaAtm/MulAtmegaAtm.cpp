//MulAtmegaAtm.cpp  Atmegatron class (add this object to Arduino sketch)
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

#include "MulAtmegaAtm.h"

// default constructor
MulAtmegaAtm::MulAtmegaAtm() : engine_(MulAtmegaAtmEngine::getInstance()), hardware_(MulAtmegaHardware::getInstance())
{
	engine_.construct(this);
	hardware_.construct(this);
} //MulAtmegaAtm

// default destructor
MulAtmegaAtm::~MulAtmegaAtm()
{
} //~MulAtmegaAtm

void MulAtmegaAtm::initialize()
{
	hardware_.getRotEncoder(MulAtmegaHardware::RE_FUNCTION).setContinuous(true);
	hardware_.getRotEncoder(MulAtmegaHardware::RE_GLOBAL).setMaxValue(7);
	hardware_.beginMidi(MIDI_UBRR);
	engine_.initialize();
	engine_.getMidiPtr()->setChannel(hardware_.getMidiChannel());
	for(unsigned char i=0;i<VOICES;++i)
	{
		hardware_.setVoiceEnable(i,MulAtmegaHardware::AUDCH_LEFT,true);
		hardware_.setVoiceEnable(i,MulAtmegaHardware::AUDCH_RIGHT,true);
	}
	for(unsigned char i=0;i<ROTARY_ENCODERS;++i)
	{
		hardware_.getLedRgb(i).flash(4,LED_FLASH_TICKS,LED_FLASH_TICKS,LedRgb::RED,LedRgb::GREEN,true);
	}
}

void MulAtmegaAtm::poll(unsigned char ticksPassed)
{
	static unsigned char l = 0;
	if(ticksPassed>0)
	{
		//for(unsigned char i=0;i<3;++i)
		//{
		//hardware_.getLedCircular(i).select(l>>4);
		//hardware_.getLedRgb(i).setColour((LedRgb::LedRgbColour)(l>>6));
		//}
		//hardware_.getLedRgb(MulAtmegaHardware::RGB_BANK).setColour((LedRgb::LedRgbColour)(l>>6));
		//hardware_.getLed(MulAtmegaHardware::LED_OCTDOWN).setState((bool)(l>>7));
		//hardware_.getLed(MulAtmegaHardware::LED_OCTUP).setState((bool)(1-(l>>7)));
		//l++;
	}
	hardware_.pollMidi();
	hardware_.pollAnlControls(ticksPassed);
	hardware_.pollSwitches(ticksPassed);
	hardware_.pollRotEncoders(ticksPassed);
	hardware_.pollI2cBuffer(ticksPassed);
	hardware_.refreshTick(ticksPassed);
	hardware_.refreshFlash(ticksPassed);
	//hardware_.refreshLcd();
	hardware_.refreshExpanders();
	engine_.poll(ticksPassed);
}

bool MulAtmegaAtm::isShiftHold(MulAtmegaAtmEngine::Func func)
{
	unsigned char i = func>>3;
	return (bool)bitRead(IS_SHIFT_HOLD[i],func-(i*8));
}

bool MulAtmegaAtm::isFuncFill(MulAtmegaAtmEngine::Func func) const
{
	unsigned char i = func>>3;
	return (bool)bitRead(IS_FUNC_FILL[i],func-(i*8));
}

//***********engine events*********************

void MulAtmegaAtm::engineVoiceChanged(unsigned char voice)
{
	//if(voice==VOICES_ALL)
	//{
	//hardware_.getLedCircular(MulAtmegaHardware::RE_GLOBAL).select(0);
	//}
	//else
	//{
	//hardware_.getLedCircular(MulAtmegaHardware::RE_GLOBAL).select((unsigned char)voice+1);
	//}
	hardware_.getRotEncoder(MulAtmegaHardware::RE_GLOBAL).setValue((char)voice);
	hardware_.getLedCircular(MulAtmegaHardware::RE_GLOBAL).select((unsigned char)voice);
}
void MulAtmegaAtm::engineFunctionChanged(unsigned char func)
{
	hardware_.getRotEncoder(MulAtmegaHardware::RE_FUNCTION).setValue((char)func);
	hardware_.getLedCircular(MulAtmegaHardware::RE_FUNCTION).select((unsigned char)func);
}

void MulAtmegaAtm::engineValueChanged(unsigned char val)
{
	hardware_.getRotEncoder(MulAtmegaHardware::RE_VALUE).setValue((char)val);
	if(isFuncFill(engine_.getFunction())==true)
	{
		hardware_.getRotEncoder(MulAtmegaHardware::RE_VALUE).setContinuous(false);
		hardware_.getLedCircular(MulAtmegaHardware::RE_VALUE).fill(val);
	}
	else
	{
		hardware_.getRotEncoder(MulAtmegaHardware::RE_VALUE).setContinuous(true);
		hardware_.getLedCircular(MulAtmegaHardware::RE_VALUE).select(val);
	}
}
void MulAtmegaAtm::engineOptionChanged(bool opt)
{
	if(opt==true)
	{
		hardware_.getLedRgb(MulAtmegaHardware::RGB_FUNCTION).setColour(LedRgb::GREEN);
	}
	else
	{
		hardware_.getLedRgb(MulAtmegaHardware::RGB_FUNCTION).setColour(LedRgb::RED);
	}
}
void MulAtmegaAtm::engineBankChanged(unsigned char bank)
{
	hardware_.getLedRgb(MulAtmegaHardware::RGB_BANK).flashStop();
	if(bank==HIGH)
	{
		hardware_.getLedRgb(MulAtmegaHardware::RGB_BANK).setColour(LedRgb::GREEN);
	}
	else
	{
		hardware_.getLedRgb(MulAtmegaHardware::RGB_BANK).setColour(LedRgb::RED);
	}
}
void MulAtmegaAtm::engineMidiTransmit(unsigned char data)
{
	hardware_.getLedRgb(MulAtmegaHardware::RGB_LCD).setColour(data>>4);
	//hardware_.txMidi(data);
	
}
void MulAtmegaAtm::engineSlaveTransmit(unsigned char id, unsigned char * data, size_t length)
{
	while (hardware_.queueI2cMessage(id,data,length)==false)
	{
		hardware_.getLed(MulAtmegaHardware::LED_OCTUP).setState(true);
		hardware_.getLed(MulAtmegaHardware::LED_OCTDOWN).setState(true);
	}
	//hardware_.txSlave(id,data,length);
	//hardware_.setVoiceEnable(0,MulAtmegaHardware::AUDCH_LEFT,!hardware_.getVoiceEnable(0,MulAtmegaHardware::AUDCH_LEFT));
}
void MulAtmegaAtm::engineSysexComplete()
{
	hardware_.getLedRgb(MulAtmegaHardware::RGB_VALUE).flash(4,LED_FLASH_TICKS,LED_FLASH_TICKS,LedRgb::RED,LedRgb::GREEN,true);
}
void MulAtmegaAtm::engineVoiceTriggered(unsigned char voice, bool on)
{
	hardware_.getLedCircular(MulAtmegaHardware::RE_GLOBAL).setSegment(voice + 8,on);
	if(on)
	{
		curVoice_ = voice;
	}
}
//**************************hardware events************************************
void MulAtmegaAtm::hardwareAnalogueControlChanged(unsigned char control, unsigned char newValue)
{
	//hardware_.getLedCircular(control % 3).fill(newValue >> 4);
	if(control<=6)
	{
		engine_.getPatchPtr()->setCtrlValue(engine_.getVoice(),engine_.getBank(),control,newValue);
	}
	else
	{
		//global fx
	}
}

void MulAtmegaAtm::hardwareSwitchChanged(unsigned char sw, unsigned char newValue)
{
	bool opt;

	//switch (sw)
	//{
	//case MulAtmegaHardware::SW_FUNCTION:
	//case MulAtmegaHardware::SW_VALUE:
	//case MulAtmegaHardware::SW_GLOBAL:
	//case MulAtmegaHardware::SW_BANK:
	//if (newValue==HIGH)
	//{
	//hardware_.getLedRgb(sw).setColour(LedRgb::GREEN);
	//}
	//else
	//{
	//hardware_.getLedRgb(sw).setColour(LedRgb::RED);
	//}
	//break;
	//case MulAtmegaHardware::SW_OCTDOWN:
	//case MulAtmegaHardware::SW_OCTUP:
	//hardware_.getLed(sw).setState(newValue);
	//break;
	//default:
	//hardware_.getLedRgb(MulAtmegaHardware::RGB_LCD).setColour((LedRgb::LedRgbColour)sw-MulAtmegaHardware::SW_LEFT);
	//break;
	//}
	//return;

	switch (sw)
	{
		case MulAtmegaHardware::SW_FUNCTION:
		if(isShiftHold(engine_.getFunction())==true)
		{
			if(newValue==HIGH)
			{
				hardware_.getLedRgb(MulAtmegaHardware::RGB_FUNCTION).setColour(LedRgb::GREEN);
			}
			else
			{
				hardware_.getLedRgb(MulAtmegaHardware::RGB_FUNCTION).flashStop();
				hardware_.getLedRgb(MulAtmegaHardware::RGB_FUNCTION).setColour(LedRgb::RED);
				if(hardware_.getSwitch(sw).getHoldTime()>MulAtmegaHardware::HOLD_EVENT_TICKS)
				{
					switch(engine_.getFunction())
					{
						case MulAtmegaAtmEngine::FUNC_AENVA:
						//engine_.writeSysexPatch(engine_.getPatchPtr()->getFunctionValue(MulAtmegaAtmEngine::FUNC_MEM));
						break;
						case MulAtmegaAtmEngine::FUNC_AENVR:
						//engine_.writeSysexMemDump();
						break;
						//case MulAtmegaAtmEngine::FUNC_MEM:
						//engine_.getPatchPtr()->writePatch(engine_.getPatchPtr()->getFunctionValue(MulAtmegaAtmEngine::FUNC_MEM));
						//break;
					}
				}
				else
				{
					switch(engine_.getFunction())
					{
						//case MulAtmegaAtmEngine::FUNC_MEM:
						//engine_.getPatchPtr()->readPatch(engine_.getPatchPtr()->getFunctionValue(MulAtmegaAtmEngine::FUNC_MEM));
						//break;
					}
				}
			}
		}
		else if(newValue==HIGH)
		{
			opt = !engine_.getPatchPtr()->getOptionValue(engine_.getVoice(), engine_.getFunction());
			engine_.getPatchPtr()->setOptionValue(engine_.getVoice(),engine_.getFunction(),opt);
			if(opt==true)
			{
				hardware_.getLedRgb(MulAtmegaHardware::RGB_FUNCTION).setColour(LedRgb::GREEN);
			}
			else
			{
				hardware_.getLedRgb(MulAtmegaHardware::RGB_FUNCTION).setColour(LedRgb::RED);
			}
		}
		break;
		case MulAtmegaHardware::SW_VALUE:
		if(newValue==HIGH)
		{
			hardware_.getRotEncoder(MulAtmegaHardware::RE_VALUE).setUpdateVal(false);
			hardware_.getLedRgb(MulAtmegaHardware::RGB_VALUE).setColour(LedRgb::GREEN);
			engine_.midiNoteOnReceived(testNote_,127);
		}
		else
		{
			hardware_.getRotEncoder(MulAtmegaHardware::RE_VALUE).setUpdateVal(true);
			hardware_.getLedRgb(MulAtmegaHardware::RGB_VALUE).setColour(LedRgb::RED);
			engine_.midiNoteOffReceived(testNote_);
		}
		break;
		case MulAtmegaHardware::SW_GLOBAL:
		if(newValue==HIGH)
		{
			engine_.setMono(!engine_.getMono());
			if(engine_.getMono()==true)
			{
				hardware_.getLedRgb(MulAtmegaHardware::RGB_GLOBAL).setColour(LedRgb::GREEN);
			}
			else
			{
				hardware_.getLedRgb(MulAtmegaHardware::RGB_GLOBAL).setColour(LedRgb::RED);
			}
		}
		break;
		case MulAtmegaHardware::SW_BANK:
		if(newValue==HIGH)
		{
			engine_.setBank(HIGH-engine_.getBank());
		}
		break;
		case MulAtmegaHardware::SW_OCTDOWN:
		if(newValue==HIGH && hardware_.getSwitch(MulAtmegaHardware::SW_BANK).getState()==HIGH)
		{
			engine_.txOsccalib(curVoice_,false);
		}
		hardware_.getLed(MulAtmegaHardware::LED_OCTDOWN).setState(newValue);
		break;
		case MulAtmegaHardware::SW_OCTUP:
		if(newValue==HIGH && hardware_.getSwitch(MulAtmegaHardware::SW_BANK).getState()==HIGH)
		{
			engine_.txOsccalib(curVoice_,true);
		}
		hardware_.getLed(MulAtmegaHardware::LED_OCTUP).setState(newValue);
		break;
		default:
		hardware_.getLedRgb(MulAtmegaHardware::RGB_LCD).setColour((LedRgb::LedRgbColour)sw-MulAtmegaHardware::SW_LEFT);
		hardware_.demoLcd(sw);
		break;
	}
}
void MulAtmegaAtm::hardwareSwitchHeld(unsigned char sw)
{

	if(sw==MulAtmegaHardware::SW_FUNCTION && isShiftHold(engine_.getFunction())==true)
	{
		hardware_.getLedRgb(MulAtmegaHardware::RGB_FUNCTION).flash(8,LED_FLASH_TICKS,LED_FLASH_TICKS,LedRgb::GREEN,LedRgb::RED,true);
	}
	if(sw==MulAtmegaHardware::SW_BANK)
	{
		if(engine_.getBank()==LOW)
		{
			hardware_.getLedRgb(MulAtmegaHardware::RGB_BANK).flash(8,LED_FLASH_TICKS,LED_FLASH_TICKS,LedRgb::RED,LedRgb::YELLOW,true);
		}
		else
		{
			hardware_.getLedRgb(MulAtmegaHardware::RGB_BANK).flash(8,LED_FLASH_TICKS,LED_FLASH_TICKS,LedRgb::GREEN,LedRgb::YELLOW,true);
		}
		//engine_.getPatchPtr()->easterEgg(engine_.getMasterClock().getOutput());
	}
}
void MulAtmegaAtm::hardwareRotaryEncoderChanged(unsigned char rotary, unsigned char newValue, bool clockwise)
{

	//hardware_.getLedCircular(rotary).select(newValue);
	//return;

	if(rotary==MulAtmegaHardware::RE_GLOBAL)
	{
		engine_.setVoice(newValue);
	}
	else if(rotary==MulAtmegaHardware::RE_FUNCTION)
	{
		engine_.setFunction((MulAtmegaAtmEngine::Func)newValue);
	}
	else
	{
		if(hardware_.getSwitch(MulAtmegaHardware::SW_VALUE).getState()==HIGH)
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
			engine_.getPatchPtr()->setFunctionValue(engine_.getVoice(),engine_.getFunction(),newValue);
		}
	}

}
void MulAtmegaAtm::hardwareMidiChannelChanged(unsigned char channel)
{
	engine_.getMidiPtr()->setChannel(channel);
}
void MulAtmegaAtm::hardwareMidiReceived(unsigned char data)
{
	//hardware_.getLedMidi().flash(1,LED_FLASH_TICKS,LED_FLASH_TICKS,true);  //do at this level, not in hardware
	engine_.getMidiPtr()->read(data);
}
void MulAtmegaAtm::hardwareMidiError(unsigned char errorType)
{
	//hardware_.getLedSwitch(MulAtmegaHardware::VALUE).setColour((LedRgb::LedRgbColour)(3-(unsigned char)hardware_.getLedSwitch(MulAtmegaHardware::VALUE).getColour()));
	hardware_.getLedCircular(MulAtmegaHardware::RE_VALUE).fill(errorType);
}