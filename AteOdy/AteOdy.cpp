/*
* AteOdy.cpp
*
* Created: 27/09/2016 14:42:01
* Author: paulsoulsby
*/


#include "AteOdy.h"

#define MICROSECONDS_PER_TIMER2_OVERFLOW 64
#define MICROSECONDS_PER_MILLSECOND 1000
#define min(a) ((a < 127) ? a : 127)
#define max(a) ((a < -127) ? -127 : a)
#define clip(a) min(max(a))

static const char RM_LEVEL[8] PROGMEM = {0,21,34,48,65,83,105,127};
static const char SCALE = 7;
static const unsigned char SQ_PULSE_INDEX = 127;
volatile static unsigned char ticksPassed = 0;
volatile static unsigned char oscWave[3];
volatile static unsigned char oscPulseIndex[2];
volatile static unsigned char oscLevel[3];
volatile static unsigned char filtType = 0;
volatile static char filtC = 0;
volatile static char filtRC = 0;
static int filtBuf[2] = {0,0};

ISR(TIMER2_OVF_vect) {

	static unsigned char processStage = 0;
	static unsigned int tickCnt = 0;
	static int output = 0;
	static unsigned char index[2];

	processStage++;
	processStage &= 0x03;

	if(processStage==0)
	{
		tickCnt += MICROSECONDS_PER_TIMER2_OVERFLOW;
		if (tickCnt >= MICROSECONDS_PER_MILLSECOND)
		{
			tickCnt -= MICROSECONDS_PER_MILLSECOND;
			ticksPassed++;
		}
		index[0] = OdyAudio::getWtIndex(0);
		index[1] = OdyAudio::getWtIndex(1);
		if(oscWave[0]==0)
		{
			output = (char)pgm_read_byte(&(SAW_WAVE[oscLevel[0]][index[0]]));
		}
		else
		{
			output = ((index[0] > SQ_PULSE_INDEX) ? -(char)pgm_read_byte(&(SQUARE_WAVE[oscLevel[0]][index[0]-SQ_PULSE_INDEX])) : (char)pgm_read_byte(&(SQUARE_WAVE[oscLevel[0]][index[0]])));
		}
		if(oscWave[1]==0)
		{
			output += (char)pgm_read_byte(&(SAW_WAVE[oscLevel[1]][index[1]]));
		}
		else
		{
			output += ((index[1] > oscPulseIndex[1]) ? -(char)pgm_read_byte(&(SQUARE_WAVE[oscLevel[1]][index[1]-oscPulseIndex[1]])) : (char)pgm_read_byte(&(SQUARE_WAVE[oscLevel[1]][index[1]])));
		}
	}
	else if(processStage==1)
	{
		if(oscWave[2]==0)
		{
			output += OdyNoise::getOutput();
		}
		else
		{
			output += (((index[0] > SQ_PULSE_INDEX) ^ (index[1] > oscPulseIndex[1])) ? -(char)pgm_read_byte(&(RM_LEVEL[oscLevel[2]])) : (char)pgm_read_byte(&(RM_LEVEL[oscLevel[2]])));
		}
		
		output = clip(output>>1);
	}
	else if(processStage==2)
	{
		if(filtType==OdyFilter::MOZZI)
		{
			filtBuf[0] += (output*filtC - filtBuf[0]*filtC + filtBuf[0]*filtRC - filtBuf[1]*filtRC)>>SCALE;  //filtRC is prescaled here
		}
		else if(filtType==OdyFilter::SIMPLE)
		{
			filtBuf[0] -= (filtBuf[0]*filtRC - filtBuf[1]*filtC + output*filtC)>>SCALE;
		}
	}
	else
	{
		if(filtType==OdyFilter::MOZZI)
		{
			filtBuf[1] += (filtBuf[0]*filtC - filtBuf[1]*filtC)>>SCALE;
		}
		else if(filtType==OdyFilter::SIMPLE)
		{
			filtBuf[1] -= (filtBuf[1]*filtRC + filtBuf[0]*filtC)>>SCALE;
		}
		else
		{
			filtBuf[1] = output;
		}
		filtBuf[1] = clip(filtBuf[1]);
		OCR2B = (unsigned char)(filtBuf[1] + 127);
	}

}

