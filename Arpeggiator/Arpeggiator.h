//Arpeggiator.h  Arpeggiator Class
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

#ifndef __ARPEGGIATOR_H__
#define __ARPEGGIATOR_H__

#include <stdlib.h>
#include <string.h>
#include "ArpeggiatorProgmem.h"
#include "ArpeggiatorBase.h"

class Arpeggiator
{
//variables
public:
protected:
private:
	ArpeggiatorBase* base_;
	unsigned char type_ = 0;
	unsigned char division_ = 2;
	unsigned int divMult_ = 1;
	unsigned char divBs_ = 4;
	bool pingPong_ = false;
	bool pongDown_ = false;
	char pos_ = 0;
	bool* noteOn_ = NULL;
	unsigned char indexLast_ = ARP_LENGTH;
	unsigned char noteLast_ = 0;
	unsigned char note_ = 0;
	unsigned char noteOrderUp_[16] = {0};
	unsigned char noteOrderDown_[16] = {0};
//functions
public:
	Arpeggiator(ArpeggiatorBase* base, bool* noteOn);
	~Arpeggiator();
	void setPingPong(bool way){pingPong_ = way;}
	bool getPingPong(){return pingPong_;}
	void setType(unsigned char newType);
	unsigned char getType(){return type_;}
	void setDivision(unsigned char newDiv);
	unsigned char getDivision(){return division_;}
	void refresh(unsigned int cycleTick);
	void buildNoteOrder();
	void reset();
protected:
private:
	Arpeggiator() {}
	Arpeggiator( const Arpeggiator &c );
	Arpeggiator& operator=( const Arpeggiator &c );
	void incrementPos();
	unsigned char getNoteOrder(char shift);
	unsigned char findNote();
}; //Arpeggiator

#endif //__ARPEGGIATOR_H__
