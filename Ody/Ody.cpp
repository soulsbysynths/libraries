//Ody.cpp  Odytron class (add this object to Arduino sketch)
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

#include "Ody.h"
#include "OdyOscillatorProgmem.h"

#define MICROSECONDS_PER_TIMER2_OVERFLOW 256//64//32
#define min(a) ((a < 127) ? a : 127)
#define max(a) ((a < -127) ? -127 : a)
#define clip(a) min(max(a))

static const char RM_LEVEL[8] PROGMEM = {0,21,34,48,65,83,105,127};
static const char SCALE = 7;
static const unsigned char SQ_PULSE_INDEX = 127;
static const char OUTPUT_MIN = -127;
static const char OUTPUT_MAX = 127;
volatile static unsigned char ticksPassed = 0;
volatile static char hpfFc = 0;
volatile static char hpfFcInc = 127;
volatile static unsigned char ampMult = 0;
volatile static unsigned char ampBs = 6;
volatile static unsigned char oscWave[3];
volatile static unsigned char oscPulseIndex[2];
volatile static unsigned char oscLevel[3];
volatile static unsigned char filtType = 0;
volatile static int filtC = 0;
volatile static int filtR = 0;
volatile static int filtRC = 0;

ISR(TIMER2_OVF_vect) {

	static unsigned char processStage = 0;
	static unsigned int tickCnt = 0;
	static int output = 0;
	static int hpfLpfOutput = 0;
	static int filtBuf[2];
	//static unsigned char lastFiltType = 0;
	static unsigned char index[2];
	const char SCALE = 7;
	const unsigned char SQ_PULSE_INDEX = 127;
	


	switch  (processStage&0x03)
	{
		case 0x00:
		index[0] = OdyAudio::getWtIndex(0);
		index[1] = OdyAudio::getWtIndex(1);
		if(oscWave[0]==0)
		{
			output = (char)pgm_read_byte(&(SAW_WAVE[oscLevel[0]][index[0]]));
		}
		else
		{
			if(index[0] > SQ_PULSE_INDEX)
			{
				output = -(char)pgm_read_byte(&(SQUARE_WAVE[oscLevel[0]][index[0]-SQ_PULSE_INDEX]));//-oscPulseIndex[0]]));//osc 0 only square for now
			}
			else
			{
				output = (char)pgm_read_byte(&(SQUARE_WAVE[oscLevel[0]][index[0]]));
			}
			
		}
		if(oscWave[1]==0)
		{
			output += (char)pgm_read_byte(&(SAW_WAVE[oscLevel[1]][index[1]]));
		}
		else
		{
			if(index[1] > oscPulseIndex[1])
			{
				output -= (char)pgm_read_byte(&(SQUARE_WAVE[oscLevel[1]][index[1]-oscPulseIndex[1]]));
			}
			else
			{
				output += (char)pgm_read_byte(&(SQUARE_WAVE[oscLevel[1]][index[1]]));
			}
		}
		break;

		case 0x01:

		if(oscWave[2]==0)
		{
			output += OdyNoise::getOutput();
		}
		else
		{
			if ((index[0] > oscPulseIndex[0]) ^ (index[1] > oscPulseIndex[1]))
			{
				output += (char)pgm_read_byte(&(RM_LEVEL[oscLevel[2]]));
			}
			else
			{
				output -= (char)pgm_read_byte(&(RM_LEVEL[oscLevel[2]]));
			}
		}
		
		output = clip(output>>1);
		//output >>= 1;
		//if(output>255)
		//{
			//output = OUTPUT_MAX;
		//}
		//else if(output<-OUTPUT_MIN)
		//{
			//output = -127;
		//}
		//else
		//{
			//output >>= 1; //should be /3 not /4 (for 3 oscs), but extra headroom good for filtering
		//}
		
		//if(filtType!=lastFiltType)
		//{
			//lastFiltType = filtType;
			//filtBuf[0] = 0;
			//filtBuf[1] = 0;
		//}
		break;
		case 0x02:
		switch (filtType)
		{
			case 0:
			output -= (filtBuf[1]*filtR)>>SCALE;
			filtBuf[0] += (output*filtC - filtBuf[0]*filtC)>>SCALE;
			filtBuf[1] += (filtBuf[0]*filtC  - filtBuf[1]*filtC)>>SCALE;
			break;
			case 1:
			filtBuf[0] -= (filtBuf[0]*filtRC - filtBuf[1]*filtC + output*filtC)>>SCALE;
			filtBuf[1] -= (filtBuf[1]*filtRC + filtBuf[0]*filtC)>>SCALE;
			break;
			case 2:
			filtBuf[0] += (output*filtC - filtBuf[0]*filtC + filtBuf[0]*filtRC - filtBuf[1]*filtRC)>>SCALE;  //filtRC is prescaled here
			filtBuf[1] += (filtBuf[0]*filtC - filtBuf[1]*filtC)>>SCALE;
			break;
		}
		break;
		case 0x03:
		filtBuf[1] = clip(filtBuf[1]);
		hpfLpfOutput = ((filtBuf[1] * hpfFc) + (hpfLpfOutput * hpfFcInc))>>SCALE;
		//output = filtBuf[1] - hpfLpfOutput;

		//hpfLpfOutput = ((output * hpfFc) + (hpfLpfOutput * hpfFcInc))>>SCALE;
		//output = output - hpfLpfOutput;		
		output = clip(((filtBuf[1] - hpfLpfOutput) * ampMult) >> SCALE);//ampBs;
		OCR2B = output + 127;

		//output = (output * ampMult) >> SCALE;//ampBs;
		//if(output>127)
		//{
			//OCR2B = 254;
		//}
		//else if(output<-127)
		//{
			//OCR2B = 0;
		//}
		//else
		//{
			//OCR2B = output + 127;
		//}
		
		tickCnt += MICROSECONDS_PER_TIMER2_OVERFLOW;

		if (tickCnt >= 1000)
		{
			tickCnt -= 1000;
			ticksPassed++;
		}

		break;
	}
	processStage++;
}

