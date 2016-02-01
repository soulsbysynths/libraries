/* 
* Ade33LogicPatch.cpp
*
* Created: 05/01/2016 11:41:34
* Author: Paul Soulsby
*/


#include "Ade33LogicPatch.h"

extern void writeMemory(const void* data, void* startAddr, size_t size);
extern void readMemory(void* data, const void* startAddr, size_t size);

// default constructor
Ade33LogicPatch::Ade33LogicPatch(Ade33LogicPatchBase* base)
{
	if(base!=NULL)
	{
		base_ = base;
	}
} //Ade33LogicPatch

// default destructor
Ade33LogicPatch::~Ade33LogicPatch()
{
	if(base_!=NULL)
	{
		delete base_;
	}
} //~Ade33LogicPatch

void Ade33LogicPatch::setFunctionValue(unsigned char func, unsigned char newValue)
{
	funcs_[func] = newValue;
	if(base_!=NULL)
	{
		base_->patchValueChanged(func,newValue);
	}
}
void Ade33LogicPatch::writePatch(unsigned char patchNum)
{
	unsigned int startAddr = (unsigned int)PATCH_SIZE * patchNum;
	unsigned int addr = 0;
	unsigned char i,j;
	unsigned char data[PATCH_SIZE];
	data[addr] = (unsigned char)true;
	addr++;
	for(i=0;i<FUNCS;++i)
	{
		data[addr] = funcs_[i];
		addr++;
	}
	writeMemory((const void*)data, (void*)startAddr, sizeof(data));	
}
void Ade33LogicPatch::readPatch(unsigned char patchNum)
{
	unsigned int startAddr = (unsigned int)PATCH_SIZE * patchNum;
	unsigned int addr = 1;
	unsigned char i,j;
	unsigned char data[PATCH_SIZE];
	readMemory((void*)data,(const void*)startAddr, sizeof(data));
	for(i=0;i<FUNCS;++i)
	{
		setFunctionValue(i,data[addr]);
		addr++;
	}

}
bool Ade33LogicPatch::getPatchSlotUsed(unsigned char patchNum)
{
	unsigned int startAddr = (unsigned int)PATCH_SIZE * patchNum;
	unsigned char data[1];
	readMemory((void*)data,(const void*)startAddr, sizeof(data));
	return (bool)data[0];			
}
void Ade33LogicPatch::setPatchSlotUsed(unsigned char patchNum, bool newValue)
{
	unsigned int startAddr = (unsigned int)PATCH_SIZE * patchNum;
	unsigned char data[1];
	data[0] = (unsigned char)newValue;
	writeMemory((const void*)data, (void*)startAddr, sizeof(data));	
}