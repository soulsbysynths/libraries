/*
//AteDrumEngine.cpp  Oscidrum audio engine
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


#include "AteDrumEngine.h"
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

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
		delayLine_[i]->setFeedback(128);
	}
	for(unsigned char i=0;i<AteDrumPatch::INSTRUMENTS;++i)
	{
		playStartAddr[i] = pgm_read_word(&(WAVE_START[i]));
		playStartAddr[i] <<= 2;
		playEndAddr[i] = pgm_read_word(&(WAVE_START[i+1])) - 1;
		playEndAddr[i] <<= 2;
		playAddr[i] = playEndAddr[i];
		playVolBig[i] = pgm_read_byte(&(VOL_SHIFT_BIG[15]));
		playVolSmall[i] = pgm_read_byte(&(VOL_SHIFT_SMALL[15]));
		playInc[i] = 4;
		volume_[i] = 15;
	}
	setInstr(0);
	setEditStep(0);
	setPlayStep(0);
}

void AteDrumEngine::setLinearFreq(unsigned char source, unsigned int newValue)
{
	linearFreq_[source] = newValue;
	unsigned int totFreq = linearFreq_[0] + linearFreq_[1];
	if(totFreq>BASE_SF_LINEAR)
	{
		totFreq = BASE_SF_LINEAR;
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
void AteDrumEngine::setEditStep(unsigned char newValue)
{
	editStep_ = newValue;
	base_->engineEditStepChanged(newValue);
}

void AteDrumEngine::setBufferPatchRead(unsigned char newValue)
{
	if(!running_)
	{
		if(newValue!=CLEAR_PATCH_READ)
		{
			patch_->readPatch(newValue);
			base_->enginePatchChanged(newValue);
			bufferPatchRead_ = CLEAR_PATCH_READ;
		}
	}
	else
	{
		bufferPatchRead_ = newValue;
	}
}

void AteDrumEngine::setPlayStep(unsigned char newValue)
{
	playStep_ = newValue;
	base_->enginePlayStepChanged(newValue);
	if(newValue==0 && bufferPatchRead_!=CLEAR_PATCH_READ && running_)
	{
		patch_->readPatch(bufferPatchRead_);
		base_->enginePatchChanged(bufferPatchRead_);
		bufferPatchRead_ = CLEAR_PATCH_READ;
	}
}
void AteDrumEngine::playStep()
{
	unsigned char stepVol;
	for(unsigned char i=0;i<AteDrumPatch::INSTRUMENTS;++i)
	{
		if(patch_->getPatternStep(i,playStep_)==true)
		{
			stepVol = delayLine_[i]->writeLine(volume_[i]);
		}
		else
		{
			stepVol = delayLine_[i]->writeLine(0);
		}
		playInstr(i,stepVol);
	}
}

void AteDrumEngine::playInstr(unsigned char instr, unsigned char vol)
{
	if(vol>0)
	{
		playVolBig[instr] = pgm_read_byte(&(VOL_SHIFT_BIG[vol]));
		playVolSmall[instr] = pgm_read_byte(&(VOL_SHIFT_SMALL[vol]));
		playAddr[instr] = playStartAddr[instr];
	}
}

void AteDrumEngine::calcEndAddress(unsigned char instr, unsigned char len)
{
	playEndCopy[instr] = false;
	unsigned int sampStart = pgm_read_word(&(WAVE_START[instr]));
	unsigned int sampEnd = pgm_read_word(&(WAVE_START[instr+1])) - 1;
	if (len==15)
	{
		playEndAddrBuffer[instr] = sampEnd;
	}
	else if(len==0)
	{
		playEndAddrBuffer[instr] = sampStart;
	}
	else
	{

		unsigned int sampLen = sampEnd - sampStart + 1;
		unsigned int sampPos = sampStart + ((sampLen >> 4) * len) + 1;  //start at second sample, so prev = 0
		
		playEndAddrBuffer[instr] = sampEnd;  //in case never meet for loop condition
		
		char prev = pgm_read_byte(&(WAVE_DATA[sampPos-1]));
		char cur;
		for(unsigned int i=sampPos;i<=sampEnd;++i)
		{
			cur = pgm_read_byte(&(WAVE_DATA[i]));
			if (prev<0 && cur>=0)
			{
				playEndAddrBuffer[instr] = i;
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
}
//****************************************patch events********************************************
void AteDrumEngine::patchCtrlChanged(unsigned char instr, unsigned char ctrl, unsigned char newValue)
{
	newValue &= 0x0F;  //safety
	switch (ctrl)
	{
		case CTRL_DELAY:
		delayLine_[instr]->setLength(newValue);
		break;
		case CTRL_VOLUME:
		volume_ [instr] = newValue;
		break;
		case CTRL_PITCH:
		playInc[instr] = (newValue >> 1) + 1;
		break;
		case CTRL_LENGTH:
		calcEndAddress(instr,newValue);
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

ISR(TIMER1_COMPA_vect, ISR_NOBLOCK)
{
	int output = 0;
	char samp = 0;
	for(unsigned char i=0;i<8;++i)
	{
		if(playEndCopy[i] && playAddr[i]==playStartAddr[i])
		{
			playEndAddr[i] = playEndAddrBuffer[i];
			playEndCopy[i] = false;
		}
		if(playAddr[i]<=playEndAddr[i])
		{
			samp = pgm_read_byte(&WAVE_DATA[playAddr[i]>>2]);
			output += (samp >> playVolBig[i]);
			output += (samp >> playVolSmall[i]);
			playAddr[i]+=playInc[i];
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
