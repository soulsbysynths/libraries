/* 
* Ade33HwTester.cpp
*
* Created: 02/01/2016 13:32:01
* Author: Paul Soulsby
*/


#include "Ade33HwTester.h"

// default constructor
Ade33HwTester::Ade33HwTester()  : hardware_(Ade33Hardware::getInstance())
{
	hardware_.construct(this,Ade33Hardware::SW_LOGIC);
} //Ade33HwTester

// default destructor
Ade33HwTester::~Ade33HwTester()
{
} //~Ade33HwTester

void Ade33HwTester::poll(unsigned char ticksPassed)
{
	
	static unsigned char doSomething = 0;
	static unsigned char ledTest = 0;
	
	hardware_.pollAnalInputs();
	hardware_.pollDigiInputs(ticksPassed);
	
	hardware_.refreshFlash(ticksPassed);
	hardware_.refreshLeds();
	
	hardware_.setOutput(!hardware_.getOutput());
	
	doSomething += ticksPassed;
	if(doSomething>250 && auto_==true)
	{
		doSomething -= 250;
		ledTest++;
		hardware_.getDemultiplexer().setOutput(ledTest&0x07);
		hardware_.getLed(0).setState((bool)((ledTest>>1)&0x01));
		hardware_.getLed(1).setState((bool)(ledTest&0x01));
	}
	
}

void Ade33HwTester::setAuto(bool newValue)
{
	unsigned char i;
	if(auto_==true)
	{
		hardware_.getDemultiplexer().setInhibit(true);
		for(i=0;i<2;++i)
		{
			hardware_.getLed(i).setState(false);
		}
	}
	auto_ = newValue;
}

void Ade33HwTester::hardwareAnalogueInputChanged(unsigned char an, unsigned char newValue)
{
	setAuto(false);
	if((bool)newValue==false)
	{
		hardware_.getDemultiplexer().setInhibit(true);
	}
	else
	{
		hardware_.getDemultiplexer().setInhibit(false);
		hardware_.getDemultiplexer().setOutput(an);
	}
}

void Ade33HwTester::hardwareSwitchChanged(unsigned char sw, unsigned char newValue)
{
	setAuto(false);
	if(sw<Ade33Hardware::DIGI_AB_EXT)
	{
		hardware_.getLed(sw).setState((bool)newValue);
	}
	else
	{
		if((bool)newValue==false)
		{
			hardware_.getDemultiplexer().setInhibit(true);
		}
		else
		{
			hardware_.getDemultiplexer().setInhibit(false);
			hardware_.getDemultiplexer().setOutput(EXT_LED);
		}
	}
}
void Ade33HwTester::hardwareSwitchHeld(unsigned char sw)
{
	if(sw<Ade33Hardware::DIGI_AB_EXT)
	{
		hardware_.getLed(sw).flash(4,1,1,true);
	}
	else
	{
		hardware_.getDemultiplexer().flash(EXT_LED,4,1,1,true);
	}
}