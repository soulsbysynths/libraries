/*
* Ade33LogicEngine.cpp
*
* Created: 05/01/2016 11:41:50
* Author: Paul Soulsby
*/


#include "Ade33LogicEngine.h"

// default constructor
Ade33LogicEngine::Ade33LogicEngine(Ade33LogicEngineBase* base)
{
	construct(base);
} //Ade33LogicEngine

// default destructor
Ade33LogicEngine::~Ade33LogicEngine()
{
	if(patch_!=NULL)
	{
		delete patch_;
	}
} //~Ade33LogicEngine

void Ade33LogicEngine::construct(Ade33LogicEngineBase* base)
{
	base_ = base;
	patch_ = new Ade33LogicPatch(this);
}

void Ade33LogicEngine::setInputA(unsigned char index, unsigned char newValue)
{
	inputA_[index] = newValue;
	refreshOutput();
}
void Ade33LogicEngine::setInputB(unsigned char newValue)
{
	inputB_ = newValue;
	refreshOutput();
}
void Ade33LogicEngine::incMode()
{
	unsigned char f;
	f = patch_->getFunctionValue(FUNC_MODE) + 1;
	if(f>5)
	{
		f = 0;
	}
	patch_->setFunctionValue(FUNC_MODE,f);
	refreshOutput();
}
void Ade33LogicEngine::refreshOutput()
{
	unsigned char op;
	switch (patch_->getFunctionValue(FUNC_MODE))
	{
		case MODE_AND:
		op = inputA_[patch_->getFunctionValue(FUNC_A_SEL)] & inputB_;
		break;
		case MODE_NAND:
		op = ~(inputA_[patch_->getFunctionValue(FUNC_A_SEL)] & inputB_);
		break;
		case MODE_OR:
		op = inputA_[patch_->getFunctionValue(FUNC_A_SEL)] | inputB_;
		break;
		case MODE_NOR:
		op = ~(inputA_[patch_->getFunctionValue(FUNC_A_SEL)] | inputB_);
		break;
		case MODE_XOR:
		op = inputA_[patch_->getFunctionValue(FUNC_A_SEL)] ^ inputB_;
		break;
		case MODE_XNOR:
		op = ~(inputA_[patch_->getFunctionValue(FUNC_A_SEL)] ^ inputB_);
		break;
	}
	op &= 0x01;
	if(op!=output_)
	{
		output_ = op;
		base_->engineOutputChanged(op);
	}
}
//*********************patch events*********************
void Ade33LogicEngine::patchValueChanged(unsigned char func, unsigned char newValue)
{
	if(func==FUNC_MODE && newValue>MODE_XNOR)
	{
		patch_->setFunctionValue(FUNC_MODE,MODE_XNOR);
	}
	else if(func==FUNC_A_SEL && newValue>1)
	{
		patch_->setFunctionValue(FUNC_A_SEL,1);
	}
	else
	{
		base_->engineFunctionChanged(func,newValue);
	}
	
}