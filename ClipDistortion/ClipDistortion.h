//ClipDistortion.h  Distortion class (labelled as Distortion on synth)
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

#ifndef __CLIPDISTORTION_H__
#define __CLIPDISTORTION_H__

#include "Wavetable.h"

class ClipDistortion
{
//variables
public:
protected:
private:
	unsigned char distLevel_ = 0;
//functions
public:
	ClipDistortion();
	~ClipDistortion();
	void setDistLevel(unsigned char newValue){distLevel_ = newValue;}
	unsigned char getDistLevel(){return distLevel_;}
	void processWavetable(Wavetable& sourceWavetable);
protected:
private:
	ClipDistortion( const ClipDistortion &c );
	ClipDistortion& operator=( const ClipDistortion &c );

}; //ClipDistortion

#endif //__CLIPDISTORTION_H__
