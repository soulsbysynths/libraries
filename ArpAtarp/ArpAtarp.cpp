//ArpAtarp.h  AT-ARP main class
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

#include "ArpAtarp.h"

// default constructor
ArpAtarp::ArpAtarp() : engine_(ArpAtarpEngine::getInstance()), hardware_(ArpHardware::getInstance())
{
	hardware_.construct(this);
	engine_.construct(this);
} //ArpAtarp

// default destructor
ArpAtarp::~ArpAtarp()
{
} //~ArpAtarp

void ArpAtarp::init()
{
	unsigned char val;
	hardware_.beginMidi(MIDI_UBRR);
	hardware_.beginTick();
	engine_.initialize();
	for (unsigned char i=0;i<ArpAtarpEngine::FUNCS;++i)
	{
		val = hardware_.readEepromByte(i+EEPROM_PATCH_OFFSET);
		if(val>FUNC_MAX[i])
		{
			val = FUNC_MAX[i];
		}
		engine_.setValue(i,val);
	}
}

void ArpAtarp::poll()
{
	unsigned char ticksPassed = hardware_.pollTickCount();
	hardware_.pollSwitches(ticksPassed);
	hardware_.pollDigiPins(ticksPassed);
	hardware_.refreshFlash(ticksPassed);
	hardware_.refreshLeds();
	if(hardware_.getJackDetect(ArpHardware::CV_CLOCK)==true && engine_.getMidiPtr()->getClockRunning()==true)
	{
		gateTimeOut_++;
		if(gateTimeOut_>150)
		{
			engine_.getMidiPtr()->read(Midi::STOP);
		}
	}
	hardware_.pollMidi();
	hardware_.pollCalibCvInputs();
	engine_.poll(hardware_.pollMidiCount());
}

