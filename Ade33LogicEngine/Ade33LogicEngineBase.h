/*
 * Ade33LogicEngineBase.h
 *
 * Created: 05/01/2016 16:33:57
 *  Author: Paul Soulsby
 */ 


#ifndef ADE33LOGICENGINEBASE_H_
#define ADE33LOGICENGINEBASE_H_

class Ade33LogicEngineBase
{
	public:
	virtual void engineFunctionChanged(unsigned char func, unsigned char val) = 0;
	virtual void engineOutputChanged(unsigned char val) = 0;
};




#endif /* ADE33LOGICENGINEBASE_H_ */