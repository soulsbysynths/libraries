/*
* AteDrum.cpp
*
* Created: 05/04/2017 15:12:34
* Author: paulsoulsby
*/


#include "AteDrum.h"

// default constructor
AteDrum::AteDrum() : engine_(AteDrumEngine::getInstance()), hardware_(AteOscHardware::getInstance())
{
	engine_.construct(this);
	hardware_.construct(this);
} //AteDrum

// default destructor
AteDrum::~AteDrum()
{
} //~AteDrum

void AteDrum::initialize()
{
	hardware_.getRotEncoder(AteOscHardware::FUNCTION).setContinuous(true);
	hardware_.getRotEncoder(AteOscHardware::VALUE).setContinuous(true);
	hardware_.setInputMode(AteOscHardware::CV_CAPTURE, AteOscHardware::IP_GATE);
	engine_.initialize();
	engine_.getPatchPtr()->readPatch(hardware_.readEepromByte(AteOscHardware::EEPROM_CURRENT_PATCH) & 0x07);
	hardware_.getLedSwitch(AteOscHardware::FUNCTION).flash(4,LED_FLASH_SHORT_TICKS,LED_FLASH_SHORT_TICKS,LedRgb::RED,LedRgb::GREEN,true);
	//hardware_.getLedSwitch(AteOscHardware::VALUE).flash(4,LED_FLASH_TICKS,LED_FLASH_TICKS,LedRgb::GREEN,LedRgb::RED,true);
}

void AteDrum::poll(unsigned char ticksPassed)
{
	hardware_.pollCvInputs(ticksPassed);
	hardware_.pollGateInputs();
	hardware_.pollSwitches(ticksPassed);
	hardware_.pollRotEncoders(ticksPassed);
	//hardware_.pollAudioBufferStatus();
	hardware_.refreshFlash(ticksPassed);
	hardware_.refreshLeds();
	engine_.pollRunning(ticksPassed);
}

void AteDrum::readPatch(unsigned char patchNum)
{
	if(patchNum!=engine_.getPatchPtr()->getPatchNum())
	{
		hardware_.getLedSwitch(AteOscHardware::FUNCTION).flash(255,LED_FLASH_LONG_TICKS,LED_FLASH_LONG_TICKS,LedRgb::RED,LedRgb::GREEN,true);
		engine_.setBufferPatchRead(patchNum);
	}
}

void AteDrum::stopFunctionFlash()
{
	hardware_.getLedSwitch(AteOscHardware::FUNCTION).flashStop();
	hardware_.getLedSwitch(AteOscHardware::FUNCTION).setColour(LedRgb::RED);
}

//***********engine events*********************
void AteDrum::engineInstrChanged(unsigned char instr)
{
	refreshLeds(instr,engine_.getStep());
}

void AteDrum::engineStepChanged(unsigned char step)
{
	refreshLeds(engine_.getInstr(),step);
}
void AteDrum::refreshLeds(unsigned char instr, unsigned char step)
{
	hardware_.getLedCircular(AteOscHardware::FUNCTION).select(instr);
	hardware_.getLedCircular(AteOscHardware::VALUE).flashStopAll();
	hardware_.getLedCircular(AteOscHardware::VALUE).setState(engine_.getPatchPtr()->getPattern(instr));
	if(engine_.getPatchPtr()->getPatternStep(instr,step)==true)
	{
		hardware_.getLedSwitch(AteOscHardware::VALUE).setColour(LedRgb::GREEN);
		hardware_.getLedCircular(AteOscHardware::VALUE).flash(step,255,LED_FLASH_LONG_TICKS,LED_FLASH_SHORT_TICKS,true);
	}
	else
	{
		hardware_.getLedSwitch(AteOscHardware::VALUE).setColour(LedRgb::RED);
		hardware_.getLedCircular(AteOscHardware::VALUE).flash(step,255,LED_FLASH_SHORT_TICKS,LED_FLASH_LONG_TICKS,false);
	}
}
void AteDrum::enginePatternChanged(unsigned char step, bool newValue)
{
	if(step==engine_.getStep())
	{
		if(newValue==true)
		{
			hardware_.getLedSwitch(AteOscHardware::VALUE).setColour(LedRgb::GREEN);
			hardware_.getLedCircular(AteOscHardware::VALUE).flash(step,255,LED_FLASH_LONG_TICKS,LED_FLASH_SHORT_TICKS,true);
		}
		else
		{
			hardware_.getLedSwitch(AteOscHardware::VALUE).setColour(LedRgb::RED);
			hardware_.getLedCircular(AteOscHardware::VALUE).flash(step,255,LED_FLASH_SHORT_TICKS,LED_FLASH_LONG_TICKS,false);
		}
	}
	else
	{
		hardware_.getLedCircular(AteOscHardware::VALUE).setSegment(step,newValue);
	}
}

