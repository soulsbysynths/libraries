/* 
* DelayLine.cpp
*
* Created: 15/04/2017 23:29:52
* Author: paulsoulsby
*/


#include "DelayLine.h"

// default constructor
DelayLine::DelayLine() : line_(NULL)
{
} //DelayLine

DelayLine::DelayLine(unsigned char size)
{
	size_ = size;
	line_ = new unsigned char[size_];
}

DelayLine::DelayLine(const DelayLine& dl)
{
	size_ = dl.size_;
	line_ = new char[dl.size_];
	for(unsigned char i=0;i<size_;++i)
	{
		line_[i] = dl.line_[i];
	}
}
// default destructor
DelayLine::~DelayLine()
{
	if(line_ != NULL)
	{
		delete [] line_;
	}
} //~DelayLine

DelayLine& DelayLine::operator = (const DelayLine& dl)
{
	if (&dl == this)
	{
		return *this;
	}
	if(line_ != NULL)
	{
		delete [] line_;
	}
	size_ = dl.size_;
	line_ = new char[dl.size_];
	for(unsigned char i=0;i<size_;++i)
	{
		line_[i] = dl.line_[i];
	}
	return *this;
}
void DelayLine::setLength(unsigned char newValue)
{
	length_ = newValue;
	if (writePos_>length_)
	{
		writePos_ = length_;
	}
	for(unsigned char i=(length_+1);i<size_;++i)
	{
		line_[i] = 0;
	}
}
void DelayLine::clearLine()
{
	memset(line_,0,sizeof(*line_));
}

unsigned char DelayLine::writeLine(unsigned char newVal)
{
	unsigned char fb = ((unsigned int)line_[writePos_] * feedback_) >> 8;
	unsigned int output = newVal + fb;
	if(output>clip_)
	{
		output = clip_;
	}
	line_[writePos_] = (unsigned char)output;
	writePos_++;
	if(writePos_>length_)
	{
		writePos_ = 0;
	}
	return (unsigned char)output;
}