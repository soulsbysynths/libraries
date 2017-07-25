/*
//AteOsc.cpp  Oscitron main class for standard software
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


#include "AteOsc.h"

// default constructor
AteOsc::AteOsc() : engine_(AteOscEngine::getInstance()), hardware_(AteOscHardware::getInstance())
{
	engine_.construct(this);
	hardware_.construct(this);
} //AteOsc

// default destructor
AteOsc::~AteOsc()
{
} //~AteOsc

void AteOsc::initialize()
{
	hardware_.getRotEncoder(AteOscHardware::FUNCTION).setContinuous(true);
	engine_.initialize();
	engine_.getQuantize().setQntKey(hardware_.readEepromByte(AteOscHardware::EEPROM_QUANT_KEY));
	setClockMode((ClockMode)hardware_.readEepromByte(AteOscHardware::EEPROM_CLOCK_MODE));
	hardware_.setCtrlMode((AteOscHardware::CtrlMode)hardware_.readEepromByte(AteOscHardware::EEPROM_CTRL_MODE));
	engine_.getPatchPtr()->readPatch(hardware_.readEepromByte(AteOscHardware::EEPROM_CURRENT_PATCH) & 0x0F);
	hardware_.getLedSwitch(AteOscHardware::FUNCTION).flash(4,LED_FLASH_TICKS,LED_FLASH_TICKS,LedRgb::RED,LedRgb::GREEN,true);
	hardware_.getLedSwitch(AteOscHardware::VALUE).flash(4,LED_FLASH_TICKS,LED_FLASH_TICKS,LedRgb::GREEN,LedRgb::RED,true);
}
void AteOsc::poll(unsigned char ticksPassed)
{
	hardware_.pollCvInputs(ticksPassed);
	hardware_.pollGateInputs();
	hardware_.pollSwitches(ticksPassed);
	hardware_.pollRotEncoders(ticksPassed);
	hardware_.pollAudioBufferStatus();
	hardware_.refreshFlash(ticksPassed);
	hardware_.refreshLeds();
	engine_.pollPitch(ticksPassed);
	engine_.pollWave();
}

void AteOsc::setClockMode(ClockMode newMode)
{
	clockMode_ = newMode;
	if(newMode<CM_WAVE_CV)
	{
		hardware_.setInputMode(AteOscHardware::CV_CAPTURE, AteOscHardware::IP_GATE);
	}
	else
	{
		hardware_.setInputMode(AteOscHardware::CV_CAPTURE, AteOscHardware::IP_CV);
	}
}

//***********engine events*********************
void AteOsc::engineFunctionChanged(unsigned char func, unsigned char val)
{
	if(valueSecondaryMode_==true)
	{
		return;
	}
	if(func==AteOscEngine::FUNC_WAVELEN)
	{
		hardware_.getRotEncoder(AteOscHardware::VALUE).setMaxValue(4);
	}
	else if(func==AteOscEngine::FUNC_MINLENGTH)
	{
		hardware_.getRotEncoder(AteOscHardware::VALUE).setMaxValue(8);
	}
	else
	{
		hardware_.getRotEncoder(AteOscHardware::VALUE).setMaxValue(16);
	}
	hardware_.getRotEncoder(AteOscHardware::VALUE).setValue((char)val);
	hardware_.getLedCircular(AteOscHardware::FUNCTION).select(func);
	hardware_.getLedCircular(AteOscHardware::VALUE).select(val);
}
void AteOsc::engineOptionChanged(unsigned char func, bool opt)
{
	if(valueSecondaryMode_==true)
	{
		return;
	}
	if(opt==true)
	{
		hardware_.getLedSwitch(AteOscHardware::FUNCTION).setColour(LedRgb::GREEN);
	}
	else
	{
		hardware_.getLedSwitch(AteOscHardware::FUNCTION).setColour(LedRgb::RED);
	}
}
void AteOsc::engineStartCapture(bool way)
{
	if(way==true)
	{
		hardware_.setAudioBufferStatus(AteOscHardware::BUFFER_WAITZCROSS);
	}
	else
	{
		hardware_.setAudioBufferStatus(AteOscHardware::BUFFER_IDLE);
	}
}
void AteOsc::engineMinLengthChanged(unsigned char newLength)
{
	hardware_.setAudioMinLength(newLength);
}
void AteOsc::engineDoEvents()
{
	hardware_.pollGateInputs();
	hardware_.pollMappedCvInput(AteOscHardware::CV_PITCH);
	engine_.pollPitch(0);
}
//**************************hardware events************************************
void AteOsc::hardwareCvInputChanged(unsigned char input, unsigned int newValue)
{
	unsigned char oldVal, newVal;
	bool oldOpt, newOpt;
	if(input==AteOscHardware::CV_PITCH)
	{
		engine_.getPitch().setInput((newValue * 15)>>4);  //see excel for why * 0.9367
	}
	else if(input==AteOscHardware::CV_FILT)
	{
		engine_.setFilterFcInput((newValue * 15)>>4);  //see excel for why * 0.9367
	}
	else if(input==AteOscHardware::CV_CAPTURE)
	{
		if(clockMode_>=CM_WAVE_CV && clockMode_<=CM_BITCRUSH_CV)
		{
			AteOscEngine::Func f = CLOCK_MODE_TO_FUNC[clockMode_ - CM_WAVE_CV];
			oldOpt = engine_.getPatchPtr()->getOptionValue(f);
			oldVal = engine_.getPatchPtr()->getFunctionValue(f);
			switch(clockMode_)
			{
				case CM_WAVE_CV:
				case CM_PITCHCOARSE_CV:
				newOpt = newValue >= AteOscHardware::CV_HALF_SCALE ? HIGH : LOW;
				newVal = (newValue & AteOscHardware::CV_HALF_SCALE_MASK) >> 7;
				break;
				case CM_RES_CV:
				newOpt = oldOpt;
				newVal = newValue >> 10;
				break;
				case CM_PORT_CV:
				case CM_FILT_CV:
				case CM_BITCRUSH_CV:
				newOpt = oldOpt;
				newVal = newValue >> 8;
				break;
			}
			if(newOpt!=oldOpt)
			{
				engine_.getPatchPtr()->setOptionValue(f,newOpt);
			}
			if(newVal!=oldVal)
			{
				engine_.getPatchPtr()->setFunctionValue(f,newVal);
			}
		}
		else if(clockMode_==CM_QUANTKEY_CV)
		{
			oldVal = engine_.getQuantize().getQntKey();
			newVal = (newValue * 3) >> 10;  //= newvalue / 12
			if(newVal!=oldVal)
			{
				engine_.getQuantize().setQntKey(newVal);
				if(valueSecondaryMode_)
				{
					hardware_.getRotEncoder(AteOscHardware::VALUE).setValue((char)newVal);
					hardware_.getLedCircular(AteOscHardware::VALUE).select(newVal);
				}				
			}
		}
		else if(clockMode_==CM_PITCHATT_CV)
		{
			hardware_.getCvInput(AteOscHardware::CV_PITCH).setGain(newValue >> 4);
		}
		else if(clockMode_==CM_FILTATT_CV)
		{
			hardware_.getCvInput(AteOscHardware::CV_FILT).setGain(newValue >> 4);
		}
	}
	else
	{
		//engine_.getPatchPtr()->setCtrlValue((unsigned char)cHardToEngCtrl((AteOscHardware::CvInputName)input),newValue>>4);
		engine_.getPatchPtr()->setCtrlValue((unsigned char)CV_INPUT_TO_CTRL[input],newValue>>4);
	}
}
void AteOsc::hardwareGateInputChanged(unsigned char input, bool newValue)
{
	unsigned char p;
	if(newValue==true)
	{
		
		switch(clockMode_)
		{
			case CM_CAPTURE:
			hardware_.setAudioBufferStatus(AteOscHardware::BUFFER_WAITZCROSS);
			break;
			case CM_WAVE_INC:
			p = engine_.getPatchPtr()->getFunctionValue(AteOscEngine::FUNC_WAVE) + 1;
			p &= 0x0F;
			engine_.getPatchPtr()->setFunctionValue(AteOscEngine::FUNC_WAVE,p);
			break;
			case CM_WAVELEN_INC:
			p = engine_.getPatchPtr()->getFunctionValue(AteOscEngine::FUNC_WAVELEN) + 1;
			p &= 0x03;
			engine_.getPatchPtr()->setFunctionValue(AteOscEngine::FUNC_WAVELEN,p);
			break;
			case CM_PORT_INC:
			p = engine_.getPatchPtr()->getFunctionValue(AteOscEngine::FUNC_PORTA) + 1;
			p &= 0x0F;
			engine_.getPatchPtr()->setFunctionValue(AteOscEngine::FUNC_PORTA,p);
			break;
			case CM_FILT_INC:
			p = engine_.getPatchPtr()->getFunctionValue(AteOscEngine::FUNC_FILT) + 1;
			p &= 0x0F;
			engine_.getPatchPtr()->setFunctionValue(AteOscEngine::FUNC_FILT,p);
			break;
			case CM_BITCRUSH_INC:
			p = engine_.getPatchPtr()->getFunctionValue(AteOscEngine::FUNC_BITCRUSH) + 1;
			p &= 0x0F;
			engine_.getPatchPtr()->setFunctionValue(AteOscEngine::FUNC_BITCRUSH,p);
			break;
			case CM_PATCH_INC:
			p = engine_.getPatchPtr()->getFunctionValue(AteOscEngine::FUNC_MEM) + 1;
			p &= 0x0F;
			engine_.getPatchPtr()->readPatch(p);
			break;
		}
	}
}
void AteOsc::hardwareSwitchChanged(unsigned char sw, unsigned char newValue)
{
	AteOscEngine::Func f = engine_.getFunction();
	bool funcHeld = ((hardware_.getSwitch(AteOscHardware::FUNCTION).getHoldTime()>AteOscHardware::HOLD_EVENT_TICKS) ? true : false);
	if(sw==AteOscHardware::FUNCTION)
	{
		if(newValue==HIGH)
		{
			hardware_.getLedSwitch(AteOscHardware::FUNCTION).setColour(LedRgb::GREEN);
		}
		else
		{
			hardware_.getLedSwitch(AteOscHardware::FUNCTION).flashStop();
			hardware_.getLedSwitch(AteOscHardware::FUNCTION).setColour(LedRgb::RED);
			switch (f)
			{
				case AteOscEngine::FUNC_WAVE:
				if(heldFuncAndValue_)
				{
					heldFuncAndValue_ = false;
					engine_.getOscillator().resetFactory();
				}
				else
				{
					toggleOption();
				}
				break;
				case AteOscEngine::FUNC_WAVELEN:
				if(funcHeld)
				{
					engine_.getOscillator().writeWavetable();
				}
				else
				{
					hardware_.setAudioBufferStatus(AteOscHardware::BUFFER_IDLE);
					engine_.getOscillator().resetWavetable();
				}
				break;
				case AteOscEngine::FUNC_PITCHCOARSE:
				if(heldFuncAndValue_)
				{
					heldFuncAndValue_ = false;
					switch(hardware_.getRotEncoder(AteOscHardware::VALUE).getValue())
					{
						case 0:
						hardware_.calcCvCalib(AteOscHardware::EEPROM_PITCH_LOW);
						break;
						case 1:
						hardware_.calcCvCalib(AteOscHardware::EEPROM_PITCH_HIGH);
						break;
						case 2:
						hardware_.calcCvCalib(AteOscHardware::EEPROM_FILT_LOW);
						break;
						case 3:
						hardware_.calcCvCalib(AteOscHardware::EEPROM_FILT_HIGH);
						break;
					}
				}
				else
				{
					toggleOption();
				}
				break;
				case AteOscEngine::FUNC_PORTA:
				case AteOscEngine::FUNC_MINLENGTH:
				case AteOscEngine::FUNC_FILT:
				if(valueSecondaryMode_==true)
				{
					unsigned char val = hardware_.getRotEncoder(AteOscHardware::VALUE).getValue();
					switch (f)
					{
						case AteOscEngine::FUNC_PORTA:
						engine_.getQuantize().setQntKey(val);
						hardware_.writeEepromByte(AteOscHardware::EEPROM_QUANT_KEY, val);
						break;
						case AteOscEngine::FUNC_MINLENGTH:
						setClockMode((ClockMode)val);
						hardware_.writeEepromByte(AteOscHardware::EEPROM_CLOCK_MODE, val);
						break;
						case AteOscEngine::FUNC_FILT:
						hardware_.setCtrlMode((AteOscHardware::CtrlMode)val);
						hardware_.writeEepromByte(AteOscHardware::EEPROM_CTRL_MODE, val);
						break;
					}
					valueSecondaryMode_ = false;
					engineFunctionChanged(f,engine_.getPatchPtr()->getFunctionValue(f)); //force circ LED through and RE max value
					engineOptionChanged(f,engine_.getPatchPtr()->getOptionValue(f)); //force sw LED through
				}
				else
				{
					if(funcHeld)
					{
						valueSecondaryMode_ = true;
						hardware_.getLedSwitch(AteOscHardware::FUNCTION).setColour(LedRgb::YELLOW);
						switch(f)
						{
							case AteOscEngine::FUNC_PORTA:
							hardware_.getRotEncoder(AteOscHardware::VALUE).setMaxValue(12);
							hardware_.getRotEncoder(AteOscHardware::VALUE).setValue((char)engine_.getQuantize().getQntKey());
							hardware_.getLedCircular(AteOscHardware::VALUE).select(engine_.getQuantize().getQntKey());
							break;
							case AteOscEngine::FUNC_MINLENGTH:
							hardware_.getRotEncoder(AteOscHardware::VALUE).setMaxValue(16);
							hardware_.getRotEncoder(AteOscHardware::VALUE).setValue((char)clockMode_);
							hardware_.getLedCircular(AteOscHardware::VALUE).select(clockMode_);
							break;
							case AteOscEngine::FUNC_FILT:
							hardware_.getRotEncoder(AteOscHardware::VALUE).setMaxValue(4);
							hardware_.getRotEncoder(AteOscHardware::VALUE).setValue((char)hardware_.getCtrlMode());
							hardware_.getLedCircular(AteOscHardware::VALUE).select(hardware_.getCtrlMode());
							break;
						}
					}
					else
					{
						toggleOption();
					}
				}
				break;
				case AteOscEngine::FUNC_BITCRUSH:
				if(funcHeld)
				{
					toggleOption();
				}
				else
				{
					toggleOption();
				}
				break;
				case AteOscEngine::FUNC_MEM:
				if(funcHeld)
				{
					engine_.getPatchPtr()->writePatch(engine_.getPatchPtr()->getFunctionValue(AteOscEngine::FUNC_MEM));
				}
				else
				{
					engine_.getPatchPtr()->readPatch(engine_.getPatchPtr()->getFunctionValue(AteOscEngine::FUNC_MEM));
				}
				hardware_.writeEepromByte(AteOscHardware::EEPROM_CURRENT_PATCH,engine_.getPatchPtr()->getFunctionValue(AteOscEngine::FUNC_MEM));
				break;
			}
		}
	}
	if (sw==AteOscHardware::VALUE)
	{
		if(newValue==HIGH && hardware_.getSwitch(AteOscHardware::FUNCTION).getState()==LOW && valueSecondaryMode_==false)
		{
			hardware_.setAudioBufferStatus(AteOscHardware::BUFFER_WAITZCROSS);
			if(hardware_.getSwitch(AteOscHardware::FUNCTION).getHoldTime()>AteOscHardware::HOLD_EVENT_TICKS)
			{
				heldFuncAndValue_ = true;
			}
		}
	}
}
void AteOsc::toggleOption()
{
	bool opt;
	opt = !engine_.getPatchPtr()->getOptionValue(engine_.getFunction());
	engine_.getPatchPtr()->setOptionValue(engine_.getFunction(),opt);
}
void AteOsc::hardwareSwitchHeld(unsigned char sw)
{

	//if(sw==AteOscHardware::FUNCTION && isShiftHold(engine_.getFunction()))
	if(sw==AteOscHardware::FUNCTION && valueSecondaryMode_==false)
	{
		if(hardware_.getLedSwitch(AteOscHardware::FUNCTION).getColour()==LedRgb::RED)
		{
			hardware_.getLedSwitch(AteOscHardware::FUNCTION).flash(8,LED_FLASH_TICKS,LED_FLASH_TICKS,LedRgb::GREEN,LedRgb::RED,true);
		}
		else
		{
			hardware_.getLedSwitch(AteOscHardware::FUNCTION).flash(8,LED_FLASH_TICKS,LED_FLASH_TICKS,LedRgb::RED,LedRgb::GREEN,true);
		}
		if(hardware_.getSwitch(AteOscHardware::VALUE).getState()==HIGH)
		{
			heldFuncAndValue_ = true;
		}
	}
}
void AteOsc::hardwareRotaryEncoderChanged(unsigned char rotary, unsigned char newValue, bool clockwise)
{
	if(rotary==AteOscHardware::FUNCTION)
	{
		if(!valueSecondaryMode_)
		{
			engine_.setFunction((AteOscEngine::Func)newValue);
		}
		else
		{
			hardware_.getRotEncoder(AteOscHardware::FUNCTION).setValue(engine_.getFunction());
		}
	}

	if(rotary==AteOscHardware::VALUE)
	{
		if(!valueSecondaryMode_)
		{
			engine_.getPatchPtr()->setFunctionValue(engine_.getFunction(),newValue);
		}
		else
		{
			hardware_.getLedCircular(AteOscHardware::VALUE).select(newValue);
		}
	}
}

void AteOsc::hardwareAudioBufferStatusChanged(unsigned char newStatus)
{

	switch (newStatus)
	{
		case AteOscHardware::BUFFER_WAITZCROSS:
		hardware_.getLedSwitch(AteOscHardware::VALUE).setColour(LedRgb::YELLOW);
		break;
		case AteOscHardware::BUFFER_CAPTURING:
		hardware_.getLedSwitch(AteOscHardware::VALUE).setColour(LedRgb::RED);
		break;
		case AteOscHardware::BUFFER_OVERFLOW:
		hardware_.getLedSwitch(AteOscHardware::VALUE).setColour(LedRgb::RED);
		if(engine_.getPatchPtr()->getOptionValue(AteOscEngine::FUNC_MINLENGTH))
		{
			hardware_.setAudioBufferStatus(AteOscHardware::BUFFER_WAITZCROSS);
		}
		break;
		case AteOscHardware::BUFFER_IDLE:
		hardware_.getLedSwitch(AteOscHardware::VALUE).setColour(LedRgb::GREEN);
		if(engine_.getPatchPtr()->getOptionValue(AteOscEngine::FUNC_MINLENGTH))
		{
			hardware_.setAudioBufferStatus(AteOscHardware::BUFFER_WAITZCROSS);
		}
		break;
		case AteOscHardware::BUFFER_CAPTURED:
		hardware_.getLedSwitch(AteOscHardware::VALUE).setColour(LedRgb::GREEN);
		//see excel sheet for proof of this interpolation
		unsigned int pos = 0;
		unsigned int jump = (unsigned int)hardware_.getAudioBufferLength() << 1;  //0-255
		for(unsigned char i=0;i<engine_.getOscillator().getWaveLength();++i)
		{
			engine_.getOscillator().setWavetableSample(i,hardware_.getAudioBuffer(pos >> 8));
			pos += jump;
		}
		hardware_.setAudioBufferStatus(AteOscHardware::BUFFER_IDLE);
		break;
	}

}