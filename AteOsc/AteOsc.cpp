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
}


//void AteOscHardwareTester::refreshAudioTest()
//{
//unsigned char i;
//unsigned int scaleWaveLen,skip,pos;
//static const unsigned char SCALE = 8;
//switch (hardware_.getAudioBufferStatus())
//{
//case AteOscHardware::BUFFER_IDLE:
//hardware_.getLedSwitch(AteOscHardware::VALUE).setColour(LedRgb::OFF);
//break;
//case AteOscHardware::BUFFER_WAITZCROSS:
//hardware_.getLedSwitch(AteOscHardware::VALUE).setColour(LedRgb::YELLOW);
//break;
//case AteOscHardware::BUFFER_CAPTURING:
//hardware_.getLedSwitch(AteOscHardware::VALUE).setColour(LedRgb::RED);
//break;
//case AteOscHardware::BUFFER_OVERFLOW:
//hardware_.getLedSwitch(AteOscHardware::FUNCTION).setColour(LedRgb::RED);
//hardware_.getLedSwitch(AteOscHardware::VALUE).setColour(LedRgb::OFF);
//break;
//case AteOscHardware::BUFFER_CAPTURED:
//hardware_.getLedSwitch(AteOscHardware::FUNCTION).setColour(LedRgb::GREEN);
//hardware_.getLedSwitch(AteOscHardware::VALUE).setColour(LedRgb::OFF);
//break;
//}
//
//hardware_.getLedCircular(AteOscHardware::VALUE).select(hardware_.getAudioCurrent()>>4);
//
//if(hardware_.getAudioBufferStatus()==AteOscHardware::BUFFER_CAPTURED)
//{
//scaleWaveLen = hardware_.getAudioBufferLength() << SCALE;
//skip = scaleWaveLen / WAVE_LEN;
//pos = 0;
//for(i=0;i<WAVE_LEN;++i)
//{
//wavetable_.setSample(i,hardware_.getAudioBuffer(pos >> SCALE));
//pos += skip;
//}
//hardware_.setAudioBufferStatus(AteOscHardware::BUFFER_IDLE);
//audio_->pasteWavetable(wavetable_);
//}
//if(hardware_.getAudioBufferStatus()!=AteOscHardware::BUFFER_CAPTURING && hardware_.getAudioBufferStatus()!=AteOscHardware::BUFFER_WAITZCROSS)
//{
//hardware_.setAudioBufferStatus(AteOscHardware::BUFFER_WAITZCROSS);
//}
//}

//***********engine events*********************
void AteOsc::engineFunctionChanged(unsigned char func, unsigned char val, bool opt)
{
	if(func==AteOscEngine::FUNC_WAVELEN || func==AteOscEngine::FUNC_MINCAPLEN)
	{
		hardware_.getRotEncoder(AteOscHardware::VALUE).setMaxValue(4);
		if(func==AteOscEngine::FUNC_MINCAPLEN)
		{
			hardware_.setAudioMinLength(1<<(val+4));
			//if(opt==true)
			//{
				//hardware_.setAudioPrescaler(7);
			//}
			//else
			//{
				//hardware_.setAudioPrescaler(3);
			//}
		}
	}
	else
	{
		hardware_.getRotEncoder(AteOscHardware::VALUE).setMaxValue(16);
	}
	hardware_.getRotEncoder(AteOscHardware::VALUE).setValue((char)val);
	hardware_.getLedCircular(AteOscHardware::FUNCTION).select(func);
	hardware_.getLedCircular(AteOscHardware::VALUE).select(val);
	if(opt==true)
	{
		hardware_.getLedSwitch(AteOscHardware::FUNCTION).setColour(LedRgb::GREEN);
	}
	else
	{
		hardware_.getLedSwitch(AteOscHardware::FUNCTION).setColour(LedRgb::RED);
	}
	//hmm what to do about this?
	//if(func==AteOscEngine::FUNC_CAPFREQ && val==15)
	//{
		//hardware_.setAudioBufferStatus(AteOscHardware::BUFFER_WAITZCROSS);
	//}
}

