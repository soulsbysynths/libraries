/*
* ArpAtarp.cpp
*
* Created: 12/04/2017 20:08:39
* Author: paulsoulsby
*/


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
	engine_.initialize();
	for (unsigned char i=0;i<ArpAtarpEngine::FUNCS;++i)
	{
		val = hardware_.readEepromByte(i+EEPROM_PATCH_OFFSET);
		if(val>FUNC_MASK[i])
		{
			val = FUNC_MASK[i];
		}
		engine_.setValue(i,val);
	}
}

void ArpAtarp::poll(unsigned char ticksPassed)
{
	hardware_.pollMidi();
	hardware_.pollCalibCvInputs();
	hardware_.pollSwitches(ticksPassed);
	hardware_.pollDigiPins(ticksPassed);
	hardware_.refreshFlash(ticksPassed);
	hardware_.refreshLeds();
	engine_.poll(ticksPassed);
	if(hardware_.getJackDetect(ArpHardware::CV_CLOCK)==true && engine_.getMidiPtr()->getClockRunning()==true)
	{
		gateTimeOut_++;
		if(gateTimeOut_>150)
		{
			engine_.getMidiPtr()->read(Midi::STOP);
		}
	}

}
void ArpAtarp::refreshNoteSource()
{
	ArpAtarpEngine::NoteSource ns;
	if(engine_.getFunction()==ArpAtarpEngine::FUNC_RECORD || (hardware_.getJackDetect(ArpHardware::CV_ROOT)==false && hardware_.getJackDetect(ArpHardware::CV_CHORD)==false))
	{
		ns = ArpAtarpEngine::NS_MIDI;
	}
	else
	{
		ns = ArpAtarpEngine::NS_CV;
	}
	if(engine_.getValue(ArpAtarpEngine::FUNC_NOTESOURCE)!=ns)
	{
		engine_.setValue(ArpAtarpEngine::FUNC_NOTESOURCE,ns);
	}
}

//*********** HARDWARE EVENTS ************
void ArpAtarp::hardwareCvInputChanged(unsigned char input, unsigned int newValue)
{
	if(engine_.getValue(ArpAtarpEngine::FUNC_NOTESOURCE)==ArpAtarpEngine::NS_MIDI)
	{
		return;
	}
	unsigned long midiNote = newValue + 34;  //want to round 50cent below and above to midi note.  Therefore add 50cent.  68.26666667 per note = 34
	midiNote = (midiNote * 15) >> 10;  // = (CV * 12) / (4096 / 5)
	if(input==ArpHardware::CV_ROOT)
	{
		if(engine_.getChordGen().getChordNote()==0)
		{
			hardware_.setOutput(newValue);
		}
		else
		{
			midiNote += 36;
			if(midiNote!=engine_.getChordGen().getRootNote())
			{
				engine_.getChordGen().setRootNote(midiNote);
				engine_.txChordGenMidi();
			}
		}
	}
	else
	{
		if(midiNote!=engine_.getChordGen().getChordNote())
		{
			engine_.getChordGen().setChordNote(midiNote);
			engine_.txChordGenMidi();
			if(midiNote==0)
			{
				hardware_.setOutput(hardware_.getCvInput(ArpHardware::CV_ROOT).getValue());
			}
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
		if(newValue==true)
		{
			engine_.setValue(ArpAtarpEngine::FUNC_CLOCKSOURCE,ArpAtarpEngine::CS_EXTERNAL);
		}
		else
		{
			engine_.setValue(ArpAtarpEngine::FUNC_CLOCKSOURCE,ArpAtarpEngine::CS_INTERNAL);
		}
		break;
		case ArpHardware::CV_CHORD:
		case ArpHardware::CV_ROOT:
		refreshNoteSource();
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
	if(engine_.getValue(ArpAtarpEngine::FUNC_NOTESOURCE)==ArpAtarpEngine::NS_CV && data<Midi::CLOCK)
	{
		return;
	}
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
					if(val==FUNC_MASK[func])
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
						val = FUNC_MASK[func];
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
	refreshNoteSource();
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
			hardware_.getLed().displayHex(newValue);
			break;
			case ArpAtarpEngine::FUNC_NOTESOURCE:
			if(newValue==0)
			{
				hardware_.getLed().displayChar('M');
			}
			else
			{
				hardware_.getLed().displayChar('C');
			}
			break;
			case ArpAtarpEngine::FUNC_CLOCKSOURCE:
			if(newValue==0)
			{
				hardware_.getLed().displayChar('I');
			}
			else
			{
				hardware_.getLed().displayChar('E');
			}
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
