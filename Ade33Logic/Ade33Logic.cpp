/*
* Ade33Logic.cpp
*
* Created: 05/01/2016 17:19:15
* Author: Paul Soulsby
*/


#include "Ade33Logic.h"

// default constructor
Ade33Logic::Ade33Logic() : engine_(Ade33LogicEngine::getInstance()), hardware_(Ade33Hardware::getInstance())
{
	engine_.construct(this);
	hardware_.construct(this,Ade33Hardware::SW_LOGIC);
} //Ade33Logic

// default destructor
Ade33Logic::~Ade33Logic()
{
} //~Ade33Logic

void Ade33Logic::initialize()
{
	unsigned char i;
	bool found = false;
	for(i=0;i<64;++i)
	{
		if(engine_.getPatchPtr()->getPatchSlotUsed(i)==true)
		{
			patchNum = i;
			found = true;
			engine_.getPatchPtr()->readPatch(patchNum);
			break;
		}
	}
	if(found==false)
	{
		engine_.getPatchPtr()->setFunctionValue(Ade33LogicEngine::FUNC_MODE,Ade33LogicEngine::MODE_AND);
		engine_.getPatchPtr()->setFunctionValue(Ade33LogicEngine::FUNC_A_SEL,0);
	}
}

void Ade33Logic::poll(unsigned char ticksPassed)
{
	hardware_.pollAnalInputs(ticksPassed);
	hardware_.pollDigiInputs(ticksPassed);
	hardware_.refreshFlash(ticksPassed);
	hardware_.refreshLeds();
	if(autoscrollMode==true)
	{
		autoscrollModeTick += ticksPassed;
		if(autoscrollModeTick>AUTOSCROLL_TICKS)
		{
			engine_.incMode();
			autoscrollModeTick -= AUTOSCROLL_TICKS;
		}
	}
}


//**************engine events************************
void Ade33Logic::engineFunctionChanged(unsigned char func, unsigned char val)
{
	switch (func)
	{
		case (unsigned char)Ade33LogicEngine::FUNC_MODE:
		hardware_.getDemultiplexer().setOutput(val);
		break;
		case (unsigned char)Ade33LogicEngine::FUNC_A_SEL:
		hardware_.getLed(val).setState(true);
		hardware_.getLed(1-val).setState(false);
		break;
	}
}
void Ade33Logic::engineOutputChanged(unsigned char val)
{
	hardware_.setOutput((bool)val);
}
//****************hardware events************************
void Ade33Logic::hardwareAnalogueInputChanged(unsigned char an, unsigned char newValue)
{
	static unsigned char lastMode = 0;
	switch (an)
	{
		case Ade33Hardware::ANAL_A1:
		engine_.setInputA(0,newValue);
		break;
		case Ade33Hardware::ANAL_A2:
		engine_.setInputA(1,newValue);
		break;
		case Ade33Hardware::ANAL_B:
		engine_.setInputB(newValue);
		break;
		case Ade33Hardware::ANAL_MODE:
		newValue /= 43;
		if(newValue!=lastMode)
		{
			lastMode = newValue;
			engine_.getPatchPtr()->setFunctionValue(Ade33LogicEngine::FUNC_MODE,newValue);
		}
		break;
	}
}
void Ade33Logic::hardwareAnalogueClockEdge(unsigned char an, bool rising)
{
	if(an==Ade33Hardware::ANAL_MODE && rising==true)
	{
		engine_.incMode();
	}
}
void Ade33Logic::hardwareSwitchChanged(unsigned char sw, unsigned char newValue)
{

	if(newValue==HIGH)
	{
		switch (sw)
		{
			case Ade33Hardware::DIGI_MODE:
			engine_.incMode();
			break;
			case Ade33Hardware::DIGI_A_SW:
			case Ade33Hardware::DIGI_A_EXT:
			engine_.getPatchPtr()->setFunctionValue(Ade33LogicEngine::FUNC_A_SEL,1-engine_.getPatchPtr()->getFunctionValue(Ade33LogicEngine::FUNC_A_SEL));
			engine_.refreshOutput();
			break;
		}
	}
	else
	{
		if(sw<Ade33Hardware::DIGI_A_EXT)  //do on release to catch autoscroll too.
		{
			engine_.getPatchPtr()->setPatchSlotUsed(patchNum,false);
			patchNum++;
			if(patchNum>63)
			{
				patchNum = 0;
			}
			engine_.getPatchPtr()->writePatch(patchNum);
		}
		autoscrollMode = false;
	}
}
void Ade33Logic::hardwareSwitchHeld(unsigned char sw)
{
	if(sw==Ade33Hardware::DIGI_MODE)
	{
		autoscrollMode = true;
		autoscrollModeTick = 0;
		engine_.incMode();		
	}
}