//**************************hardware events************************************
void AteOsc::hardwareCvInputChanged(unsigned char control, unsigned int newValue)
{
	static const float MULT =  1.0199179261845620021111281154094;
	static const float OFFSET = -13.19092317;
	static const unsigned int HALF_SCALE = 2048;
	static unsigned int lastValue = 0;
	
	
	if(control==AteOscHardware::CV_PITCH)
	{
			float adj = ((float)newValue * MULT) + OFFSET;
			if(adj>4095)
			{
				engine_.setFrequencyCv(4095);
			}
			else if(adj<0)
			{
				engine_.setFrequencyCv(0);
			}
			else
			{
				engine_.setFrequencyCv((unsigned int)adj);
			}
			
		
	}
	else if(control==AteOscHardware::CV_CAPTURE)
	{
		if(engine_.getPatchPtr()->getOptionValue(AteOscEngine::FUNC_CAPFREQ)==true && lastValue<HALF_SCALE && newValue>=HALF_SCALE)
		{
			hardware_.setAudioBufferStatus(AteOscHardware::BUFFER_WAITZCROSS);
		}
		lastValue = newValue;
	}
	else
	{
		engine_.getPatchPtr()->setCtrlValue(control,newValue>>4);
	}
	
}

void AteOsc::hardwareSwitchChanged(unsigned char sw, unsigned char newValue)
{
	bool opt;

	if(sw==AteOscHardware::FUNCTION)
	{
		if(engine_.getFunction()==AteOscEngine::FUNC_MEM)
		{
			if(newValue==HIGH)
			{
				hardware_.getLedSwitch(AteOscHardware::FUNCTION).setColour(LedRgb::GREEN);
			}
			else
			{
				hardware_.getLedSwitch(AteOscHardware::FUNCTION).flashStop();
				hardware_.getLedSwitch(AteOscHardware::FUNCTION).setColour(LedRgb::RED);
				if(hardware_.getSwitch(sw).getHoldTime()>AteOscHardware::HOLD_EVENT_TICKS)
				{
					engine_.getPatchPtr()->writePatch(engine_.getPatchPtr()->getFunctionValue(AteOscEngine::FUNC_MEM));
				}
				else
				{
					engine_.getPatchPtr()->readPatch(engine_.getPatchPtr()->getFunctionValue(AteOscEngine::FUNC_MEM));
				}
			}
		}
		else if(newValue==HIGH)
		{
			opt = !engine_.getPatchPtr()->getOptionValue(engine_.getFunction());
			engine_.getPatchPtr()->setOptionValue(engine_.getFunction(),opt);
			if(opt==true)
			{
				hardware_.getLedSwitch(AteOscHardware::FUNCTION).setColour(LedRgb::GREEN);
			}
			else
			{
				hardware_.getLedSwitch(AteOscHardware::FUNCTION).setColour(LedRgb::RED);
			}
		}
	}
	if (sw==AteOscHardware::VALUE)
	{
		if(newValue==HIGH)
		{
			hardware_.setAudioBufferStatus(AteOscHardware::BUFFER_WAITZCROSS);
		}
	}
}
void AteOsc::hardwareSwitchHeld(unsigned char sw)
{

	if(sw==AteOscHardware::FUNCTION && engine_.getFunction()==AteOscEngine::FUNC_MEM)
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
		if(engine_.getPatchPtr()->getFunctionValue(AteOscEngine::FUNC_CAPFREQ)==15)
		{
			hardware_.setAudioBufferStatus(AteOscHardware::BUFFER_WAITZCROSS);
		}
		break;
		case AteOscHardware::BUFFER_IDLE:
		hardware_.getLedSwitch(AteOscHardware::VALUE).setColour(LedRgb::GREEN);
		if(engine_.getPatchPtr()->getFunctionValue(AteOscEngine::FUNC_CAPFREQ)==15)
		{
			hardware_.setAudioBufferStatus(AteOscHardware::BUFFER_WAITZCROSS);
		}
		break;
		case AteOscHardware::BUFFER_CAPTURED:
		hardware_.getLedSwitch(AteOscHardware::VALUE).setColour(LedRgb::GREEN);
		//look at hardware tester if you wanna know why these numbers work!
		unsigned int pos = 0;
		unsigned int jump = hardware_.getAudioBufferLength() << 1;
		unsigned char destWaveLen = engine_.getOscillatorPtr()->getUserWaveLength();
		for(unsigned char i=0;i<destWaveLen;++i)
		{
			engine_.getOscillatorPtr()->setUserWavetableSample(i,hardware_.getAudioBuffer(pos >> 8));
			pos += jump;
		}
		engine_.getPatchPtr()->setOptionValue(AteOscEngine::FUNC_WAVELEN,true);
		hardware_.setAudioBufferStatus(AteOscHardware::BUFFER_IDLE);
		break;
	}

}