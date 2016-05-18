//AteOscPatchBase.h  Atmegatron Patch class
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

#ifndef __ATEOSCPATCH_H__
#define __ATEOSCPATCH_H__

#include <math.h>
#include <stdlib.h>
#include "AteOscPatchBase.h"
#include "SsHelpers.h"

#ifndef bitRead
#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#endif
#ifndef bitSet
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#endif
#ifndef bitClear
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#endif
#ifndef bitWrite
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))
#endif

class AteOscPatch
{
	//variables
	public:
	static const unsigned char PATCH_SIZE = 10;
	protected:
	private:
	static const unsigned char FUNC_CS = 4;
	static const unsigned char FUNCS = 8;
	static const unsigned char OPT_CS = 1;
	static const unsigned char OPTS = 7;
	static const unsigned char CTRLS = 5;
	unsigned char funcValue_c_[FUNC_CS] = {0x33,0x00,0x00,0x00};
	unsigned char optionValue_c_ = 0;
	unsigned char ctrlValue_[CTRLS] = {0,0,0,0,0};
	AteOscPatchBase* base_ = NULL;
	//functions
	public:
	AteOscPatch(AteOscPatchBase* base);
	AteOscPatch() {}
	~AteOscPatch();
	//void initialize();
	void setFunctionValue(unsigned char func, unsigned char newValue);
	unsigned char getFunctionValue(unsigned char func);
	void setOptionValue(unsigned char func, bool newValue);
	bool getOptionValue(unsigned char func);
	void setCtrlValue(unsigned char ctrl, unsigned char newValue);
	unsigned char getCtrlValue(unsigned char ctrl);
	void readPatch(unsigned char patchNum);
	void writePatch(unsigned char patchNum);
	void easterEgg(unsigned int seed);
	protected:
	private:
	AteOscPatch( const AteOscPatch &c );
	AteOscPatch& operator=( const AteOscPatch &c );

}; //AteOscPatch

#endif //__ATEOSCPATCH_H__
