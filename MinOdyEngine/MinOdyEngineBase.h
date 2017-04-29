/*
 * MinOdyEngineBase.h
 *
 * Created: 21/12/2016 13:16:46
 *  Author: Paul Soulsby
 */ 


#ifndef MINODYENGINEBASE_H_
#define MINODYENGINEBASE_H_

class MinOdyEngineBase
{
	public:
	virtual void engineValueChanged(unsigned char func, unsigned char val, bool opt) = 0;
	virtual void engineBankChanged(unsigned char bank) = 0;
	virtual void engineMidiTransmit(unsigned char data) = 0;
};



#endif /* MINODYENGINEBASE_H_ */