//Lfo.h  Low frequency oscillator class
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

#ifndef __LFO_H__
#define __LFO_H__

#include "LfoProgmem.h"

class Lfo
{
//variables
public:
protected:
private:
	unsigned char division_ = 2;
	unsigned int divMult_ = 1;
	unsigned char divBs_ = 4;
	unsigned char table_ = 0;
	char output_ = 0;
	bool invert_ = false;
	unsigned char index_ = 0;
//functions
public:
	Lfo();
	~Lfo();
	void setDivision(unsigned char newDiv);
	unsigned char getDivision(){return division_;}
	void setTable(unsigned char newTable){table_ = newTable;}
	unsigned char getTable(){return table_;}
	void setInvert(bool new_inv){invert_ = new_inv;}
	bool getInvert(){return invert_;}
	char getOutput(){return output_;}
	unsigned char getIndex(){return index_;}
	void refresh(unsigned int cycleTick);
protected:
private:
	Lfo( const Lfo &c );
	Lfo& operator=( const Lfo &c );

}; //Lfo

#endif //__LFO_H__
