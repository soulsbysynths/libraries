/*
//AteDrumPatch.h  Oscidrum patch -they're stored in FRAM
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


#ifndef __ATEDRUMPATCH_H__
#define __ATEDRUMPATCH_H__

#include <stdlib.h>
#include <string.h>
#include "AteDrumPatchBase.h"
#include "SsHelpers/SsHelpers.h"

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

class AteDrumPatch
{
//variables
public:
	static const unsigned char PATCH_SIZE = 32;
	static const unsigned char INSTRUMENTS = 8;
protected:
private:
	
	static const unsigned char STEPS = 16;
	static const unsigned char CTRLS = 4;
	static const unsigned char CTRLS_CS = 2;
	static const unsigned int FRAM_START_ADDR = 4096;
	unsigned int pattern_c_[INSTRUMENTS] = {0};
	unsigned char ctrlValue_c_[INSTRUMENTS][CTRLS_CS] = {{0}};
	AteDrumPatchBase* base_ = NULL;
	unsigned char patchNum_ = 255;
//functions
public:
	AteDrumPatch(AteDrumPatchBase* base);
	AteDrumPatch() {}
	~AteDrumPatch();
	void setPatternStep(unsigned char instr, unsigned char step, bool newValue);
	bool getPatternStep(unsigned char instr, unsigned char step);
	void setPattern(unsigned char instr, unsigned int newValue){pattern_c_[instr] = newValue;}
	unsigned int getPattern(unsigned char instr){return pattern_c_[instr];}
	void setCtrlValue(unsigned char instr, unsigned char ctrl, unsigned char newValue);
	unsigned char getCtrlValue(unsigned char instr, unsigned char ctrl);
	unsigned char getPatchNum(){return patchNum_;}
	void readPatch(unsigned char patchNum);
	void writePatch(unsigned char patchNum);
	void clearPattern();
protected:
private:
	AteDrumPatch( const AteDrumPatch &c );
	AteDrumPatch& operator=( const AteDrumPatch &c );

}; //AteDrumPatch

#endif //__ATEDRUMPATCH_H__
