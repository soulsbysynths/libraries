/*
 * AteDrumPatchBase.h
 *
 * Created: 05/04/2017 16:26:06
 *  Author: paulsoulsby
 */ 


#ifndef ATEDRUMPATCHBASE_H_
#define ATEDRUMPATCHBASE_H_

class AteDrumPatchBase
{
	public:
	virtual void patchPatternChanged(unsigned char instr, unsigned char step, bool newValue) = 0;
	virtual void patchCtrlChanged(unsigned char instr, unsigned char ctrl, unsigned char newValue) = 0;
};



#endif /* ATEDRUMPATCHBASE_H_ */