/* 
* AteDrumPatch.cpp
*
* Created: 05/04/2017 15:29:54
* Author: paulsoulsby
*/


#include "AteDrumPatch.h"

extern void writeMemory(const void* data, void* startAddr, size_t size);
extern void readMemory(void* data, const void* startAddr, size_t size);

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
	ctrlValue_[instr][ctrl] = newValue;
	if(base_!=NULL)
	{
		base_->patchCtrlChanged(instr,ctrl,newValue);
	}
}
unsigned char AteDrumPatch::getCtrlValue(unsigned char instr, unsigned char ctrl)
{
	return ctrlValue_[instr][ctrl];
}

void AteDrumPatch::writePatch(unsigned char patchNum)
{
	unsigned int startAddr = (unsigned int)PATCH_SIZE * patchNum;
	unsigned int addr = 0;
	unsigned char i,j;
	unsigned char data[PATCH_SIZE];
	for(i=0;i<INSTRUMENTS;++i)
	{
			data[addr] = pattern_c_[i] >> 8;
			addr++;
			data[addr] = pattern_c_[i] & 0xFF;
			addr++;
	}
	for(i=0;i<INSTRUMENTS;++i)
	{	
		for(j=0;j<CTRLS;++j)
		{
			data[addr] = ctrlValue_[i][j];
			addr++;
		}
	}
	writeMemory((const void*)data, (void*)startAddr, sizeof(data));
	patchNum_ = patchNum;
}
void AteDrumPatch::readPatch(unsigned char patchNum)
{
	unsigned int startAddr = (unsigned int)PATCH_SIZE * patchNum;
	unsigned int addr = 0;
	unsigned int pat;
	unsigned char i,j;
	unsigned char data[PATCH_SIZE];
	readMemory((void*)data,(const void*)startAddr, sizeof(data));
	for(i=0;i<INSTRUMENTS;++i)
	{
		pat = data[addr] << 8;
		addr++;
		pat |= data[addr];
		addr++;
		for(j=0;j<STEPS;++j)
		{
			setPatternStep(i,j,(bool)bitRead(pat,j));
		}
	}
	for(i=0;i<INSTRUMENTS;++i)
	{
		for(j=0;j<CTRLS;++j)
		{
			data[addr] = ctrlValue_[i][j];
			addr++;
		}
	}
	patchNum_ = patchNum;
}
void AteDrumPatch::clearPattern()
{
	memset(pattern_c_,0,sizeof(pattern_c_));
}