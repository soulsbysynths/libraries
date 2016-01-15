/* 
* CD4051.cpp
*
* Created: 02/01/2016 14:42:03
* Author: Paul Soulsby
*/


#include "CD4051.h"

// default constructor
CD4051::CD4051()
{
} //CD4051

// default destructor
CD4051::~CD4051()
{
} //~CD4051

bool CD4051::getInput(unsigned char input)
{
	if(inhibit_==true)
	{
		return (bool)bitRead(7,input); //special just for ADE33
	}
	else
	{
		return (bool)bitRead(output_,input);
	}
	
}
void CD4051::setFlashOffTicks(unsigned char offticks)
{
	flashOnOffTicks_c_ = compressFourBit(flashOnOffTicks_c_,offticks,0);
}
unsigned char CD4051::getFlashOffTicks()
{
	return uncompressFourBit(flashOnOffTicks_c_,0);
}
void CD4051::setFlashOnTicks(unsigned char onticks)
{
	flashOnOffTicks_c_ = compressFourBit(flashOnOffTicks_c_,onticks,1);
}
unsigned char CD4051::getFlashOnticks()
{
	return uncompressFourBit(flashOnOffTicks_c_,1);
}
void CD4051::flash(unsigned char output, unsigned char flashes, unsigned char onTicks, unsigned char offTicks, bool startOn)
{
	output_ = output;
	flashCnt_ = flashes;
	flashTick_ = 0;
	setFlashOnTicks(onTicks);
	setFlashOffTicks(offTicks);
	inhibit_ = !startOn;
}
void CD4051::flashStop(bool inhibit)
{
	flashCnt_ = 0;
	inhibit_ = inhibit;
}
void CD4051::refreshFlash(unsigned char tick_inc)
{
	bool state;
	unsigned char onticks = 0;
	unsigned char offticks = 0;
	
	if(flashCnt_>0)
	{
		flashTick_ += tick_inc;
		state = !inhibit_;
		onticks = getFlashOnticks();
		offticks = getFlashOffTicks();
		if((state==true && flashTick_>=onticks) || (state==false && flashTick_>=offticks))
		{
			if(state==true)
			{
				flashTick_ -= onticks;
			}
			else
			{
				flashTick_ -= offticks;
			}
			inhibit_ = state;
			if(flashCnt_<255)
			{
				flashCnt_--;
			}
		}
	}
}