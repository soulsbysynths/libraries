//AteOscPatchBase.cpp  Atmegatron Patch class
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

#include "AteOscPatch.h"

extern void writeMemory(const void* data, void* startAddr, size_t size);
extern void readMemory(void* data, const void* startAddr, size_t size);
// default constructor
AteOscPatch::AteOscPatch(AteOscPatchBase* base)
{
	if(base!=NULL)
	{
		base_ = base;
	}
} //AteOscPatch

// default destructor
AteOscPatch::~AteOscPatch()
{
	if(base_!=NULL)
	{
		delete base_;
	}
} //~AteOscPatch

void AteOscPatch::setFunctionValue(unsigned char func, unsigned char newValue)
{
	unsigned char i = func>>1;
	funcValue_c_[i] = compressFourBit(funcValue_c_[i],newValue,func%2);
	if(base_!=NULL)
	{
		base_->patchValueChanged(func,newValue);
	}
}

unsigned char AteOscPatch::getFunctionValue(unsigned char func)
{
	unsigned char i = func>>1;
	return uncompressFourBit(funcValue_c_[i],func%2);
}
void AteOscPatch::setOptionValue(unsigned char func, bool newValue)
{
	bitWrite(optionValue_c_,func,newValue);
	if(base_!=NULL)
	{
		base_->patchOptionChanged(func,newValue);
	}
}
bool AteOscPatch::getOptionValue(unsigned char func)
{
	return (bool)bitRead(optionValue_c_,func);
}
void AteOscPatch::setCtrlValue(unsigned char ctrl, unsigned char newValue)
{
	ctrlValue_[ctrl] = newValue;
	if(base_!=NULL)
	{
		base_->patchCtrlChanged(ctrl,newValue);
	}
}
unsigned char AteOscPatch::getCtrlValue(unsigned char ctrl)
{
	return ctrlValue_[ctrl];
}
void AteOscPatch::writePatch(unsigned char patchNum)
{
	unsigned int startAddr = (unsigned int)PATCH_SIZE * patchNum;
	unsigned int addr = 0;
	unsigned char i;
	unsigned char data[PATCH_SIZE];
	for(i=0;i<FUNC_CS;++i)
	{
		data[addr] = funcValue_c_[i];
		addr++;
	}
	for(i=0;i<CTRLS;++i)
	{
		data[addr] = ctrlValue_[i];
		addr++;
	}
	data[addr] = optionValue_c_;
	addr++;
	writeMemory((const void*)data, (void*)startAddr, sizeof(data));
}
void AteOscPatch::readPatch(unsigned char patchNum)
{
	unsigned int startAddr = (unsigned int)PATCH_SIZE * patchNum;
	unsigned int addr = 0;
	unsigned char i;
	unsigned char data[PATCH_SIZE];
	readMemory((void*)data,(const void*)startAddr, sizeof(data));
	for(i=0;i<FUNC_CS;++i)
	{
		setFunctionValue(i<<1,uncompressFourBit(data[addr],false));
		setFunctionValue((i<<1)+1,uncompressFourBit(data[addr],true));
		addr++;
	}
	for(i=0;i<CTRLS;++i)
	{
		setCtrlValue(i,data[addr]);
		addr++;
	}
	for(i=0;i<OPTS;++i)
	{
		setOptionValue(i,(bool)bitRead(data[addr],i));
	}
	addr++;
}
void AteOscPatch::easterEgg(unsigned int seed)
{
	unsigned char i;
	srand(seed);
	for(i=0;i<(FUNCS-1);++i)  //don't touch patch num
	{
		setFunctionValue(i,rand()>>11);
		setOptionValue(i,(bool)(rand()>>14));
	}
	for(i=0;i<CTRLS;++i)
	{
		setCtrlValue(i,rand()>>7);
	}
}