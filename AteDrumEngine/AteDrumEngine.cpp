/*
* AteDrumEngine.cpp
*
* Created: 05/04/2017 15:28:57
* Author: paulsoulsby
*/


#include "AteDrumEngine.h"
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

unsigned char playStep = 15;  //so next rising edge = 0
volatile unsigned int playAddr[AteDrumPatch::INSTRUMENTS] = {0};
volatile unsigned int playStartAddr[AteDrumPatch::INSTRUMENTS] = {0};
volatile unsigned int playEndAddr[AteDrumPatch::INSTRUMENTS] = {0};
volatile unsigned int playEndAddrBuffer[AteDrumPatch::INSTRUMENTS] = {0};
volatile bool playEndCopy[AteDrumPatch::INSTRUMENTS] = {false};
volatile unsigned char playInc[AteDrumPatch::INSTRUMENTS] = {0};
volatile unsigned char playVolBig[AteDrumPatch::INSTRUMENTS] = {0};
volatile unsigned char playVolSmall[AteDrumPatch::INSTRUMENTS] = {0};

// default constructor
AteDrumEngine::AteDrumEngine(AteDrumEngineBase* base)
{
	construct(base);
} //AteDrumEngine

// default destructor
AteDrumEngine::~AteDrumEngine()
{
	if(patch_!=NULL)
	{
		delete patch_;
	}
} //~AteDrumEngine

void AteDrumEngine::construct(AteDrumEngineBase* base)
{
	base_ = base;
	patch_ = new AteDrumPatch(this);
}

void AteDrumEngine::initialize()
{
	AtmAudio::initialize();
	setSampleFreq(BASE_SF);
	for(unsigned char i=0;i<AteDrumPatch::INSTRUMENTS;++i)
	{
		delayLine_[i] = new DelayLine(16);
		delayLine_[i]->setClip(15);
	}
	setInstr(0);
	setStep(0);
	for(unsigned char i=0;i<AteDrumPatch::INSTRUMENTS;++i)
	{
		playStartAddr[i] = &WAVE_DATA[pgm_read_word(&(WAVE_START[i]))];
		playStartAddr[i] <<= 2;
		playEndAddr[i] = &WAVE_DATA[pgm_read_word(&(WAVE_START[i+1]))] - 1;
		playEndAddr[i] <<= 2;
		playAddr[i] = playEndAddr[i];
		playVolBig[i] = pgm_read_byte(&(VOL_SHIFT_BIG[15]));
		playVolSmall[i] = pgm_read_byte(&(VOL_SHIFT_SMALL[15]));
		calcEndAddress(i,15,4);
		volume_[i] = 15;
	}
}
void AteDrumEngine::pollRunning(unsigned char ticksPassed)
{
	if(runningTick_<RUNNING_TIMEOUT)
	{
		runningTick_ += ticksPassed;
	}
	else
	{
		resetPlayStep();
	}
}
void AteDrumEngine::setLinearFreq(unsigned char source, unsigned int newValue)
{
	linearFreq_[source] = newValue;
	unsigned int totFreq = linearFreq_[0] + linearFreq_[1];
	if(totFreq>4096)
	{
		totFreq = 4096;
	}
	if (totFreq==0)
	{
		setSampleFreq(BASE_SF);
	}
	else
	{
		setSampleFreq(AteOscPitch::calcFrequency(totFreq) << 4);
	}
}
void AteDrumEngine::setSampleFreq(unsigned long newSf)
{
	unsigned int ocr;
	unsigned char i;

	if (newSf!=sampleFreq_)
	{
		sampleFreq_ = newSf;
		ocr = F_CPU/sampleFreq_;
		OCR1A = ocr;
	}
}