void AteDrum::enginePatchChanged(unsigned char patchNum)
{
	stopFunctionFlash();
	hardware_.writeEepromByte(AteOscHardware::EEPROM_CURRENT_PATCH,patchNum);
}

//**************************hardware events************************************
void AteDrum::hardwareCvInputChanged(unsigned char input, unsigned int newValue)
{
	if(input==AteOscHardware::CV_PITCH)
	{
		unsigned int midiNote = newValue + 34;  //want to round 50cent below and above to midi note.  Therefore add 50cent.  68.26666667 per note = 34
		midiNote = (midiNote * 15) >> 10;  // = (CV * 12) / (4096 / 5)
		engine_.setLinearFreq(0,midiNote << 6);  //64 cents per linear semitone
	}
	else if(input==AteOscHardware::CV_PITCHPOT)
	{
		engine_.setLinearFreq(1,(newValue>>5)<<5);
	}
	else if(input==AteOscHardware::CV_FILT)
	{
		static unsigned char lastAction = 255;
		unsigned int midiNote = newValue + 34;  //want to round 50cent below and above to midi note.  Therefore add 50cent.  68.26666667 per note = 34
		midiNote = (midiNote * 15) >> 10;  // = (CV * 12) / (4096 / 5)
		if(midiNote>8)  //0 = do nothing, 1-8 = load patch, 9 = reset
		{
			midiNote = 9;
		}
		if(lastAction!=midiNote)
		{
			lastAction = (unsigned char)midiNote;
			if(midiNote>0 && midiNote<=8)
			{
				readPatch(midiNote-1);
			}
			else if (midiNote==9 && engine_.getRunning())
			{
				engine_.resetPlayStep();
				engine_.setBufferPatchRead(255);
				stopFunctionFlash();
			}
		}
	}
	else
	{
		engine_.getPatchPtr()->setCtrlValue(engine_.getInstr(),(unsigned char)CV_INPUT_TO_CTRL[input],newValue>>4);
	}

}
void AteDrum::hardwareGateInputChanged(unsigned char input, bool newValue)
{
	unsigned char ps = engine_.getPlayStep();
	if(newValue==true)
	{
		ps++;
		ps &= 0x0F;
		engine_.setPlayStep(ps);
		if(ps!=engine_.getStep())
		{
			hardware_.getLedCircular(AteOscHardware::VALUE).flash(ps,1,LED_FLASH_SHORT_TICKS, LED_FLASH_SHORT_TICKS,!engine_.getPatchPtr()->getPatternStep(engine_.getInstr(),ps));
		}
	}

}
void AteDrum::hardwareSwitchChanged(unsigned char sw, unsigned char newValue)
{
	unsigned char i = engine_.getInstr();
	unsigned char s = engine_.getStep();
	bool funcHeld = ((hardware_.getSwitch(AteOscHardware::FUNCTION).getHoldTime()>AteOscHardware::HOLD_EVENT_TICKS) ? true : false);
	if(sw==AteOscHardware::FUNCTION)
	{
		if(newValue==HIGH)
		{
			hardware_.getLedSwitch(AteOscHardware::FUNCTION).setColour(LedRgb::GREEN);
		}
		else
		{
			if(funcHeld)//mem stuff
			{
				stopFunctionFlash();
				engine_.getPatchPtr()->writePatch(engine_.getInstr());
				hardware_.writeEepromByte(AteOscHardware::EEPROM_CURRENT_PATCH,engine_.getInstr());
			}
			else
			{
				readPatch(engine_.getInstr());
			}
		}
	}
	funcHeld = ((hardware_.getSwitch(AteOscHardware::VALUE).getHoldTime()>AteOscHardware::HOLD_EVENT_TICKS) ? true : false);
	if (sw==AteOscHardware::VALUE)
	{
		if(newValue==HIGH)
		{
			hardware_.getLedSwitch(AteOscHardware::VALUE).setColour(LedRgb::GREEN);
		}
		else
		{
			unsigned char i = engine_.getInstr();
			unsigned char s = engine_.getStep();
			if (funcHeld)
			{
				hardware_.getLedSwitch(AteOscHardware::VALUE).flashStop();
				if(hardware_.getSwitch(AteOscHardware::FUNCTION).getState()==HIGH)
				{
					engine_.resetPlayStep();  //ready to play 0 on next edge
				}
				else
				{
					engine_.getPatchPtr()->clearPattern();
				}
				refreshLeds(engine_.getInstr(),engine_.getStep());
			}
			else
			{

				engine_.getPatchPtr()->setPatternStep(i,s,!engine_.getPatchPtr()->getPatternStep(i,s));
			}
		}
	}
}

