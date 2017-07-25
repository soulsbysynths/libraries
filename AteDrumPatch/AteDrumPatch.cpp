/*
//AteDrumPatch.cpp  Oscidrum patch -they're stored in FRAM
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


#include "AteDrumPatch.h"

extern void writeMemory(const void* data, void* startAddr, size_t size);
extern void readMemory(void* data, const void* startAddr, size_t size);
extern void writeFram(const void* data, unsigned int startAddr, size_t size);
extern void readFram(void* data, unsigned int startAddr, size_t size);


// default constructor
AteDrumPatch::AteDrumPatch(AteDrumPatchBase* base)
{
	if(base!=NULL)
	{
		base_ = base;
	}
} //AteDrumPatch

// default destructor
AteDrumPatch::~AteDrumPatch()
{
	if(base_!=NULL)
	{
		delete base_;
	}
} //~AteDrumPatch

void AteDrumPatch::setPatternStep(unsigned char instr, unsigned char step, bool newValue)
{
	if(newValue==true)
	{
		bitSet(pattern_c_[instr],step);
	}
	else
	{
		bitClear(pattern_c_[instr],step);
	}
	if(base_!=NULL)
	{
		base_->patchPatternChanged(instr, step, newValue);
	}
}

bool AteDrumPatch::getPatternStep(unsigned char instr, unsigned char step)
{
	return (bool)bitRead(pattern_c_[instr],step);
}

void AteDrumPatch::setCtrlValue(unsigned char instr, unsigned char ctrl, unsigned char newValue)
{
	unsigned char i = ctrl>>1;
	ctrlValue_c_[instr][i] = compressFourBit(ctrlValue_c_[instr][i],newValue,ctrl%2);
	if(base_!=NULL)
	{
		base_->patchCtrlChanged(instr,ctrl,newValue);
	}
}
unsigned char AteDrumPatch::getCtrlValue(unsigned char instr, unsigned char ctrl)
{
	unsigned char i = ctrl>>1;
	return uncompressFourBit(ctrlValue_c_[instr][i],ctrl%2);
}

void AteDrumPatch::writePatch(unsigned char patchNum)
{
	unsigned int startAddr = ((unsigned int)PATCH_SIZE * patchNum) + FRAM_START_ADDR;
	unsigned int addr = 0;
	unsigned char i,j;
	unsigned char data[PATCH_SIZE];
	for(i=0;i<INSTRUMENTS;++i)
	{
		data[addr] =  pattern_c_[i] & 0xFF;
		addr++;
		data[addr] = pattern_c_[i] >> 8;
		addr++;
	}
	for(i=0;i<INSTRUMENTS;++i)
	{
		for(j=0;j<CTRLS_CS;++j)
		{
			data[addr] = ctrlValue_c_[i][j];
			addr++;
		}
	}
	//writeMemory((const void*)data, (void*)startAddr, sizeof(data));
	writeFram((const void*)data,startAddr,sizeof(data));
	patchNum_ = patchNum;
}
void AteDrumPatch::readPatch(unsigned char patchNum)
{
	unsigned int startAddr = ((unsigned int)PATCH_SIZE * patchNum) + FRAM_START_ADDR;
	unsigned int addr = 0;
	unsigned int pat;
	unsigned char i,j;
	unsigned char data[PATCH_SIZE];
	bool blank = true;
	//readMemory((void*)data,(const void*)startAddr, sizeof(data));
	readFram((void*)data,startAddr,sizeof(data));
	for(i=0;i<INSTRUMENTS;++i)
	{
		if(data[addr]<0xFF || data[addr+1]<0xFF)
		{
			blank = false;
		}
		pat = (data[addr+1] << 8) + data[addr];
		setPattern(i,pat);
		addr+=2;
	}
	for(i=0;i<INSTRUMENTS;++i)
	{
		for(j=0;j<CTRLS_CS;++j)
		{
			if(data[addr]<0xFF)
			{
				blank = false;
			}
			setCtrlValue(i,j<<1,uncompressFourBit(data[addr],false));
			setCtrlValue(i,(j<<1)+1,uncompressFourBit(data[addr],true));
			addr++;
		}
	}
	if(blank)
	{
		for(i=0;i<INSTRUMENTS;++i)
		{
			setPattern(i,0);  //blank
			setCtrlValue(i,0,15);  //vol
			setCtrlValue(i,1,0);  //del
			setCtrlValue(i,2,7);  //pitch
			setCtrlValue(i,3,15);  // len
		}
	}
	patchNum_ = patchNum;
}
void AteDrumPatch::clearPattern()
{
	memset(pattern_c_,0,sizeof(pattern_c_));
}