/* 
* JamHardwareTester.cpp
*
* Created: 18/06/2016 22:21:51
* Author: paulsoulsby
*/


#include "JamHardwareTester.h"

// default constructor
JamHardwareTester::JamHardwareTester() : hardware_(JamHardware::getInstance())
{
	hardware_.construct(this);
} //JamHardwareTester

// default destructor
JamHardwareTester::~JamHardwareTester()
{
} //~JamHardwareTester

void JamHardwareTester::initialize()
{
	unsigned char i;
	hardware_.initializeAudio();
	hardware_.getLed().setColour(LedRgb::RED);
	hardware_.refreshLEDs();

}
void JamHardwareTester::poll(unsigned char ticksPassed)
{
	//static bool toggle = false;
	hardware_.pollAnlControls(ticksPassed);
	hardware_.pollSwitches(ticksPassed);
	hardware_.refreshFlash(ticksPassed);
	hardware_.refreshLEDs();
	//if(ticksPassed>0)
	//{
		//toggle = !toggle;
		//if(toggle==true)
		//{
			//bitSet(PORTB,PINB1);
		//}
		//else
		//{
			//bitClear(PORTB,PINB1);
		//}
	//}
}

void JamHardwareTester::pollAudio()
{
	static char audio = 0;
	
	while(!hardware_.isAudioBufferFull())
	{
		audio++;	
		hardware_.writeAudioBuffer(audio);
	}
}

//**************************hardware events************************************
void JamHardwareTester::hardwareAnalogueControlChanged(unsigned char control, unsigned char newValue)
{
	hardware_.getLed().setColour((LedRgb::LedRgbColour)(newValue>>5));
}

void JamHardwareTester::hardwareSwitchChanged(unsigned char sw, unsigned char newValue)
{
	if(newValue==HIGH)
	{
		hardware_.getLed().setColour((LedRgb::LedRgbColour)(sw + 1));
	}
	else
	{
		hardware_.getLed().setColour(LedRgb::OFF);
	}
}
void JamHardwareTester::hardwareSwitchHeld(unsigned char sw)
{
}
