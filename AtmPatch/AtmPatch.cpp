//AtmPatchBase.cpp  Atmegatron Patch class
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

#include "AtmPatch.h"

extern void writeMemory(const void* data, void* startAddr, size_t size);
extern void readMemory(void* data, const void* startAddr, size_t size);
// default constructor
AtmPatch::AtmPatch(AtmPatchBase* base)
{
	if(base!=NULL)
	{
		base_ = base;
	}
} //AtmPatch

// default destructor
AtmPatch::~AtmPatch()
{
	if(base_!=NULL)
	{
		delete base_;
	}
} //~AtmPatch

void AtmPatch::setFunctionValue(unsigned char func, unsigned char newValue)
{
	unsigned char i = func>>1;
	funcValue_c_[i] = compressFourBit(funcValue_c_[i],newValue,func%2);
	if(base_!=NULL)
	{
		base_->patchValueChanged(func,newValue);
	}
}

unsigned char AtmPatch::getFunctionValue(unsigned char func)
{
	unsigned char i = func>>1;
	return uncompressFourBit(funcValue_c_[i],func%2);
}
void AtmPatch::setOptionValue(unsigned char func, bool newValue)
{
	unsigned char i = func>>3;
	unsigned char b = func - (8*i);
	bitWrite(optionValue_c_[i],b,newValue);
	if(base_!=NULL)
	{
		base_->patchOptionChanged(func,newValue);
	}
}
bool AtmPatch::getOptionValue(unsigned char func)
{
	unsigned char i = func>>3;
	unsigned char b = func - (8*i);
	return (bool)bitRead(optionValue_c_[i],b);
}
void AtmPatch::setCtrlValue(unsigned char bank, unsigned char ctrl, unsigned char newValue)
{
	ctrlValue_[bank][ctrl] = newValue;
	if(base_!=NULL)
	{
		base_->patchCtrlChanged(bank,ctrl,newValue);
	}
}
unsigned char AtmPatch::getCtrlValue(unsigned char bank, unsigned char ctrl)
{
	return ctrlValue_[bank][ctrl];
}
void AtmPatch::writePatch(unsigned char patchNum)
{
	unsigned int startAddr = (unsigned int)PATCH_SIZE * patchNum;
	unsigned int addr = 0;
	unsigned char i,j;
	unsigned char data[PATCH_SIZE];
	for(i=0;i<FUNC_CS;++i)
	{
		data[addr] = funcValue_c_[i];
		addr++;		
	}
	for(i=0;i<BANKS;++i)
	{
		for(j=0;j<CTRLS;++j)
		{
			data[addr] = ctrlValue_[i][j];
			addr++;			
		}
	}
	for(i=0;i<OPT_CS;++i)
	{
		data[addr] = optionValue_c_[i];
		addr++;
	}
	writeMemory((const void*)data, (void*)startAddr, sizeof(data));
}
void AtmPatch::readPatch(unsigned char patchNum)
{
	unsigned int startAddr = (unsigned int)PATCH_SIZE * patchNum;
	unsigned int addr = 0;
	unsigned char i,j;
	unsigned char data[PATCH_SIZE];
	readMemory((void*)data,(const void*)startAddr, sizeof(data));
	for(i=0;i<FUNC_CS;++i)
	{
		setFunctionValue(i<<1,uncompressFourBit(data[addr],false));
		setFunctionValue((i<<1)+1,uncompressFourBit(data[addr],true));
		addr++;
	}
	for(i=0;i<BANKS;++i)
	{
		for(j=0;j<CTRLS;++j)
		{
			setCtrlValue(i,j,data[addr]);
			addr++;	
		}
	}
	for(i=0;i<OPT_CS;++i)
	{
		for(j=0;j<OPTS;++j)
		{
			setOptionValue((i*OPTS)+j,(bool)bitRead(data[addr],j));
		}
		addr++;
	}
}
unsigned char AtmPatch::getSysexByte(unsigned char index)
{
	if(index<FUNCS)
	{
		return getFunctionValue(index);
	}
	else if(index<(FUNCS+CTRLS))
	{
		return getCtrlValue(LOW,index-FUNCS)>>1;
	}
	else if (index<(FUNCS+CTRLS+CTRLS))
	{
		return getCtrlValue(HIGH,index-(FUNCS+CTRLS))>>1;
	}
	else
	{
		return (unsigned char)getOptionValue(index-(FUNCS+CTRLS+CTRLS));
	}
}
void AtmPatch::setSysexByte(unsigned char index, unsigned char newValue)
{
	if(index<FUNCS)
	{
		setFunctionValue(index,newValue);
	}
	else if (index<(FUNCS+CTRLS))
	{
		setCtrlValue(LOW,index-(FUNCS),newValue<<1);
	}
	else if (index<(FUNCS+CTRLS+CTRLS))
	{
		setCtrlValue(HIGH,index-(FUNCS+CTRLS),newValue<<1);
	}
	else
	{
		setOptionValue(index-(FUNCS+CTRLS+CTRLS),(bool)newValue);
	}
}
void AtmPatch::easterEgg(unsigned int seed)
{
	unsigned char i,j;
	srand(seed);
	for(i=0;i<(FUNCS-1);++i)  //don't touch patch num
	{
		setFunctionValue(i,rand()>>11);
		if(i<5 || i>8)
		{
			setOptionValue(i,(bool)(rand()>>14));
		}
	}
	for(i=0;i<BANKS;++i)
	{
		for(j=0;j<CTRLS;++j)
		{
			setCtrlValue(i,j,rand()>>7);
		}
	}
}