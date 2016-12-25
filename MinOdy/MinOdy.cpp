//MinOdy.cpp  miniAtmegatron class (add this object to Arduino sketch)
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

#include "MinOdy.h"
#include "OdyOscillatorProgmem.h"

#define MICROSECONDS_PER_TIMER2_OVERFLOW 64
#define MICROSECONDS_PER_MILLSECOND 1000
#define min(a) ((a < 127) ? a : 127)
#define max(a) ((a < -127) ? -127 : a)
#define clip(a) min(max(a))

static const char RM_LEVEL[8] PROGMEM = {0,21,34,48,65,83,105,127};
static const char SCALE = 7;
static const unsigned char SQ_PULSE_INDEX = 127;
volatile static unsigned char ticksPassed = 0;
volatile static char hpfFc = 0;
volatile static char hpfFcInc = 127;
volatile static unsigned char ampMult = 0;
volatile static unsigned char ampBs = 7;
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
	static int hpfLpfOutput = 0;
	//static int filtBuf[2];
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
			//if ((index[0] > SQ_PULSE_INDEX) ^ (index[1] > oscPulseIndex[1]))
			//{
			//output += (char)pgm_read_byte(&(RM_LEVEL[oscLevel[2]]));
			//}
			//else
			//{
			//output -= (char)pgm_read_byte(&(RM_LEVEL[oscLevel[2]]));
			//}
		}
		
		output = clip(output>>1);
	}
	else if(processStage==2)
	{
		if(filtType==OdyFilter::MOZZI)
		{
			filtBuf[0] += (output*filtC - filtBuf[0]*filtC + filtBuf[0]*filtRC - filtBuf[1]*filtRC)>>SCALE;  //filtRC is prescaled here
			filtBuf[1] += (filtBuf[0]*filtC - filtBuf[1]*filtC)>>SCALE;
		}
		else if(filtType==OdyFilter::SIMPLE)
		{
			filtBuf[0] -= (filtBuf[0]*filtRC - filtBuf[1]*filtC + output*filtC)>>SCALE;
			filtBuf[1] -= (filtBuf[1]*filtRC + filtBuf[0]*filtC)>>SCALE;
		}
		else
		{
			filtBuf[1] = output;
		}
	}
	else
	{
		filtBuf[1] = clip(filtBuf[1]);
		hpfLpfOutput = ((filtBuf[1] * hpfFc) + (hpfLpfOutput * hpfFcInc))>>SCALE;
		output = clip(((filtBuf[1] - hpfLpfOutput) * ampMult) >> SCALE);
		OCR2B = (unsigned char)(output + 127);
	}

}

// default constructor
MinOdy::MinOdy() : engine_(MinOdyEngine::getInstance()), hardware_(MinHardware::getInstance())
{
	engine_.construct(this);
	hardware_.construct(this);
} //MinOdy

// default destructor
MinOdy::~MinOdy()
{
} //~MinOdy

void MinOdy::initialize()
{
	unsigned char i;
	hardware_.beginMidi(MIDI_UBRR);
	for(i=0;i<2;++i)
	{
		hardware_.getLed(i).setColour(LedRgb::RED);
	}
	hardware_.refreshLEDs();
	//hardware_.setFirstBoot(true);
	engine_.initialize();
	if(hardware_.getFirstBoot()==true)
	{
		engine_.initPatch();
	}
	engine_.setFunction(MinOdyEngine::FUNC_PATTERN);
	engine_.setBank(hardware_.getSwitch(MinHardware::SW_BANK).getState());
}

void MinOdy::poll()
{
	unsigned char ticks;

	ticks = ticksPassed;

	if(ticks>0)
	{
		hardware_.pollMidi();
		hardware_.pollAnlControls(ticks);
		hardware_.pollSwitches(ticks);
		hardware_.refreshFlash(ticks);
		refreshValueLed();
		hardware_.refreshLEDs();
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
	if(filtType!=(unsigned char)engine_.getFilter().getType())
	{
		filtType = (unsigned char)engine_.getFilter().getType();
		filtBuf[0] = 0;
		filtBuf[1] = 0;
	}
	filtC = engine_.getFilter().getCscaled();
	filtRC = engine_.getFilter().getRCscaled();

}
//***********engine events*********************
void MinOdy::engineValueChanged(unsigned char func, unsigned char val, bool opt)
{
	bool col;
	hardware_.getLed(MinHardware::LED_FUNC).setColour((LedRgb::LedRgbColour)func);
}
void MinOdy::engineBankChanged(unsigned char bank)
{
	//shown physically on switch
}
void MinOdy::engineMidiTransmit(unsigned char data)
{
	hardware_.writeMidi(data);
}

void MinOdy::refreshValueLed()
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
void MinOdy::hardwareAnalogueControlChanged(unsigned char control, unsigned char newValue)
{
	engine_.getPatchPtr()->setCtrlValue(engine_.getBank(),control,newValue);
}

void MinOdy::hardwareSwitchChanged(unsigned char switch_, unsigned char newValue)
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
				engine_.setFunction(MinOdyEngine::FUNC_PATTERN);
			}
			else
			{
				engine_.setFunction((MinOdyEngine::Func)(func-1));
			}
		}

		if(switch_==MinHardware::SW_FUNC_INC)
		{
			if(func==7)
			{
				engine_.setFunction(MinOdyEngine::FUNC_WAVE);
			}
			else
			{
				engine_.setFunction((MinOdyEngine::Func)(func+1));
			}
		}

		if(switch_==MinHardware::SW_VAL_DEC)
		{
			if(hardware_.getSwitch(MinHardware::SW_FUNC_INC).getState()==HIGH)
			{
				val = engine_.getPatchPtr()->getFunctionValue(MinOdyEngine::FUNC_BPM);
				if(val>0)
				{
					val--;
					engine_.getPatchPtr()->setFunctionValue(MinOdyEngine::FUNC_BPM,val);
				}
			}
			else
			{
				val = engine_.getPatchPtr()->getFunctionValue((MinOdyEngine::Func)func);
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
				val = engine_.getPatchPtr()->getFunctionValue(MinOdyEngine::FUNC_BPM);
				if(val<15)
				{
					val++;
					engine_.getPatchPtr()->setFunctionValue(MinOdyEngine::FUNC_BPM,val);
				}
			}
			else
			{
				val = engine_.getPatchPtr()->getFunctionValue((MinOdyEngine::Func)func);
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
void MinOdy::hardwareSwitchHeld(unsigned char switch_)
{
	unsigned char col = (unsigned char)hardware_.getLed(MinHardware::LED_FUNC).getColour();
	unsigned char invcol = ~col & 0x07;
	if(switch_==MinHardware::SW_FUNC_DEC)
	{
		engine_.getPatchPtr()->writePatch(MinOdyEngine::PATCH_NUM);
		hardware_.getLed(MinHardware::LED_FUNC).flash(4,FLASH_TICKS,FLASH_TICKS,(LedRgb::LedRgbColour)col,(LedRgb::LedRgbColour)invcol,true);
		hardware_.setFirstBoot(false);
	}
}

void MinOdy::hardwareMidiReceived(unsigned char data)
{
	engine_.getMidiPtr()->read(data);
}