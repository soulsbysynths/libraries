//OdyAmplifier.h  Odytron Calculate current amplitude.
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


#ifndef __ODYAMPLIFIER_H__
#define __ODYAMPLIFIER_H__

#include "SsHelpers.h"

class OdyAmplifier
{
//variables
public:
	enum AmpAmSource : unsigned char
	{
		AR,
		ADSR
	};
protected:
private:
	unsigned char amp_ = 0;
	unsigned char level_ = 0;  //0-127 if you ever want to implement this
	unsigned char clip_ = 7;
	unsigned char amAmount_ = 16;  //0-15 if you ever want to implement this
	AmpAmSource amSource_ = AR;
//functions
public:
	OdyAmplifier();
	~OdyAmplifier();
	void refresh(char am);
	void setLevel(unsigned char newLevel){level_ = newLevel;}
	unsigned char getLevel(){return level_;}
	void setClip(unsigned char newCip);
	unsigned char getClip(){return clip_;}
	void setAmSource(AmpAmSource newSource){amSource_ = newSource;}
	AmpAmSource getAmSource(){return amSource_;}
	char getOutput(){return amp_;}
protected:
private:
	OdyAmplifier( const OdyAmplifier &c );
	OdyAmplifier& operator=( const OdyAmplifier &c );
}; //OdyAmplifier

#endif //__ODYAMPLIFIER_H__
