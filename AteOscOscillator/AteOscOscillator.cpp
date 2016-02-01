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

extern void writeMemory(const void* data, void* startAddr, size_t size);
extern void readMemory(void* data, const void* startAddr, size_t size);

// default constructor
AteOscOscillator::AteOscOscillator(unsigned int userWaveStartAddress)
{
	userWavetable_ = new Wavetable(OSC_USER_WAVELEN);
	userWaveStartAddress_ = userWaveStartAddress;
	readUserWave(0);
} //AteOscOscillator

// default destructor
AteOscOscillator::~AteOscOscillator()
{
	if(userWavetable_ != NULL)
	{
		delete userWavetable_;
	}
} //~AteOscOscillator

void AteOscOscillator::copyWavetable(Wavetable& destWavetable)
{
	unsigned char i = 0;
	unsigned char pos = 0;
	
	unsigned char destWaveLen = destWavetable.getWaveLength();
	unsigned char jump;

	if(userMode_==true)
	{
		if(userWavetable_ != NULL)
		{
			//destWavetable = *userWavetable_;	
			jump = OSC_USER_WAVELEN / destWaveLen;			
			for(i=0;i<destWaveLen;++i)
			{
				destWavetable.setSample(i,userWavetable_->getSample(pos));
				pos += jump;				
			}		
		}
	}
	else
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
			jump = OSC_WAVELEN / destWaveLen;
			for(i=0;i<destWaveLen;++i)
			{
				destWavetable.setSample(i,pgm_read_byte(&(OSC_WAVETABLE[bank_][table_][pos])));
				pos += jump;
			}
		}
	}
}

void AteOscOscillator::setTable(unsigned char newTable)
{
	table_ = newTable;
	if(userMode_==true)
	{
		readUserWave(newTable);
	}
}
void AteOscOscillator::setUserMode(bool newUserMode)
{
	userMode_ = newUserMode;
	if(userMode_==true)
	{
		readUserWave(table_);
	}
}
void AteOscOscillator::setUserWavetableSample(unsigned char index,char newSample)
{
	userWavetable_->setSample(index,newSample);
}
char AteOscOscillator::getUserWavetableSample(unsigned char index)
{
	return userWavetable_->getSample(index);
}
void AteOscOscillator::writeUserWave(unsigned char waveNum)
{
	//unsigned int addr  = ((unsigned int)waveNum * waveLength_) + userWaveStartAddress_;
	//unsigned char data[waveLength_];
	//for (unsigned char i=0;i<waveLength_;++i)
	//{
	//data[i] = userWavetable_->getSample(i);
	//}
	//writeMemory((const void*)data, (void*)addr, sizeof(data));
}
void AteOscOscillator::readUserWave(unsigned char waveNum)
{
	//unsigned int addr = ((unsigned int)waveNum * waveLength_) + userWaveStartAddress_;
	//unsigned char data[waveLength_];
	//readMemory((void*)data,(const void*)addr, sizeof(data));
	//for (unsigned char i=0;i<waveLength_;++i)
	//{
	//userWavetable_->setSample(i,(char)data[i]);
	//}
}