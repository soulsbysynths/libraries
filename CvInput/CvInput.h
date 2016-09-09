//CvInput.h  Hardware analogue control (ie potentiometer)
//Copyright (C) 2015  Paul Soulsby info@soulsbysynths.com
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

#ifndef __CVINPUT_H__
#define __CVINPUT_H__

#include <stdlib.h>

#define absdiff(a,b) ((a)>(b)?(a-b):(b-a))

class CvInput
{
//variables
public:
protected:
private:
	static const unsigned int kCtrlMoveThreshold = 16;
	static const unsigned int kCtrlMoveTimeOut = 2000;
	bool moving_ = false;
	unsigned int moveTick_ = 0;
	unsigned int value_ = 0;
	unsigned int valueLast_ = 0;
	bool lockOut_ = true;
	unsigned char gain_ = 0xFF;
	bool gainChanged_ = false;
//functions
public:
	CvInput() {}
	CvInput(unsigned int initValue);
	~CvInput();
	void setValue(unsigned int newValue){value_ = newValue;}
	unsigned int getValue(){return value_;}
	void setGain(unsigned char newValue);
	unsigned char getGain(){return gain_;}
	unsigned int getOutput();
	void setLockOut(bool newValue){lockOut_ = newValue;}
	bool getMoving(){return moving_;}
	bool hasChanged(unsigned char ticksPassed);
protected:
private:
	
	//CvInput( const CvInput &c );
	//CvInput& operator=( const CvInput &c );

}; //CvInput

#endif //__CVINPUT_H__
