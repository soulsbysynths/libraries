/* 
* Ade33LogicPatch.h
*
* Created: 05/01/2016 11:41:35
* Author: Paul Soulsby
*/


#ifndef __ADE33LOGICPATCH_H__
#define __ADE33LOGICPATCH_H__

#include <stdlib.h>
#include "Ade33LogicPatchBase.h"

class Ade33LogicPatch
{
//variables
public:
	static const unsigned char PATCH_SIZE = 3;
protected:
private:
	static const unsigned char FUNCS = 2;
	static const unsigned char HIGH = 1;
	static const unsigned char LOW = 0;
	unsigned char funcs_[FUNCS] = {0,0};
	Ade33LogicPatchBase* base_ = NULL;
//functions
public:
	Ade33LogicPatch(Ade33LogicPatchBase* base);
	~Ade33LogicPatch();
	void setFunctionValue(unsigned char func, unsigned char newValue);
	unsigned char getFunctionValue(unsigned char func){return funcs_[func];}
	void readPatch(unsigned char patchNum);
	void writePatch(unsigned char patchNum);
	void setPatchSlotUsed(unsigned char patchNum, bool newValue);
	bool getPatchSlotUsed(unsigned char patchNum);
protected:
private:
	Ade33LogicPatch( const Ade33LogicPatch &c );
	Ade33LogicPatch& operator=( const Ade33LogicPatch &c );

}; //Ade33LogicPatch

#endif //__ADE33LOGICPATCH_H__
