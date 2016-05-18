/*
* AteOsc.cpp
*
* Created: 08/01/2016 15:43:50
* Author: Paul Soulsby
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
	hardware_.getLedSwitch(AteOscHardware::FUNCTION).flash(4,LED_FLASH_TICKS,LED_FLASH_TICKS,LedRgb::RED,LedRgb::GREEN,true);
	hardware_.getLedSwitch(AteOscHardware::VALUE).flash(4,LED_FLASH_TICKS,LED_FLASH_TICKS,LedRgb::GREEN,LedRgb::RED,true);
	//Serial.begin(9600);
}

void AteOsc::poll(unsigned char ticksPassed)
{
	hardware_.pollCvInputs(ticksPassed);
	hardware_.pollSwitches(ticksPassed);
	hardware_.pollRotEncoders(ticksPassed);
	hardware_.pollAudioBufferStatus();
	hardware_.refreshFlash(ticksPassed);
	hardware_.refreshLeds();
	engine_.poll(ticksPassed);
	if(hardware_.getAudioBufferStatus()==AteOscHardware::BUFFER_OVERFLOW && engine_.getPatchPtr()->getOptionValue(AteOscEngine::FUNC_MINLENGTH)==true)
	{
		hardware_.setAudioBufferStatus(AteOscHardware::BUFFER_WAITZCROSS);
	}
}

AteOscEngine::Ctrl AteOsc::cHardToEngFunc(AteOscHardware::CvInputName cvInput)
{
	switch (cvInput)
	{
		case AteOscHardware::CV_PITCHPOT:
		return AteOscEngine::CTRL_PITCHFINE;
		break;
		case AteOscHardware::CV_FILTPOT:
		return AteOscEngine::CTRL_FILTOFF;
		break;
		case AteOscHardware::CV_FLANGE:
		return AteOscEngine::CTRL_FX;
		break;
		case AteOscHardware::CV_PWM:
		return AteOscEngine::CTRL_PWM;
		break;
		case AteOscHardware::CV_RES:
		return AteOscEngine::CTRL_Q;
		break;
	}
}
bool AteOsc::isShiftHold(AteOscEngine::Func func)
{
	if(func==AteOscEngine::FUNC_MEM || func==AteOscEngine::FUNC_WAVELEN)
	{
		return true;
	}
	else
	{
		return false;
	}
}


//***********engine events*********************
void AteOsc::engineFunctionChanged(unsigned char func, unsigned char val)
{
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
	if(opt==true)
	{
		hardware_.getLedSwitch(AteOscHardware::FUNCTION).setColour(LedRgb::GREEN);
	}
	else
	{
		hardware_.getLedSwitch(AteOscHardware::FUNCTION).setColour(LedRgb::RED);
	}
}
void AteOsc::engineMinLengthChanged(unsigned char newLength)
{
	hardware_.setAudioMinLength(newLength);
}
//**************************hardware events************************************
void AteOsc::hardwareCvInputChanged(unsigned char control, unsigned int newValue)
{
	//static const float MULT =  1.0199179261845620021111281154094;
	//static const float OFFSET = -13.19092317;
	static const unsigned int HALF_SCALE = 2048;
	static unsigned int lastValue = 0;
	
	
	if(control==AteOscHardware::CV_PITCH)
	{
		engine_.getPitch().setInput((newValue * 15)>>4);  //see excel for why * 0.9367
	}
	else if(control==AteOscHardware::CV_FILT)
	{
		engine_.setFilterFcInput((newValue * 15)>>4);  //see excel for why * 0.9367
	}
	else if(control==AteOscHardware::CV_CAPTURE)
	{
		if(engine_.getPatchPtr()->getOptionValue(AteOscEngine::FUNC_MINLENGTH)==true && lastValue<HALF_SCALE && newValue>=HALF_SCALE)
		{
			hardware_.setAudioBufferStatus(AteOscHardware::BUFFER_WAITZCROSS);
		}
		lastValue = newValue;
	}
	else
	{
		engine_.getPatchPtr()->setCtrlValue((unsigned char)cHardToEngFunc((AteOscHardware::CvInputName)control),newValue>>4);
	}
	//Serial.print(control,DEC);
	//Serial.print(':');
	//Serial.println(newValue,DEC);
}

void AteOsc::hardwareSwitchChanged(unsigned char sw, unsigned char newValue)
{
	bool opt;

	if(sw==AteOscHardware::FUNCTION)
	{
		if(isShiftHold(engine_.getFunction())==true)
		{
			if(newValue==HIGH)
			{
				hardware_.getLedSwitch(AteOscHardware::FUNCTION).setColour(LedRgb::GREEN);
			}
			else
			{
				hardware_.getLedSwitch(AteOscHardware::FUNCTION).flashStop();
				hardware_.getLedSwitch(AteOscHardware::FUNCTION).setColour(LedRgb::RED);
				switch (engine_.getFunction())
				{
					case AteOscEngine::FUNC_WAVELEN:
					if(hardware_.getSwitch(sw).getHoldTime()>AteOscHardware::HOLD_EVENT_TICKS)
					{
						if(hardware_.getSwitch(AteOscHardware::VALUE).getState()==HIGH)
						{
							engine_.getOscillator().resetFactory();
							hardware_.getLedSwitch(AteOscHardware::VALUE).flash(8,LED_FLASH_TICKS,LED_FLASH_TICKS,LedRgb::GREEN,LedRgb::RED,true);
						}
						else
						{
							engine_.getOscillator().writeWavetable();
						}
					}
					else
					{
						hardware_.setAudioBufferStatus(AteOscHardware::BUFFER_IDLE);
						engine_.getOscillator().resetWavetable();
					}
					break;
					case AteOscEngine::FUNC_MEM:
					if(hardware_.getSwitch(sw).getHoldTime()>AteOscHardware::HOLD_EVENT_TICKS)
					{
						engine_.getPatchPtr()->writePatch(engine_.getPatchPtr()->getFunctionValue(AteOscEngine::FUNC_MEM));
					}
					else
					{
						engine_.getPatchPtr()->readPatch(engine_.getPatchPtr()->getFunctionValue(AteOscEngine::FUNC_MEM));
					}
					break;
				}
				

			}
		}
		else if(newValue==HIGH)
		{
			opt = !engine_.getPatchPtr()->getOptionValue(engine_.getFunction());
			engine_.getPatchPtr()->setOptionValue(engine_.getFunction(),opt);
		}
	}
	if (sw==AteOscHardware::VALUE)
	{
		if(newValue==HIGH && hardware_.getSwitch(AteOscHardware::FUNCTION).getState()==LOW)
		{
			hardware_.setAudioBufferStatus(AteOscHardware::BUFFER_WAITZCROSS);
		}
	}
}
void AteOsc::hardwareSwitchHeld(unsigned char sw)
{

	if(sw==AteOscHardware::FUNCTION && isShiftHold(engine_.getFunction()))
	{
		hardware_.getLedSwitch(AteOscHardware::FUNCTION).flash(8,LED_FLASH_TICKS,LED_FLASH_TICKS,LedRgb::GREEN,LedRgb::RED,true);
	}
}
void AteOsc::hardwareRotaryEncoderChanged(unsigned char rotary, unsigned char newValue, bool clockwise)
{
	if(rotary==AteOscHardware::FUNCTION)
	{
		engine_.setFunction((AteOscEngine::Func)newValue);
	}

	if(rotary==AteOscHardware::VALUE)
	{
		engine_.getPatchPtr()->setFunctionValue(engine_.getFunction(),newValue);
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
		break;
		case AteOscHardware::BUFFER_IDLE:
		hardware_.getLedSwitch(AteOscHardware::VALUE).setColour(LedRgb::GREEN);
		if(engine_.getPatchPtr()->getOptionValue(AteOscEngine::FUNC_MINLENGTH)==true)
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