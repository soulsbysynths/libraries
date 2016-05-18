//Min.cpp  miniAtmegatron class (add this object to Arduino sketch)
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

#include "Min.h"


// default constructor
Min::Min() : engine_(MinEngine::getInstance()), hardware_(MinHardware::getInstance())
{
	engine_.construct(this);
	hardware_.construct(this);
} //Min

// default destructor
Min::~Min()
{
} //~Min

void Min::initialize()
{
	unsigned char i;
	hardware_.beginMidi(MIDI_UBRR);
	for(i=0;i<2;++i)
	{
		hardware_.getLed(i).setColour(LedRgb::RED);
	}
	hardware_.refreshLEDs();
	engine_.initialize();
	if(hardware_.getFirstBoot()==true)
	{
		engine_.initPatch();
	}
	engine_.setFunction(MinEngine::FUNC_PATTERN);
	engine_.setBank(hardware_.getSwitch(MinHardware::SW_BANK).getState());
}

void Min::poll(unsigned char ticksPassed)
{
	hardware_.pollMidi();
	hardware_.pollAnlControls(ticksPassed);
	hardware_.pollSwitches(ticksPassed);
	hardware_.refreshFlash(ticksPassed);
	refreshValueLed();
	hardware_.refreshLEDs();
	engine_.poll(ticksPassed);
}
//***********engine events*********************
void Min::engineValueChanged(unsigned char func, unsigned char val, bool opt)
{
	bool col;
	hardware_.getLed(MinHardware::LED_FUNC).setColour((LedRgb::LedRgbColour)func);
}
void Min::engineBankChanged(unsigned char bank)
{
	//shown physically on switch
}
void Min::engineMidiTransmit(unsigned char data)
{
	hardware_.writeMidi(data);
}

void Min::refreshValueLed()
{
	unsigned char step = (engine_.getMasterClock().getOutput()%4096)>>8;
	unsigned char func = (unsigned char)engine_.getFunction();
	unsigned char val = engine_.getPatchPtr()->getFunctionValue(func);
	if(step%4==0)
	{
		if(val>=step)
		{
			hardware_.getLed(MinHardware::LED_VALUE).setColour(LedRgb::YELLOW);
		}
		else
		{
			hardware_.getLed(MinHardware::LED_VALUE).setColour(LedRgb::GREEN);
		}
	}
	else if(step%2==0)
	{
		if(val>=step)
		{
			hardware_.getLed(MinHardware::LED_VALUE).setColour(LedRgb::GREEN);
		}
		else
		{
			hardware_.getLed(MinHardware::LED_VALUE).setColour(LedRgb::OFF);
		}
	}
	else
	{
		if(val>=step)
		{
			hardware_.getLed(MinHardware::LED_VALUE).setColour(LedRgb::RED);
		}
		else
		{
			hardware_.getLed(MinHardware::LED_VALUE).setColour(LedRgb::OFF);
		}
	}

}


//**************************hardware events************************************
void Min::hardwareAnalogueControlChanged(unsigned char control, unsigned char newValue)
{
	engine_.getPatchPtr()->setCtrlValue(engine_.getBank(),control,newValue);
}

void Min::hardwareSwitchChanged(unsigned char switch_, unsigned char newValue)
{
	bool opt;
	unsigned char func = (unsigned char)engine_.getFunction();
	unsigned char val;
	
	if(newValue==HIGH)
	{
		if(switch_==MinHardware::SW_FUNC_DEC)
		{
			if(func==0)
			{
				engine_.setFunction(MinEngine::FUNC_PATTERN);
			}
			else
			{
				engine_.setFunction((MinEngine::Func)(func-1));
			}
		}

		if(switch_==MinHardware::SW_FUNC_INC)
		{
			if(func==7)
			{
				engine_.setFunction(MinEngine::FUNC_WAVE);
			}
			else
			{
				engine_.setFunction((MinEngine::Func)(func+1));
			}
		}

		if(switch_==MinHardware::SW_VAL_DEC)
		{
			if(hardware_.getSwitch(MinHardware::SW_FUNC_INC).getState()==HIGH)
			{
				val = engine_.getPatchPtr()->getFunctionValue(MinEngine::FUNC_BPM);
				if(val>0)
				{
					val--;
					engine_.getPatchPtr()->setFunctionValue(MinEngine::FUNC_BPM,val);
				}
			}
			else
			{
				val = engine_.getPatchPtr()->getFunctionValue((MinEngine::Func)func);
				if(val==0)
				{
					engine_.getPatchPtr()->setFunctionValue(func,15);
				}
				else
				{
					engine_.getPatchPtr()->setFunctionValue(func,val-1);
				}				
			}
		}

		if(switch_==MinHardware::SW_VAL_INC)
		{
			if(hardware_.getSwitch(MinHardware::SW_FUNC_INC).getState()==HIGH)
			{
				val = engine_.getPatchPtr()->getFunctionValue(MinEngine::FUNC_BPM);
				if(val<15)
				{
					val++;
					engine_.getPatchPtr()->setFunctionValue(MinEngine::FUNC_BPM,val);
				}	
			}
			else
			{
				val = engine_.getPatchPtr()->getFunctionValue((MinEngine::Func)func);
				if(val==15)
				{
					engine_.getPatchPtr()->setFunctionValue(func,0);
				}
				else
				{
					engine_.getPatchPtr()->setFunctionValue(func,val+1);
				}				
			}
		}
	}

	if(switch_==MinHardware::SW_PLAY)
	{
		if(newValue==HIGH)
		{
			engine_.midiNoteOnReceived(engine_.getTestNote(),127);
		}
		else
		{
			engine_.getMidiPtr()->reset();
		}
	}
	
	if(switch_==MinHardware::SW_BANK)
	{
		engine_.setBank(newValue);
	}
}
void Min::hardwareSwitchHeld(unsigned char switch_)
{
	unsigned char col = (unsigned char)hardware_.getLed(MinHardware::LED_FUNC).getColour();
	unsigned char invcol = ~col & 0x07;
	if(switch_==MinHardware::SW_FUNC_DEC)
	{
		engine_.getPatchPtr()->writePatch(PATCH_NUM);
		hardware_.getLed(MinHardware::LED_FUNC).flash(4,FLASH_TICKS,FLASH_TICKS,(LedRgb::LedRgbColour)col,(LedRgb::LedRgbColour)invcol,true);
		hardware_.setFirstBoot(false);
	}
}

void Min::hardwareMidiReceived(unsigned char data)
{
	engine_.getMidiPtr()->read(data);
}