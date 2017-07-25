/*
//Delayline.h - tape style delay for bytes. written for Oscidrum
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
	unsigned char readPos_ = 0;
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
