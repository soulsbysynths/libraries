//Arpeggiator.cpp  Arpeggiator Class
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

#include "Arpeggiator.h"

// default constructor
Arpeggiator::Arpeggiator(ArpeggiatorBase* base, bool* noteOn)
{
	base_ = base;
	noteOn_ = noteOn;
} //Arpeggiator

// default destructor
Arpeggiator::~Arpeggiator()
{
	if(base_!=NULL)
	{
		delete base_;
	}
	if(noteOn_!=NULL)
	{
		delete noteOn_;
	}
} //~Arpeggiator

void Arpeggiator::setType(unsigned char newType)
{
	if(type_==0 && newType>0)
	{
		buildNoteOrder();
	}
	type_ = newType;

}

void Arpeggiator::setDivision(unsigned char newDiv)
{
	division_ = newDiv;
	divMult_ = pgm_read_word(&(arpMult[division_]));
	divBs_ = pgm_read_byte(&(arpBs[division_]));
}
void Arpeggiator::reset()
{
	pos_ = 0;
	indexLast_ = arpLength;
	noteLast_ = 0;
	note_ = findNote();
}
void Arpeggiator::refresh(unsigned int cycleTick)
{
	if(type_==0)
	{
		return;
	}
	unsigned char index = ((unsigned long)cycleTick * divMult_) >> divBs_;
	unsigned char newNote = 0;
	index &= arpBitMask;
	
	if(index!=indexLast_)
	{
		char steps_passed = index - indexLast_;
		if(steps_passed<0)
		{
			steps_passed += arpLength;
		}
		for(char i=0;i<steps_passed;++i)
		{
			note_ = findNote();
		}
		if (note_>0 || noteLast_>0)
		{                                                                 //assuming it now has found as note
			base_->arpeggiatorNoteEvent(noteLast_,note_);                                                       //fire it
			noteLast_ = note_;
		}
		indexLast_ = index;
	}
}
unsigned char Arpeggiator::findNote()
{
	unsigned char note = 0;
	unsigned char l = arpLength;
	if(pingPong_==true)
	{
		l *= 2;
	}
	for(unsigned char i=0;i<l;++i)
	{
		note =  getNoteOrder(pgm_read_byte(&(arpTypes[type_][pos_])));
		incrementPos();
		if(note>0)
		{
			break;
		}
	}
	return note;
}
unsigned char Arpeggiator::getNoteOrder(char shift)   //this finds out which note to fire, based on which notes are held down
{
	unsigned char index;
	if(shift>0)
	{
		index = shift - 1;
		return noteOrderUp_[index];
	}
	else
	{
		index = -(shift+1);
		return noteOrderDown_[index];
	}
}

void Arpeggiator::buildNoteOrder()
{
	unsigned char i;
	char ind;

	memset(noteOrderUp_,0,sizeof(noteOrderUp_));
	memset(noteOrderDown_,0,sizeof(noteOrderDown_));

	ind = 0;
	for(i=0;i<128;++i)
	{
		if(noteOn_[i]==true)
		{
			noteOrderUp_[ind] = i;
			ind++;
			if(ind>15)
			{
				break;
			}
		}
	}
	ind = 0;
	for(i=127;i!=0;--i)
	{
		if(noteOn_[i]==true)
		{
			noteOrderDown_[ind] = i;
			ind++;
			if(ind>15)
			{
				break;
			}
		}
	}
}
void Arpeggiator::incrementPos()
{
	if (pingPong_==false)
	{
		if (pos_==arpBitMask)
		{
			pos_ = 0;
		}
		else
		{
			pos_++;
		}
	}
	else
	{
		if (pongDown_==false)
		{
			if (pos_==arpBitMask)
			{
				pos_--;
				pongDown_ = true;
			}
			else
			{
				pos_++;
			}
		}
		else{
			if (pos_==0)
			{
				pos_++;
				pongDown_ = false;
			}
			else
			{
				pos_--;
			}
		}
	}
}