//*********** HARDWARE EVENTS ************
void ArpAtarp::hardwareCvInputChanged(unsigned char input, unsigned int newValue)
{
	unsigned long midiNote = newValue + 34;  //want to round 50cent below and above to midi note.  Therefore add 50cent.  68.26666667 per note = 34
	midiNote = (midiNote * 15) >> 10;  // = (CV * 12) / (4096 / 5)
	if(input==ArpHardware::CV_ROOT)
	{
		if(engine_.getRootSource()==ArpAtarpEngine::IS_CV)
		{
			engine_.setRootNote(midiNote + ArpAtarpEngine::ROOT_NOTE_OFFSET);
		}
	}
	else
	{
		if (engine_.getChordSource()==ArpAtarpEngine::IS_CV)
		{
			engine_.setChordNote(midiNote);
		}
	}
}
void ArpAtarp::hardwareGateInputChanged(bool newValue)
{
	if(newValue==true)
	{
		if(engine_.getMidiPtr()->getClockRunning()==false)
		{
			engine_.getMidiPtr()->read(Midi::START);
		}
		engine_.getMidiPtr()->read(Midi::CLOCK);
		gateTimeOut_ = 0;
	}
}
void ArpAtarp::hardwareJackDetectChanged(unsigned char jack, bool newValue)
{
	switch(jack)
	{
		case ArpHardware::CV_CLOCK:
		// nothing to do here
		break;
		case ArpHardware::CV_CHORD:
		if (newValue==true)
		{
			engine_.setChordSource(ArpAtarpEngine::IS_CV);
		}
		else
		{
			engine_.setChordSource(ArpAtarpEngine::IS_MIDI);
		}
		break;
		case ArpHardware::CV_ROOT:
		if (newValue==true)
		{
			engine_.setRootSource(ArpAtarpEngine::IS_CV);
		}
		else
		{
			engine_.setRootSource(ArpAtarpEngine::IS_MIDI);
		}
		break;
	}
}
void ArpAtarp::hardwareMidiChannelChanged(unsigned char channel)
{
	engine_.getMidiPtr()->setChannel(channel);
}
void ArpAtarp::hardwareMidiError(unsigned char errorType)
{

}
void ArpAtarp::hardwareMidiReceived(unsigned char data)
{
	engine_.getMidiPtr()->read(data);
}
void ArpAtarp::hardwareSwitchChanged(unsigned char sw, unsigned char newValue)
{
	if(newValue==LOW)
	{
		if(swHoldBlock_==true)
		{
			swHoldBlock_ = false;
		}
		else
		{
			unsigned char func = engine_.getFunction();
			if(funcSelect_)
			{
				if(sw==ArpHardware::SW_UP)
				{
					if((engine_.getValue(ArpAtarpEngine::FUNC_PATTERN)<=0x0F && func>=ArpAtarpEngine::FUNCS-3) || func>=ArpAtarpEngine::FUNCS-1)
					{
						func = 0;
					}
					else
					{
						func++;
					}
				}
				else
				{
					if(func==0)
					{
						if(engine_.getValue(ArpAtarpEngine::FUNC_PATTERN)<=0x0F)
						{
							func = ArpAtarpEngine::FUNCS-3;
						}
						else
						{
							func = ArpAtarpEngine::FUNCS-1;
						}
					}
					else
					{
						func--;
					}
				}
				engine_.setFunction(func);
			}
			else
			{
				unsigned char val = engine_.getValue(func);
				if(sw==ArpHardware::SW_UP)
				{
					if(val==FUNC_MAX[func])
					{
						val = 0;
					}
					else
					{
						val++;
					}
					
				}
				else
				{
					if(val==0)
					{
						val = FUNC_MAX[func];
					}
					else
					{
						val--;
					}
				}
				engine_.setValue(func,val);
				hardware_.writeEepromByte((unsigned int)func + EEPROM_PATCH_OFFSET, val);
			}
		}
	}
}
void ArpAtarp::hardwareSwitchHeld(unsigned char sw)
{
	swHoldBlock_ = true;
	funcSelect_ = !funcSelect_;
	if(funcSelect_==true)
	{
		engineFunctionChanged(engine_.getFunction());
	}
	else
	{
		engineValueChanged(engine_.getValue(engine_.getFunction()));
	}
}
//***************ENGINE EVENTS ********************
void ArpAtarp::engineOutputChanged(unsigned int newOutput)
{
	hardware_.setOutput(newOutput);
	hardware_.getLed().flash(true,1,LED_SHORT_FLASH_TICKS,LED_SHORT_FLASH_TICKS,true);
}
void ArpAtarp::engineFunctionChanged(unsigned char newFunction)
{
	if(funcSelect_)
	{
		hardware_.getLed().displayChar(FUNC_DISPLAY[newFunction]);
		hardware_.getLed().flash(false,255,LED_FLASH_TICKS,LED_FLASH_TICKS,true);
	}
	else
	{
		engineValueChanged(engine_.getValue(newFunction));
	}
}
void ArpAtarp::engineValueChanged(unsigned char newValue)
{
	if(!funcSelect_)
	{
		hardware_.getLed().flashStop(false);
		switch(engine_.getFunction())
		{
			case ArpAtarpEngine::FUNC_PATTERN:
			hardware_.getLed().displayHex(newValue & 0x0F);
			if(newValue>0x0F)
			{
				hardware_.getLed().setInvertDp(true);
			}
			else
			{
				hardware_.getLed().setInvertDp(false);
			}
			break;
			case ArpAtarpEngine::FUNC_RECORD:
			hardware_.getLed().displayTapeRoll(newValue);
			break;
			case ArpAtarpEngine::FUNC_RESET:
			case ArpAtarpEngine::FUNC_PINGPONG:
			if(newValue==0)
			{
				hardware_.getLed().displayChar('N');
			}
			else
			{
				hardware_.getLed().displayChar('Y');
			}
			break;
			case ArpAtarpEngine::FUNC_DIV:
			case ArpAtarpEngine::FUNC_MIDICHANNEL:
			case ArpAtarpEngine::FUNC_CVOCTAVE:
			hardware_.getLed().displayHex(newValue);
			break;
			case ArpAtarpEngine::FUNC_CLOCKPPQN:
			hardware_.getLed().displayHex(PPQN_DISPLAY[newValue]);
			hardware_.setMidiClksOutPerPulse(48 / PPQN_VALUE[newValue]);
			break;
		}
	}
}
void ArpAtarp::engineLoadUserPattern(unsigned char patternNum)
{
	unsigned int addr = patternNum * 16;
	for(unsigned char i=0;i<16;++i)
	{
		engine_.getArpeggiatorPtr()->setUserPattern(i,hardware_.readEepromByte(addr));
		addr++;
	}
}
void ArpAtarp::engineSaveUserPatternStep(unsigned char patternNum, unsigned char step, char value)
{
	unsigned int addr = (patternNum * 16) + step;
	hardware_.writeEepromByte(addr,value);
}
