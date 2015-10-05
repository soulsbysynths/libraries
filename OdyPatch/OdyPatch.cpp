/* 
* OdyPatch.cpp
*
* Created: 23/09/2015 13:55:47
* Author: Paul Soulsby
*/


#include "OdyPatch.h"
extern void writeMemory(const void* data, void* startAddr, size_t size);
extern void readMemory(void* data, const void* startAddr, size_t size);
// default constructor
OdyPatch::OdyPatch(OdyPatchBase* base)
{
	if(base!=NULL)
	{
		base_ = base;
	}
} //OdyPatch

// default destructor
OdyPatch::~OdyPatch()
{
	if(base_!=NULL)
	{
		delete base_;
	}
} //~OdyPatch

void OdyPatch::setFunctionValue(unsigned char func, unsigned char newValue)
{
	unsigned char i = func>>1;
	funcValue_c_[i] = compressFourBit(funcValue_c_[i],newValue,(bool)(func%2));
	if(base_!=NULL)
	{
		base_->patchValueChanged(func,newValue);
	}
}

unsigned char OdyPatch::getFunctionValue(unsigned char func)
{
	unsigned char i = func>>1;
	return uncompressFourBit(funcValue_c_[i],(bool)(func%2));
}
void OdyPatch::setOptionValue(unsigned char func, bool newValue)
{
	unsigned char i = func>>3;
	unsigned char b = func - (8*i);
	bitWrite(optionValue_c_[i],b,newValue);
	if(base_!=NULL)
	{
		base_->patchOptionChanged(func,newValue);
	}
}
bool OdyPatch::getOptionValue(unsigned char func)
{
	unsigned char i = func>>3;
	unsigned char b = func - (8*i);
	return (bool)bitRead(optionValue_c_[i],b);
}
void OdyPatch::setCtrlValue(unsigned char ctrl, unsigned char newValue)
{
	ctrlValue_[ctrl] = newValue;
	if(base_!=NULL)
	{
		base_->patchCtrlChanged(ctrl,newValue);
	}
}
unsigned char OdyPatch::getCtrlValue(unsigned char ctrl)
{
	return ctrlValue_[ctrl];
}
void OdyPatch::writePatch(unsigned char patchNum)
{
	unsigned int startAddr = (unsigned int)PATCH_SIZE * patchNum;
	unsigned int addr = 0;
	unsigned char i,j;
	unsigned char data[PATCH_SIZE];
	for(i=0;i<10;++i)
	{
		data[addr] = funcValue_c_[i];
		addr++;
	}
	for(i=0;i<6;++i)
	{
		data[addr] = ctrlValue_[i];
		addr++;
	}
	for(i=0;i<2;++i)
	{
		data[addr] = optionValue_c_[i];
		addr++;
	}
	writeMemory((const void*)data, (void*)startAddr, sizeof(data));
}
void OdyPatch::readPatch(unsigned char patchNum)
{
	unsigned int startAddr = (unsigned int)PATCH_SIZE * patchNum;
	unsigned int addr = 0;
	unsigned char i,j;
	unsigned char data[PATCH_SIZE];
	readMemory((void*)data,(const void*)startAddr, sizeof(data));
	for(i=0;i<10;++i)
	{
		setFunctionValue(i<<1,uncompressFourBit(data[addr],false));
		setFunctionValue((i<<1)+1,uncompressFourBit(data[addr],true));
		addr++;
	}
	for(i=0;i<6;++i)
	{
		setCtrlValue(i,data[addr]);
		addr++;
	}
	for(i=0;i<2;++i)
	{
		for(j=0;j<8;++j)
		{
			setOptionValue((i*8)+j,(bool)bitRead(data[addr],j));
		}
		addr++;
	}
}