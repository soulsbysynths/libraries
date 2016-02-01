/*
 * OdyEngineBase.h
 *
 * Created: 23/09/2015 13:16:46
 *  Author: Paul Soulsby
 */ 


#ifndef ODYENGINEBASE_H_
#define ODYENGINEBASE_H_

class OdyEngineBase
{
	public:
	virtual void engineFunctionChanged(unsigned char func, unsigned char val, bool opt) = 0;
};



#endif /* ODYENGINEBASE_H_ */