//AteOscOscillator.cpp  Atmegatron Oscillator class
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

#include "AteOscOscillator.h"

//extern void writeMemory(const void* data, void* startAddr, size_t size);
//extern void readMemory(void* data, const void* startAddr, size_t size);
extern void writeFram(const void* data, unsigned int startAddr, size_t size);
extern void readFram(void* data, unsigned int startAddr, size_t size);
extern bool getFramBusy();

// default constructor
AteOscOscillator::AteOscOscillator()
{
	wavetable_ = new Wavetable(OSC_WAVELEN);
	resetWavetable();
} //AteOscOscillator

// default destructor
AteOscOscillator::~AteOscOscillator()
{
	if(wavetable_ != NULL)
	{
		delete wavetable_;
	}
} //~AteOscOscillator

void AteOscOscillator::copyWavetable(Wavetable& destWavetable)
{
	unsigned char i = 0;
	unsigned char pos = 0;
	
	unsigned char destWaveLen = destWavetable.getWaveLength();
	unsigned char jump;

	if(wavetable_ != NULL)
	{
		if(table_==15 && bank_==0)
		{
			for(i=0;i<destWaveLen;++i)
			{
				destWavetable.setSample(i,rand()>>7);
			}
		}
		else
		{
			//destWavetable = *userWavetable_;
			jump = OSC_WAVELEN / destWaveLen;
			for(i=0;i<destWaveLen;++i)
			{
				destWavetable.setSample(i,wavetable_->getSample(pos));
				pos += jump;
			}
		}
	}
}
void AteOscOscillator::setBank(unsigned char newBank)
{
	bank_ = newBank;
	readWavetable();
}
void AteOscOscillator::setTable(unsigned char newTable)
{
	table_ = newTable;
	readWavetable();
}
void AteOscOscillator::setWavetableSample(unsigned char index,char newSample)
{
	wavetable_->setSample(index,newSample);
}
char AteOscOscillator::getWavetableSample(unsigned char index)
{
	return wavetable_->getSample(index);
}
void AteOscOscillator::writeWavetable()
{
	unsigned int waveNum = (bank_ * OSC_TABLES) + table_;
	unsigned int addr  = waveNum * OSC_WAVELEN;
	unsigned char data[OSC_WAVELEN];
	for (unsigned char i=0;i<OSC_WAVELEN;++i)
	{
		data[i] = (unsigned char)wavetable_->getSample(i);
	}
	writeFram((const void*)data, addr, sizeof(data));
}
void AteOscOscillator::readWavetable()
{
	unsigned int waveNum = (bank_ * OSC_TABLES) + table_;
	unsigned int addr = waveNum * OSC_WAVELEN;
	unsigned char data[OSC_WAVELEN];
	readFram((void*)data, addr, sizeof(data));
	for (unsigned char i=0;i<OSC_WAVELEN;++i)
	{
		wavetable_->setSample(i,(char)data[i]);
	}
}
void AteOscOscillator::resetWavetable()
{
	for(unsigned char i=0;i<OSC_WAVELEN;++i)
	{
		wavetable_->setSample(i,(char)pgm_read_byte(&(OSC_WAVETABLE[bank_][table_][i])));
	}
}
void AteOscOscillator::resetFactory()
{
	unsigned int waveNum;
	unsigned int addr;
	unsigned char data[OSC_WAVELEN];
	for(unsigned char i=0;i<OSC_BANKS;++i)
	{
		for(unsigned char j=0;j<OSC_TABLES;++j)
		{
			while (getFramBusy())
			{
			}
			waveNum = (i * OSC_TABLES) + j;
			addr  = waveNum * OSC_WAVELEN;
			for (unsigned char k=0;k<OSC_WAVELEN;++k)
			{
				data[k] = pgm_read_byte(&(OSC_WAVETABLE[i][j][k]));
			}
			writeFram((const void*)data, addr, sizeof(data));			
		}
	}
	resetWavetable();
}