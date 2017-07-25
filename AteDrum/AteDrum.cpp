/*
//AteDrum.cpp  Oscitron Drum main class
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
	refreshLeds();
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
	refreshRunning(ticksPassed);
	refreshMidiNoteAction();
}

void AteDrum::readPatch(unsigned char patchNum)
{
	hardware_.getLedSwitch(AteOscHardware::FUNCTION).flash(255,LED_FLASH_LONG_TICKS,LED_FLASH_LONG_TICKS,LedRgb::RED,LedRgb::GREEN,true);
	engine_.setBufferPatchRead(patchNum);
}
void AteDrum::stopReset()
{
	runningTimeOutTick_ = RUNNING_TIME_OUT_DEFAULT;
	engine_.setRunning(false);
	engine_.setPlayStep(0);
	engine_.setBufferPatchRead(AteDrumEngine::CLEAR_PATCH_READ);
	hardware_.refreshLeds();
}
void AteDrum::stopFunctionFlash()
{
	hardware_.getLedSwitch(AteOscHardware::FUNCTION).flashStop();
	hardware_.getLedSwitch(AteOscHardware::FUNCTION).setColour(LedRgb::RED);
}

void AteDrum::refreshRunning(unsigned char ticksPassed)
{
	if(engine_.getRunning())
	{
		runningTick_ += ticksPassed;
		if(runningTick_>=runningTimeOutTick_)
		{
			stopReset();
		}
	}
}
void AteDrum::toggleFuncLed()
{
		if(hardware_.getLedSwitch(AteOscHardware::FUNCTION).getColour()==LedRgb::RED)
		{
			hardware_.getLedSwitch(AteOscHardware::FUNCTION).setColour(LedRgb::GREEN);
		}
		else
		{
			hardware_.getLedSwitch(AteOscHardware::FUNCTION).setColour(LedRgb::RED);
		}
}
void AteDrum::refreshMidiNoteAction()
{

	if(midiNoteDebounce_<MIDI_NOTE_ACTION_DEBOUNCE)
	{
		midiNoteDebounce_++;
	}
	else if(midiNoteDebounce_==MIDI_NOTE_ACTION_DEBOUNCE)
	{
		//toggleFuncLed();
		if(midiNoteAction_>=1 && midiNoteAction_<=8)
		{
			readPatch(midiNoteAction_-1);
		}
		else if(midiNoteAction_>=13 && midiNoteAction_<=20)
		{
			engine_.playInstr(midiNoteAction_-13,15);
		}
		else if(midiNoteAction_==MIDI_NOTE_ACTION_RESET)
		{
			stopReset();
		}
		midiNoteDebounce_++;
	}
}

//***********engine events*********************
void AteDrum::engineInstrChanged(unsigned char instr)
{
	refreshLeds();
}

void AteDrum::engineEditStepChanged(unsigned char step)
{
	refreshLeds();
}
void AteDrum::enginePlayStepChanged(unsigned char step)
{
	if(step!=engine_.getEditStep())
	{
		hardware_.getLedCircular(AteOscHardware::VALUE).flash(step,1,LED_FLASH_SHORT_TICKS, LED_FLASH_SHORT_TICKS,!engine_.getPatchPtr()->getPatternStep(engine_.getInstr(),step));
	}
}
void AteDrum::refreshLeds()
{
	unsigned char es = engine_.getEditStep();
	unsigned char instr = engine_.getInstr();
	hardware_.getLedCircular(AteOscHardware::FUNCTION).select(instr);
	hardware_.getLedCircular(AteOscHardware::VALUE).flashStopAll();
	hardware_.getLedCircular(AteOscHardware::VALUE).setState(engine_.getPatchPtr()->getPattern(instr));
	if(engine_.getPatchPtr()->getPatternStep(instr,es)==true)
	{
		hardware_.getLedSwitch(AteOscHardware::VALUE).setColour(LedRgb::GREEN);
		hardware_.getLedCircular(AteOscHardware::VALUE).flash(es,255,LED_FLASH_LONG_TICKS,LED_FLASH_SHORT_TICKS,true);
	}
	else
	{
		hardware_.getLedSwitch(AteOscHardware::VALUE).setColour(LedRgb::RED);
		hardware_.getLedCircular(AteOscHardware::VALUE).flash(es,255,LED_FLASH_SHORT_TICKS,LED_FLASH_LONG_TICKS,false);
	}
}
void AteDrum::enginePatternChanged(unsigned char step, bool newValue)
{
	if(step==engine_.getEditStep())
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
	//toggleFuncLed();
	stopFunctionFlash();
	hardware_.writeEepromByte(AteOscHardware::EEPROM_CURRENT_PATCH,patchNum);
	refreshLeds();
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
		newValue = 4095 - newValue;  //invert it
		unsigned int lf = ((unsigned long)newValue * 3377) >> 12;  //3377 = 11025 as linear freq
		engine_.setLinearFreq(1,(lf>>5)<<5);
	}
	else if(input==AteOscHardware::CV_FILT)
	{
		unsigned int mn = newValue + 34;  //want to round 50cent below and above to midi note.  Therefore add 50cent.  68.26666667 per note = 34
		mn = (mn * 15) >> 10;  // = (CV * 12) / (4096 / 5)
		if(mn>MIDI_NOTE_ACTION_RESET)  //0 = do nothing, 1-8 = load patch, 14-21 = triger sound, 22 = reset
		{
			mn = MIDI_NOTE_ACTION_RESET;
		}
		if(mn!=midiNoteAction_)
		{
			midiNoteAction_ = mn;
			midiNoteDebounce_ = 0;
		}
	}
	else
	{
		AteDrumEngine::Ctrl ctrl = CV_INPUT_TO_CTRL[input];
		unsigned char c = newValue >> 8;
		if(ctrl<AteDrumEngine::CTRL_DUMMY && c!=cvInputBuffer[ctrl])  //safety
		{
			engine_.getPatchPtr()->setCtrlValue(engine_.getInstr(),(unsigned char)ctrl,newValue>>8);
			cvInputBuffer[ctrl] = c;
		}
	}

}
void AteDrum::hardwareGateInputChanged(unsigned char input, bool newValue)
{
	if(newValue==true)
	{
		if(engine_.getRunning())
		{
			unsigned char ps = engine_.getPlayStep();
			ps++;
			ps &= 0x0F;
			engine_.setPlayStep(ps);
			runningTimeOutTick_ = runningTick_ << 1;
		}
		else
		{
			engine_.setRunning(true);
		}
		runningTick_ = 0;
		engine_.playStep();
	}

}
void AteDrum::hardwareSwitchChanged(unsigned char sw, unsigned char newValue)
{
	unsigned char i = engine_.getInstr();
	unsigned char s = engine_.getEditStep();
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
				engine_.getPatchPtr()->writePatch(i);
				hardware_.writeEepromByte(AteOscHardware::EEPROM_CURRENT_PATCH,i);
			}
			else
			{
				readPatch(i);
			}
			hardware_.getLedSwitch(AteOscHardware::FUNCTION).setColour(LedRgb::RED);
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
			if (funcHeld)
			{
				hardware_.getLedSwitch(AteOscHardware::VALUE).flashStop();
				if(hardware_.getSwitch(AteOscHardware::FUNCTION).getState()==HIGH)
				{
					engine_.setPlayStep(0);
				}
				else
				{
					engine_.getPatchPtr()->clearPattern();
				}
				refreshLeds();
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
		engine_.setEditStep(newValue);
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