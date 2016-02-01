//OdyNoise.h  Odytron white noise simulation. pink filter not used.  creates white-noise-esque noise super quick.
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


#ifndef __ODYNOISE_H__
#define __ODYNOISE_H__

#include "OdyNoiseProgmem.h"
#include "SsHelpers.h"

class OdyNoise
{
//variables
public:
protected:
private:
	unsigned char level_ = 0;  //0-7 for ody
	bool pink_ = false;
	unsigned char writeIndex_ = 0;
//functions
public:
	OdyNoise();
	~OdyNoise();
	void setLevel(unsigned char newLevel){level_ = newLevel;}
	unsigned char getLevel(){return level_;}
	void setPink(bool newPink){pink_ = newPink;}
	bool getPink(){return pink_;}
	void refresh();
	static char getOutput();
protected:
private:
	OdyNoise( const OdyNoise &c );
	OdyNoise& operator=( const OdyNoise &c );
	char filterPink(char in);
}; //OdyNoise

#endif //__ODYNOISE_H__
