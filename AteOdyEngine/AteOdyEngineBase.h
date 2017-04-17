/*
 * AteOdyEngineBase.h
 *
 * Created: 27/09/2016 14:47:01
 *  Author: paulsoulsby
 */ 


#ifndef ATEODYENGINEBASE_H_
#define ATEODYENGINEBASE_H_

class AteOdyEngineBase
{
	public:
	virtual void engineFunctionChanged(unsigned char func, unsigned char val) = 0;
	virtual void engineOptionChanged(unsigned char func, bool opt) = 0;
};



#endif /* ATEODYENGINEBASE_H_ */