// default constructor
AteOdy::AteOdy()  : engine_(AteOdyEngine::getInstance()), hardware_(AteOscHardware::getInstance())
{
	engine_.construct(this);
	hardware_.construct(this);
} //AteOdy

// default destructor
AteOdy::~AteOdy()
{
} //~AteOdy

void AteOdy::initialize()
{
	
	hardware_.getRotEncoder(AteOscHardware::FUNCTION).setContinuous(true);
	hardware_.getRotEncoder(AteOscHardware::VALUE).setContinuous(false);

	engine_.initialize();
	hardware_.getLedSwitch(0).flash(4,LED_FLASH_TICKS,LED_FLASH_TICKS,LedRgb::YELLOW,LedRgb::RED,true);

}

void AteOdy::poll()
{
	unsigned char ticks = ticksPassed;
	hardware_.pollCvInputs(ticks);
	hardware_.pollGateInputs();
	hardware_.pollMappedCvInput(AteOscHardware::CV_PITCH);
	hardware_.pollMappedCvInput(AteOscHardware::CV_FILT);
	hardware_.pollSwitches(ticks);
	hardware_.pollRotEncoders(ticks);
	hardware_.pollAudioBufferStatus();
	hardware_.refreshFlash(ticks);
	hardware_.refreshLeds();

	for(unsigned char i=0;i<2;++i)
	{
		oscWave[i] = (unsigned char)engine_.getOsc(i).getWaveform();
		oscLevel[i] = engine_.getOsc(i).getLevel();
		oscPulseIndex[i] = engine_.getOsc(i).getPulseIndex();
	}
	oscWave[2] = (unsigned char)engine_.getFxSource();
	oscLevel[2] = engine_.getFxLevel();
	if(filtType!=(unsigned char)engine_.getFilter().getType())
	{
		filtType = (unsigned char)engine_.getFilter().getType();
		filtBuf[0] = 0;
		filtBuf[1] = 0;
	}
	filtC = engine_.getFilter().getCscaled();
	filtRC = engine_.getFilter().getRCscaled();
	engine_.poll(ticks);
	ticksPassed -= ticks;
}