// default constructor
Ody::Ody()  : engine_(OdyEngine::getInstance()), hardware_(AtmHardware::getInstance())
{
	engine_.construct(this);
	hardware_.construct(this);
} //Ody

// default destructor
Ody::~Ody()
{
} //~Ody

void Ody::initialize()
{
	
	hardware_.getRotEncoder(AtmHardware::FUNCTION).setContinuous(true);
	hardware_.getRotEncoder(AtmHardware::VALUE).setContinuous(false);
	hardware_.beginMidi(MIDI_UBRR);
	
	if(hardware_.getMidiChannelSelectMode()==false)
	{
		engine_.initialize();
		engine_.getMidiPtr()->setChannel(hardware_.getMidiChannel());
		for(unsigned char i=0;i<2;++i)
		{
			hardware_.getLedSwitch(i).flash(4,LED_FLASH_TICKS,LED_FLASH_TICKS,LedRgb::YELLOW,LedRgb::RED,true);
		}
	}
}

void Ody::poll()
{
	unsigned char ticks = ticksPassed;
	if(ticks>0)
	{
		hardware_.pollMidi();
		hardware_.pollAnlControls(ticks);
		hardware_.pollSwitches(ticks);
		hardware_.pollRotEncoders(ticks);
		hardware_.refreshFlash(ticks);
		hardware_.refreshLeds();
		engine_.poll(ticks);
		ticksPassed -= ticks;
	}

	hpfFc = engine_.getHPF().getFc()>>1;
	hpfFcInc = 127 - hpfFc;
	ampMult = engine_.getAmp().getOutput();
	for(unsigned char i=0;i<2;++i)
	{
		oscWave[i] = (unsigned char)engine_.getOsc(i).getWaveform();
		oscLevel[i] = engine_.getOsc(i).getLevel();
		oscPulseIndex[i] = engine_.getOsc(i).getPulseIndex();
	}
	oscWave[2] = (unsigned char)engine_.getFxSource();
	oscLevel[2] = engine_.getFxLevel();
	filtType = (unsigned char)engine_.getFilter().getType();
	filtC = engine_.getFilter().getCscaled();
	filtR = engine_.getFilter().getRscaled();
	filtRC = engine_.getFilter().getRCscaled();
}

//***********engine events*********************
void Ody::engineFunctionChanged(unsigned char func, unsigned char val, bool opt)
{

	if(func==OdyEngine::FUNC_FILTTYPE)
	{
		switch (val)
		{
			case 0:
			hardware_.getLedSwitch(AtmHardware::BANK).setColour(LedRgb::YELLOW);
			break;
			case 1:
			hardware_.getLedSwitch(AtmHardware::BANK).setColour(LedRgb::YELLOW); //red
			break;
			case 2:
			hardware_.getLedSwitch(AtmHardware::BANK).setColour(LedRgb::YELLOW);  //green
			break;
		}
	}
	else
	{
		hardware_.getRotEncoder(AtmHardware::VALUE).setValue((char)val);
		hardware_.getLedCircular(AtmHardware::VALUE).fill(val);
		if(func==OdyEngine::FUNC_ENVA2)
		{
			hardware_.getLedCircular(AtmHardware::FUNCTION).select((unsigned char)OdyEngine::FUNC_ENVA);
		}
		else if(func==OdyEngine::FUNC_ENVR2)
		{
			hardware_.getLedCircular(AtmHardware::FUNCTION).select((unsigned char)OdyEngine::FUNC_ENVR);
		}
		else
		{
			hardware_.getLedCircular(AtmHardware::FUNCTION).select((unsigned char)func);
		}
		if(opt==true)
		{
			hardware_.getLedSwitch(AtmHardware::FUNCTION).setColour(LedRgb::RED);
		}
		else
		{
			hardware_.getLedSwitch(AtmHardware::FUNCTION).setColour(LedRgb::YELLOW);
		}
	}
}