void AteDrumEngine::setInstr(unsigned char newValue)
{
	instr_ = newValue;
	base_->engineInstrChanged(newValue);
}
void AteDrumEngine::setStep(unsigned char newValue)
{
	step_ = newValue;
	base_->engineStepChanged(newValue);
}
void AteDrumEngine::setBufferPatchRead(unsigned char newValue)
{
	if(runningTick_>=RUNNING_TIMEOUT)
	{
		patch_->readPatch(newValue);
		base_->enginePatchChanged(newValue);
		bufferPatchRead_ = 255;
	}
	else
	{
		bufferPatchRead_ = newValue;
	}
}
void AteDrumEngine::resetPlayStep()
{
	playStep = 15;
}
void AteDrumEngine::setPlayStep(unsigned char newValue)
{
	unsigned char stepVol;
	playStep = newValue;
	runningTick_ = 0;
	if(newValue==0 && bufferPatchRead_<8 && runningTick_<RUNNING_TIMEOUT)
	{
		patch_->readPatch(bufferPatchRead_);
		base_->enginePatchChanged(bufferPatchRead_);
		bufferPatchRead_ = 255;
	}
	for(unsigned char i=0;i<AteDrumPatch::INSTRUMENTS;++i)
	{
		if(patch_->getPatternStep(i,newValue)==true)
		{
			stepVol = delayLine_[i]->writeLine(volume_[i]);
		}
		else
		{
			stepVol = delayLine_[i]->writeLine(0);
		}
		if(stepVol>0)
		{
			playVolBig[i] = pgm_read_byte(&(VOL_SHIFT_BIG[stepVol]));
			playVolSmall[i] = pgm_read_byte(&(VOL_SHIFT_SMALL[stepVol]));
			playAddr[i] = playStartAddr[i];
		}
	}
}
unsigned char AteDrumEngine::getPlayStep()
{
	return playStep;
}
bool AteDrumEngine::getRunning()
{
	if (runningTick_<RUNNING_TIMEOUT)
	{
		return true;
	}
	else
	{
		return false;
	}
}
void AteDrumEngine::calcEndAddress(unsigned char instr, unsigned char len, unsigned char inc)
{
	if((len==length_[instr] && inc==playInc[instr]) || playEndCopy[instr]==true)
	{
		return;
	}

	length_[instr] = len;
	playInc[instr] = inc;
	if (length_[instr]==15)
	{
		playEndAddrBuffer[instr] = &WAVE_DATA[pgm_read_word(&(WAVE_START[instr+1]))] - playInc[instr];
	}
	else if(length_[instr]==0)
	{
		playEndAddrBuffer[instr] = &WAVE_DATA[pgm_read_word(&(WAVE_START[instr]))];
	}
	else
	{
		unsigned int sampLen = pgm_read_word(&(WAVE_START[instr+1])) - inc - pgm_read_word(&(WAVE_START[instr]));
		unsigned int sampPos = ((sampLen * len) >> 4) + pgm_read_word(&(WAVE_START[instr]));
		sampLen = pgm_read_word(&(WAVE_START[instr+1])) - inc;
		
		playEndAddrBuffer[instr] = &WAVE_DATA[sampLen];  //in case never meet for loop condition
		
		char prev = pgm_read_byte(&(WAVE_DATA[sampPos-1]));
		char cur;
		for(unsigned int i=sampPos;i<sampLen;++i)
		{
			cur = pgm_read_byte(&(WAVE_DATA[i]));
			if (cur>=0 && prev<0)
			{
				playEndAddrBuffer[instr] = &WAVE_DATA[i];
				break;
			}
			else
			{
				prev = cur;
			}
		}
	}
	playEndAddrBuffer[instr] <<= 2;
	playEndCopy[instr] = true;
	//playEndAddr[instr] = addr;
	//bitSet(TIMSK1,OCIE1A);
}
//****************************************patch events********************************************
void AteDrumEngine::patchCtrlChanged(unsigned char instr, unsigned char ctrl, unsigned char newValue)
{
	switch (ctrl)
	{
		case CTRL_DELAY:
		newValue >>= 4;
		if(newValue==0)
		{
			delayLine_[instr]->setFeedback(0);
			delayLine_[instr]->setLength(0);
		}
		else
		{
			delayLine_[instr]->setFeedback(128);
			delayLine_[instr]->setLength(newValue-1);
		}
		break;
		case CTRL_VOLUME:
		volume_ [instr] = newValue >> 4;
		break;
		case CTRL_PITCH:
		calcEndAddress(instr,length_[instr],(newValue >> 5) + 1);
		break;
		case CTRL_LENGTH:
		calcEndAddress(instr,newValue>>4, playInc[instr]);
		break;
	}
}

void AteDrumEngine::patchPatternChanged(unsigned char instr, unsigned char step, bool newValue)
{
	if(instr==instr_)
	{
		base_->enginePatternChanged(step,newValue);
	}
}
ISR(TIMER1_COMPA_vect)
{
	int output = 0;
	char samp = 0;
	for(unsigned char i=0;i<8;++i)
	{
		if(playAddr[i]<playEndAddr[i])
		{
			samp = pgm_read_byte(playAddr[i]>>2);
			output += (samp >> playVolBig[i]);
			output += (samp >> playVolSmall[i]);
			playAddr[i]+=playInc[i];
		}
		if(playEndCopy[i] && (playAddr[i]>=playEndAddr[i] || playAddr[i]==playStartAddr[i]))
		{
			playEndAddr[i] = playEndAddrBuffer[i];
			playAddr[i] = playEndAddr[i];
			playEndCopy[i] = false;
		}
	}
	if(output>127)
	{
		OCR2B = 255;
	}
	else if(output<-128)
	{
		OCR2B = 0;
	}
	else
	{
		OCR2B = output + 128;
	}

}
