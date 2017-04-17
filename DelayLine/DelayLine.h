/* 
* DelayLine.h
*
* Created: 15/04/2017 23:29:52
* Author: paulsoulsby
*/


#ifndef __DELAYLINE_H__
#define __DELAYLINE_H__

#include <string.h>

class DelayLine
{
//variables
public:
protected:
private:
	unsigned char* line_ = NULL;
	unsigned char size_ = 0;
	unsigned char length_ = 0;
	unsigned char feedback_ = 0;
	unsigned char writePos_ = 0;
	unsigned char clip_ = 255;
//functions
public:
	DelayLine();
	DelayLine(unsigned char size);
	DelayLine(const DelayLine& dl);
	~DelayLine();
	DelayLine& operator = (const DelayLine& dl);
	void setLength(unsigned char newValue);
	unsigned char getLength(){return length_;}
	void setClip(unsigned char newValue){clip_ = newValue;}
	void setFeedback(unsigned char newValue){feedback_ = newValue;}
	void clearLine();
	unsigned char writeLine(unsigned char newVal);
protected:
private:
}; //DelayLine

#endif //__DELAYLINE_H__
