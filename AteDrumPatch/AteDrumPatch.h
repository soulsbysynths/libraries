/* 
* AteDrumPatch.h
*
* Created: 05/04/2017 15:29:55
* Author: paulsoulsby
*/


#ifndef __ATEDRUMPATCH_H__
#define __ATEDRUMPATCH_H__

#include <stdlib.h>
#include <string.h>
#include "AteDrumPatchBase.h"

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
	static const unsigned char PATCH_SIZE = 56;
	static const unsigned char INSTRUMENTS = 8;
protected:
private:
	
	static const unsigned char STEPS = 16;
	static const unsigned char CTRLS = 5;
	unsigned int pattern_c_[INSTRUMENTS] = {0};
	unsigned char ctrlValue_[INSTRUMENTS][CTRLS] = {{0}};
	AteDrumPatchBase* base_ = NULL;
	unsigned char patchNum_ = 255;
//functions
public:
	AteDrumPatch(AteDrumPatchBase* base);
	AteDrumPatch() {}
	~AteDrumPatch();
	void setPatternStep(unsigned char instr, unsigned char step, bool newValue);
	bool getPatternStep(unsigned char instr, unsigned char step);
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