//**************************hardware events************************************
void Ody::hardwareAnalogueControlChanged(unsigned char control, unsigned char newValue)
{
	engine_.getPatchPtr()->setCtrlValue(control,newValue);
}

void Ody::hardwareSwitchChanged(unsigned char sw, unsigned char newValue)
{
	bool opt;

	if(sw==AtmHardware::FUNCTION)
	{
		if(engine_.getFunction()==OdyEngine::FUNC_PORTAMENTO)
		{
			if(newValue==HIGH)
			{
				hardware_.getLedSwitch(AtmHardware::FUNCTION).setColour(LedRgb::RED);
			}
			else
			{
				hardware_.getLedSwitch(AtmHardware::FUNCTION).flashStop();
				hardware_.getLedSwitch(AtmHardware::FUNCTION).setColour(LedRgb::RED);
				if(hardware_.getSwitch(sw).getHoldTime()>AtmHardware::HOLD_EVENT_TICKS)
				{
					engine_.getPatchPtr()->writePatch(0);
				}
				else
				{
					if(engine_.getFunction()==OdyEngine::FUNC_PORTAMENTO)
					{
						engine_.getPatchPtr()->readPatch(0);
					}
				}
			}
		}
		else if(newValue==HIGH)
		{
			opt = !engine_.getPatchPtr()->getOptionValue(engine_.getFunction());
			engine_.getPatchPtr()->setOptionValue(engine_.getFunction(),opt);  //the LEDs are set by engine call back
		}
	}

	
	if(sw==AtmHardware::VALUE)
	{
		if(newValue==HIGH)
		{
			hardware_.getRotEncoder(AtmHardware::VALUE).setUpdateVal(false);
			hardware_.getLedSwitch(AtmHardware::VALUE).setColour(LedRgb::RED);
			engine_.midiNoteOnReceived(testNote_,127);
		}
		else
		{
			hardware_.getRotEncoder(AtmHardware::VALUE).setUpdateVal(true);
			hardware_.getLedSwitch(AtmHardware::VALUE).setColour(LedRgb::YELLOW);
			engine_.midiNoteOffReceived(testNote_);
		}
	}
	
	if(sw==AtmHardware::BANK)
	{
		if(newValue==HIGH)
		{
			unsigned char filt = (unsigned char)engine_.getFilter().getType();
			filt++;
			if(filt>2)
			{
				filt = 0;
			}
			engine_.getPatchPtr()->setFunctionValue(OdyEngine::FUNC_FILTTYPE,filt);
		}
	}
}
void Ody::hardwareSwitchHeld(unsigned char sw)
{
	if(sw==AtmHardware::FUNCTION && engine_.getFunction()==OdyEngine::FUNC_PORTAMENTO)
	{
		hardware_.getLedSwitch(AtmHardware::FUNCTION).flash(8,LED_FLASH_TICKS,LED_FLASH_TICKS,LedRgb::RED,LedRgb::YELLOW,true);
	}
	//if(sw==AtmHardware::BANK)
	//{
	//if(engine_.getBank()==LOW)
	//{
	//hardware_.getLedSwitch(AtmHardware::BANK).flash(8,LED_FLASH_TICKS,LED_FLASH_TICKS,LedRgb::RED,LedRgb::YELLOW,true);
	//}
	//else
	//{
	//hardware_.getLedSwitch(AtmHardware::BANK).flash(8,LED_FLASH_TICKS,LED_FLASH_TICKS,LedRgb::GREEN,LedRgb::YELLOW,true);
	//}
	//engine_.getPatchPtr()->easterEgg(engine_.getMasterClock().getOutput());
	//}
}
void Ody::hardwareRotaryEncoderChanged(unsigned char rotary, unsigned char newValue, bool clockwise)
{

	if(rotary==AtmHardware::FUNCTION)
	{
		engine_.setFunction((OdyEngine::Func)newValue);
	}

	if(rotary==AtmHardware::VALUE)
	{
		if(hardware_.getSwitch(AtmHardware::VALUE).getState()==HIGH)
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
			engine_.getPatchPtr()->setFunctionValue(engine_.getFunction(),newValue);
		}
	}
}
void Ody::hardwareMidiChannelChanged(unsigned char channel)
{
	engine_.getMidiPtr()->setChannel(channel);
}
void Ody::hardwareMidiReceived(unsigned char data)
{
	hardware_.getLedMidi().flash(1,LED_FLASH_TICKS,LED_FLASH_TICKS,true);  //do at this level, not in hardware
	engine_.getMidiPtr()->read(data);
}
void Ody::hardwareMidiError(unsigned char errorType)
{
	hardware_.getLedSwitch(AtmHardware::VALUE).setColour((LedRgb::LedRgbColour)(3-(unsigned char)hardware_.getLedSwitch(AtmHardware::VALUE).getColour()));
	hardware_.getLedCircular(AtmHardware::VALUE).fill(errorType);
}