//***********engine events*********************
void AteOdy::engineFunctionChanged(unsigned char func, unsigned char val)
{

	if(func==AteOdyEngine::FUNC_FILTTYPE)
	{
		switch (val)
		{
			case OdyFilter::OFF:
			hardware_.getLedSwitch(AteOscHardware::VALUE).setColour(LedRgb::OFF);
			break;
			case OdyFilter::MOZZI:
			hardware_.getLedSwitch(AteOscHardware::VALUE).setColour(LedRgb::GREEN); //red
			break;
			case OdyFilter::SIMPLE:
			hardware_.getLedSwitch(AteOscHardware::VALUE).setColour(LedRgb::RED);  //green
			break;
		}
	}
	else
	{
		hardware_.getRotEncoder(AteOscHardware::VALUE).setValue((char)val);
		if(func==AteOdyEngine::FUNC_MEM)
		{
			hardware_.getLedCircular(AteOscHardware::FUNCTION).select((unsigned char)AteOdyEngine::FUNC_QUANT);
			hardware_.getLedCircular(AteOscHardware::VALUE).select(val);
		}
		else
		{
			hardware_.getLedCircular(AteOscHardware::FUNCTION).select((unsigned char)func);
			hardware_.getLedCircular(AteOscHardware::VALUE).select(val);
		}
	}
}
void AteOdy::engineOptionChanged(unsigned char func, bool opt)
{
	if(opt==true)
	{
		hardware_.getLedSwitch(AteOscHardware::FUNCTION).setColour(LedRgb::RED);
	}
	else
	{
		hardware_.getLedSwitch(AteOscHardware::FUNCTION).setColour(LedRgb::YELLOW);
	}
}
//**************************hardware events************************************
void AteOdy::hardwareCvInputChanged(unsigned char input, unsigned int newValue)
{
	unsigned char oldVal, newVal;
	bool oldOpt, newOpt;
	if(input==AteOscHardware::CV_PITCH)
	{
		engine_.getCvPitch(0).setInput((newValue * 15)>>4);  //see excel for why * 0.9367
	}
	else if(input==AteOscHardware::CV_FILT)
	{
		engine_.getCvPitch(1).setInput((newValue * 15)>>4);  //see excel for why * 0.9367
	}
	else if(input==AteOscHardware::CV_CAPTURE)
	{

	}
	else
	{
		//engine_.getPatchPtr()->setCtrlValue((unsigned char)cHardToEngCtrl((AteOscHardware::CvInputName)input),newValue>>4);
		engine_.getPatchPtr()->setCtrlValue((unsigned char)CV_INPUT_TO_CTRL[input],newValue>>4);
	}
}
void AteOdy::hardwareGateInputChanged(unsigned char input, bool newValue)
{

}
void AteOdy::hardwareSwitchChanged(unsigned char sw, unsigned char newValue)
{
	bool opt;


	if(sw==AteOscHardware::FUNCTION && newValue==HIGH)
	{
		opt = !engine_.getPatchPtr()->getOptionValue(engine_.getFunction());
		engine_.getPatchPtr()->setOptionValue(engine_.getFunction(),opt);  //the LEDs are set by engine call back
	}

	if(sw==AteOscHardware::VALUE)
	{
		if(engine_.getFunction()==AteOdyEngine::FUNC_MEM)
		{
			if(newValue==HIGH)
			{
				hardware_.getLedSwitch(AteOscHardware::VALUE).setColour(LedRgb::RED);
			}
			else
			{
				hardware_.getLedSwitch(AteOscHardware::VALUE).flashStop();
				hardware_.getLedSwitch(AteOscHardware::VALUE).setColour(LedRgb::YELLOW);
				if(hardware_.getSwitch(sw).getHoldTime()>AteOscHardware::HOLD_EVENT_TICKS)
				{
					engine_.getPatchPtr()->writePatch(engine_.getPatchPtr()->getFunctionValue(AteOdyEngine::FUNC_MEM));
				}
				else
				{
					engine_.getPatchPtr()->readPatch(engine_.getPatchPtr()->getFunctionValue(AteOdyEngine::FUNC_MEM));
				}
			}
		}
		else
		{
			if(newValue==HIGH)
			{
				unsigned char filt = (unsigned char)engine_.getFilter().getType();
				filt++;
				if(filt>2)
				{
					filt = 0;
				}
				engine_.getPatchPtr()->setFunctionValue(AteOdyEngine::FUNC_FILTTYPE,filt);
			}
		}
	}

}

void AteOdy::hardwareSwitchHeld(unsigned char sw)
{

	if(sw==AteOscHardware::VALUE && engine_.getFunction()==AteOdyEngine::FUNC_MEM)
	{
		hardware_.getLedSwitch(AteOscHardware::VALUE).flash(8,LED_FLASH_TICKS,LED_FLASH_TICKS,hardware_.getLedSwitch(AteOscHardware::VALUE).getColour(),LedRgb::YELLOW,true);
	}
}
void AteOdy::hardwareRotaryEncoderChanged(unsigned char rotary, unsigned char newValue, bool clockwise)
{
	if(rotary==AteOscHardware::FUNCTION)
	{
		engine_.setFunction((AteOdyEngine::Func)newValue);
	}

	if(rotary==AteOscHardware::VALUE)
	{
		engine_.getPatchPtr()->setFunctionValue(engine_.getFunction(),newValue);
	}
}

void AteOdy::hardwareAudioBufferStatusChanged(unsigned char newStatus)
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