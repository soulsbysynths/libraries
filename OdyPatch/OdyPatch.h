//OdyPatch.h  Read and save patch.  No easter egg at present.  There's > 16 functions, to allow for the extra envelope and filter ones.
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



#ifndef __ODYPATCH_H__
#define __ODYPATCH_H__

#include <math.h>
#include <stdlib.h>
#include "OdyPatchBase.h"
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

class OdyPatch
{
//variables
public:
	static const unsigned char PATCH_SIZE = 22;
protected:
private:
	//alt bootup patch
	//unsigned char funcValue_c_[10] = {0x00,0xF0,0xA0,0xF6,0x0F,0x6F,0x72,0x00,0x06,0x10};
	//unsigned char optionValue_c_[2] = {0x58,0x03};
	//unsigned char ctrlValue_[6] = {0xFF,0x00,0x60,0x55,0x90,0x00};
	unsigned char funcValue_c_[10] = {0x00,0x00,0x00,0xF0,0x0F,0x08,0x40,0x00,0x04,0x20};
	unsigned char optionValue_c_[3] = {0x80,0x00,0x00};
	unsigned char ctrlValue_[6] = {0xFF,0x00,0xC0,0x80,0x00,0x00};
	OdyPatchBase* base_ = NULL;
//functions
public:
	OdyPatch(OdyPatchBase* base);
	OdyPatch() {}
	~OdyPatch();
	void setFunctionValue(unsigned char func, unsigned char newValue);
	unsigned char getFunctionValue(unsigned char func);
	void setOptionValue(unsigned char func, bool newValue);
	bool getOptionValue(unsigned char func);
	void setCtrlValue(unsigned char ctrl, unsigned char newValue);
	unsigned char getCtrlValue(unsigned char ctrl);
	void readPatch(unsigned char patchNum);
	void writePatch(unsigned char patchNum);
protected:
private:
	OdyPatch( const OdyPatch &c );
	OdyPatch& operator=( const OdyPatch &c );
}; //OdyPatch

#endif //__ODYPATCH_H__
