/* 
* CD4051.h
*
* Created: 02/01/2016 14:42:04
* Author: Paul Soulsby
*/


#ifndef __CD4051_H__
#define __CD4051_H__

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

class CD4051
{
//variables
public:
protected:
private:
	unsigned char output_ = 0;
	bool inhibit_ = false;
	unsigned char flashCnt_ = 0;
	unsigned char flashOnOffTicks_c_ = 0;   //compresses on and off ticks into 4 bit
	unsigned int flashTick_ = 0;
//functions
public:
	CD4051();
	~CD4051();
	unsigned char getOutput(){return output_;}
	void setOutput(unsigned char newValue){output_ = newValue;}
	bool getInput(unsigned char input);
	void setInhibit(bool newValue){inhibit_ = newValue;}
	bool getInhibit(){return inhibit_;}
	void flash(unsigned char output, unsigned char flashes, unsigned char onTicks, unsigned char offTicks, bool startOn);
	void flashStop(bool inhibit);
	void refreshFlash(unsigned char tick_inc);
protected:
private:
	CD4051( const CD4051 &c );
	CD4051& operator=( const CD4051 &c );
	void setFlashOnTicks(unsigned char onticks);
	unsigned char getFlashOnticks();
	void setFlashOffTicks(unsigned char offticks);
	unsigned char getFlashOffTicks();
}; //CD4051

#endif //__CD4051_H__
