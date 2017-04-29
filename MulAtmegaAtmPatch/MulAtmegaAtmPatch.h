//MulAtmegaAtmPatchBase.h  Atmegatron Patch class
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

#ifndef __MULATMEGAATMPATCH_H__
#define __MULATMEGAATMPATCH_H__

#include <math.h>
#include <stdlib.h>
#include "MulAtmegaAtmPatchBase.h"
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
#ifndef HIGH
#define  HIGH 0x01
#endif
#ifndef LOW
#define LOW 0x00
#endif

class MulAtmegaAtmPatch
{
	//variables
	public:
	static const unsigned char PATCH_SIZE = 22;
	protected:
	private:
	static const unsigned char VOICES_MEM = 7;
	static const unsigned char VOICES_ALL = 0;
	static const unsigned char FUNC_CS = 8;
	static const unsigned char FUNCS = 16;
	static const unsigned char OPT_CS = 2;
	static const unsigned char OPTS = 8;
	static const unsigned char CTRLS = 6;
	static const unsigned char BANKS = 2;
	unsigned char funcValue_c_[VOICES_MEM][FUNC_CS] = {{16,96,0,246,0,8,5,0},{16,96,0,246,0,8,5,0},{16,96,0,246,0,8,5,0},{16,96,0,246,0,8,5,0},{16,96,0,246,0,8,5,0},{16,96,0,246,0,8,5,0},{16,96,0,246,0,8,5,0}};
	unsigned char optionValue_c_[VOICES_MEM][OPT_CS] ={{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}};
	unsigned char ctrlValue_[VOICES_MEM][BANKS][CTRLS] = {{{255,0,0,0,0,0},{255,0,0,0,0,0}},{{255,0,0,0,0,0},{255,0,0,0,0,0}},{{255,0,0,0,0,0},{255,0,0,0,0,0}},{{255,0,0,0,0,0},{255,0,0,0,0,0}},{{255,0,0,0,0,0},{255,0,0,0,0,0}},{{255,0,0,0,0,0},{255,0,0,0,0,0}},{{255,0,0,0,0,0},{255,0,0,0,0,0}}};
	MulAtmegaAtmPatchBase* base_ = NULL;
	//functions
	public:
	MulAtmegaAtmPatch(MulAtmegaAtmPatchBase* base);
	MulAtmegaAtmPatch() {}
	~MulAtmegaAtmPatch();
	//void initialize();
	void setFunctionValue(unsigned char voice, unsigned char func, unsigned char newValue);
	unsigned char getFunctionValue(unsigned char voice, unsigned char func);
	void setOptionValue(unsigned char voice, unsigned char func, bool newValue);
	bool getOptionValue(unsigned char voice, unsigned char func);
	void setCtrlValue(unsigned char voice, unsigned char bank, unsigned char ctrl, unsigned char newValue);
	unsigned char getCtrlValue(unsigned char voice, unsigned char bank, unsigned char ctrl);
	void readPatch(unsigned char voice, unsigned char patchNum);
	void writePatch(unsigned char voice, unsigned char patchNum);
	//void easterEgg(unsigned int seed);
	protected:
	private:
	MulAtmegaAtmPatch( const MulAtmegaAtmPatch &c );
	MulAtmegaAtmPatch& operator=( const MulAtmegaAtmPatch &c );
	unsigned char getVoiceMin(unsigned char voice){ return ((voice==VOICES_ALL) ? 0 : voice);}
	unsigned char getVoiceMax(unsigned char voice){ return ((voice==VOICES_ALL) ? VOICES_MEM : voice+1);}
}; //MulAtmegaAtmPatch

#endif //__MULATMEGAATMPATCH_H__