void AteDrum::hardwareSwitchHeld(unsigned char sw)
{
	hardware_.getLedSwitch(sw).flash(8,LED_FLASH_SHORT_TICKS,LED_FLASH_SHORT_TICKS,LedRgb::GREEN,LedRgb::RED,true);
}
void AteDrum::hardwareRotaryEncoderChanged(unsigned char rotary, unsigned char newValue, bool clockwise)
{
	if(rotary==AteOscHardware::FUNCTION)
	{
		engine_.setInstr(newValue);
	}

	if(rotary==AteOscHardware::VALUE)
	{
		engine_.setStep(newValue);
	}
}

void AteDrum::hardwareAudioBufferStatusChanged(unsigned char newStatus)
{

	//switch (newStatus)
	//{
	//case AteOscHardware::BUFFER_WAITZCROSS:
	//hardware_.getLedSwitch(AteOscHardware::VALUE).setColour(LedRgb::YELLOW);
	//break;
	//case AteOscHardware::BUFFER_CAPTURING:
	//hardware_.getLedSwitch(AteOscHardware::VALUE).setColour(LedRgb::RED);
	//break;
	//case AteOscHardware::BUFFER_OVERFLOW:
	//hardware_.getLedSwitch(AteOscHardware::VALUE).setColour(LedRgb::RED);
	//if(engine_.getPatchPtr()->getOptionValue(AteOscEngine::FUNC_MINLENGTH))
	//{
	//hardware_.setAudioBufferStatus(AteOscHardware::BUFFER_WAITZCROSS);
	//}
	//break;
	//case AteOscHardware::BUFFER_IDLE:
	//hardware_.getLedSwitch(AteOscHardware::VALUE).setColour(LedRgb::GREEN);
	//if(engine_.getPatchPtr()->getOptionValue(AteOscEngine::FUNC_MINLENGTH))
	//{
	//hardware_.setAudioBufferStatus(AteOscHardware::BUFFER_WAITZCROSS);
	//}
	//break;
	//case AteOscHardware::BUFFER_CAPTURED:
	//hardware_.getLedSwitch(AteOscHardware::VALUE).setColour(LedRgb::GREEN);
	////see excel sheet for proof of this interpolation
	//unsigned int pos = 0;
	//unsigned int jump = (unsigned int)hardware_.getAudioBufferLength() << 1;  //0-255
	//for(unsigned char i=0;i<engine_.getOscillator().getWaveLength();++i)
	//{
	//engine_.getOscillator().setWavetableSample(i,hardware_.getAudioBuffer(pos >> 8));
	//pos += jump;
	//}
	//hardware_.setAudioBufferStatus(AteOscHardware::BUFFER_IDLE);
	//break;
	//}

}