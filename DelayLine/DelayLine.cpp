/*
//Delayline.cpp - tape style delay for bytes. written for Oscidrum
//Copyright (C) 2017  Paul Soulsby info@soulsbysynths.com
//
//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
	writePos_ = readPos_ + length_;
	if(writePos_>=size_)
	{
		writePos_ -= size_;
	}
//old method
	//length_ = newValue;
	//if (writePos_>length_)
	//{
		//writePos_ = length_;
	//}
	//for(unsigned char i=(length_+1);i<size_;++i)
	//{
		//line_[i] = 0;
	//}
}
void DelayLine::clearLine()
{
	memset(line_,0,sizeof(*line_));
}

unsigned char DelayLine::writeLine(unsigned char newVal)
{

	unsigned int readVal;
	if (length_==0)
	{
		readVal = newVal;
	}
	else
	{
		readVal = (unsigned int)line_[readPos_] + newVal;  //read = delayed + dry
		if(readVal>clip_)
		{
			readVal = clip_;
		}
	}

	unsigned int writeVal = (readVal * feedback_) >> 8;   //write = read * fb
	line_[writePos_] = (unsigned char)writeVal;
	
	readPos_++;
	if(readPos_>=size_)
	{
		readPos_ -= size_;
	}
	
	writePos_ = readPos_ + length_;
	if(writePos_>=size_)
	{
		writePos_ -= size_;
	}

	return (unsigned char)readVal;

//old method
	//unsigned char fb = ((unsigned int)line_[writePos_] * feedback_) >> 8;
	//unsigned int output = newVal + fb;
	//if(output>clip_)
	//{
		//output = clip_;
	//}
	//line_[writePos_] = (unsigned char)output;
	//writePos_++;
	//if(writePos_>length_)
	//{
		//writePos_ = 0;
	//}
	//return (unsigned char)output;
}