/*
 * AteDrumEngineBase.h
 *
 * Created: 05/04/2017 15:28:47
 *  Author: paulsoulsby
 */ 


#ifndef ATEDRUMENGINEBASE_H_
#define ATEDRUMENGINEBASE_H_

class AteDrumEngineBase
{
	public:
	virtual void engineInstrChanged(unsigned char instr) = 0;
	virtual void engineStepChanged(unsigned char step) = 0;
	virtual void enginePatternChanged(unsigned char step, bool newValue) = 0;
	virtual void enginePatchChanged(unsigned char patchNum) = 0;
	//virtual void engineStartCapture(bool way) = 0;
};



#endif /* ATEDRUMENGINEBASE_H_ */