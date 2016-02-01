/*
 * Ade33LogicPatchBase.h
 *
 * Created: 05/01/2016 11:46:13
 *  Author: Paul Soulsby
 */ 


#ifndef ADE33LOGICPATCHBASE_H_
#define ADE33LOGICPATCHBASE_H_

class Ade33LogicPatchBase
{
	public:
	virtual void patchValueChanged(unsigned char func, unsigned char newValue) = 0;
};



#endif /* ADE33LOGICPATCHBASE_H_ */