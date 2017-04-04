//MulAtmegaAtmPatchBase.cpp  Atmegatron Patch class
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

#include "MulAtmegaAtmPatch.h"

extern void writeMemory(const void* data, void* startAddr, size_t size);
extern void readMemory(void* data, const void* startAddr, size_t size);
// default constructor
MulAtmegaAtmPatch::MulAtmegaAtmPatch(MulAtmegaAtmPatchBase* base)
{
	if(base!=NULL)
	{
		base_ = base;
	}
} //MulAtmegaAtmPatch

// default destructor
MulAtmegaAtmPatch::~MulAtmegaAtmPatch()
{
	if(base_!=NULL)
	{
		delete base_;
	}
} //~MulAtmegaAtmPatch

void MulAtmegaAtmPatch::setFunctionValue(unsigned char voice, unsigned char func, unsigned char newValue)
{
	for(unsigned char v=getVoiceMin(voice);v<getVoiceMax(voice);++v)
	{
		unsigned char i = func>>1;
		funcValue_c_[v][i] = compressFourBit(funcValue_c_[v][i],newValue,func%2);
	}
	if(base_!=NULL)
	{
		base_->patchValueChanged(voice,func,newValue);
	}
}

unsigned char MulAtmegaAtmPatch::getFunctionValue(unsigned char voice, unsigned char func)
{
	unsigned char i = func>>1;
	return uncompressFourBit(funcValue_c_[getVoiceMin(voice)][i],func%2);
}
void MulAtmegaAtmPatch::setOptionValue(unsigned char voice, unsigned char func, bool newValue)
{
	for(unsigned char v=getVoiceMin(voice);v<getVoiceMax(voice);++v)
	{
		unsigned char i = func>>3;
		unsigned char b = func - (8*i);
		bitWrite(optionValue_c_[v][i],b,newValue);
	}
	if(base_!=NULL)
	{
		base_->patchOptionChanged(voice,func,newValue);
	}
}
bool MulAtmegaAtmPatch::getOptionValue(unsigned char voice, unsigned char func)
{
	unsigned char i = func>>3;
	unsigned char b = func - (8*i);
	return (bool)bitRead(optionValue_c_[getVoiceMin(voice)][i],b);
}
void MulAtmegaAtmPatch::setCtrlValue(unsigned char voice, unsigned char bank, unsigned char ctrl, unsigned char newValue)
{
	for(unsigned char v=getVoiceMin(voice);v<getVoiceMax(voice);++v)
	{
		ctrlValue_[v][bank][ctrl] = newValue;
	}
	if(base_!=NULL)
	{
		base_->patchCtrlChanged(voice,bank,ctrl,newValue);
	}
}
unsigned char MulAtmegaAtmPatch::getCtrlValue(unsigned char voice, unsigned char bank, unsigned char ctrl)
{
	return ctrlValue_[getVoiceMin(voice)][bank][ctrl];
}
void MulAtmegaAtmPatch::writePatch(unsigned char voice, unsigned char patchNum)
{
	if(patchNum>6) return;
	unsigned int startAddr = ((unsigned int)PATCH_SIZE * VOICES_MEM * patchNum) + ((unsigned int)PATCH_SIZE * voice);
	unsigned int addr = 0;
	unsigned char i,j;
	unsigned char data[PATCH_SIZE];
	for(i=0;i<FUNC_CS;++i)
	{
		data[addr] = funcValue_c_[voice][i];
		addr++;
	}
	for(i=0;i<BANKS;++i)
	{
		for(j=0;j<CTRLS;++j)
		{
			data[addr] = ctrlValue_[voice][i][j];
			addr++;
		}
	}
	for(i=0;i<OPT_CS;++i)
	{
		data[addr] = optionValue_c_[voice][i];
		addr++;
	}
	writeMemory((const void*)data, (void*)startAddr, sizeof(data));
}
void MulAtmegaAtmPatch::readPatch(unsigned char voice, unsigned char patchNum)
{
	if(patchNum>6) return;
	unsigned int startAddr = ((unsigned int)PATCH_SIZE * VOICES_MEM * patchNum) + ((unsigned int)PATCH_SIZE * voice);
	unsigned int addr = 0;
	unsigned char i,j;
	unsigned char data[PATCH_SIZE];
	readMemory((void*)data,(const void*)startAddr, sizeof(data));
	for(i=0;i<FUNC_CS;++i)
	{
		setFunctionValue(voice,i<<1,uncompressFourBit(data[addr],false));
		setFunctionValue(voice,(i<<1)+1,uncompressFourBit(data[addr],true));
		addr++;
	}
	for(i=0;i<BANKS;++i)
	{
		for(j=0;j<CTRLS;++j)
		{
			setCtrlValue(voice,i,j,data[addr]);
			addr++;
		}
	}
	for(i=0;i<OPT_CS;++i)
	{
		for(j=0;j<OPTS;++j)
		{
			setOptionValue(voice,(i*OPTS)+j,(bool)bitRead(data[addr],j));
		}
		addr++;
	}
}
//void MulAtmegaAtmPatch::easterEgg(unsigned int seed)
//{
//unsigned char i,j;
//srand(seed);
//for(i=0;i<(FUNCS-1);++i)  //don't touch patch num
//{
//setFunctionValue(i,rand()>>11);
//if(i<5 || i>8)
//{
//setOptionValue(i,(bool)(rand()>>14));
//}
//}
//for(i=0;i<BANKS;++i)
//{
//for(j=0;j<CTRLS;++j)
//{
//setCtrlValue(i,j,rand()>>7);
//}
//}
//}