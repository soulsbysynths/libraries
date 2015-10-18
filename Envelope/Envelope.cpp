//Envelope.cpp  ADSR Envelope class 
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

#include "Envelope.h"

// default constructor
Envelope::Envelope()
{
} //Envelope

// default destructor
Envelope::~Envelope()
{
} //~Envelope

void Envelope::setAttack(unsigned int inc)
{
	attack_ = inc;
	update_ = true;
}

void Envelope::setDecay(unsigned int inc)
{
	decay_ = inc;
	update_ = true;
}

void Envelope::setSustain(int level)
{
	sustain_ = level;
	update_ = true;
}

void Envelope::setRelease(unsigned int inc)
{
	release_ = inc;
	update_ = true;
}

void Envelope::setInvert(bool inverted)
{
	invert_ = inverted;
	update_ = true;
}

void Envelope::setRetrigger(bool retrigger)
{
	retrigger_ = retrigger;
	update_ = true;
}

char Envelope::getOutput()
{
	char out;
	out = output_ >> 8;
	if(invert_==true)
	{
		return -out;
	}
	else
	{
		return out;
	}
}
char Envelope::getExpOutput()
{
	char out = convertExponential(abs(output_>>8));
	if(invert_==true)
	{
		return -out;
	}
	else
	{
		return out;
	}
}

void Envelope::trigger()
{
	state_ = ATTACKING;
	update_ = true;
	if(retrigger_==true)
	{
		output_ = 0;
	}
}

void Envelope::release()
{
	state_ = RELEASING;
	update_ = true;
}

void Envelope::refresh(unsigned char ticksPassed)
{
	unsigned long inc;
	if (update_==true){
		switch (state_){
			case IDLE:
			output_ = 0;
			update_ = false;
			break;
			case ATTACKING:
			inc = (unsigned long)attack_ * ticksPassed;
			if(inc >= (unsigned long)(MAX_VALUE - output_))
			{
				state_ = DECAYING;
				output_ = MAX_VALUE;
			}
			else
			{
				output_ += inc;
			}
			break;
			case DECAYING:
			inc = (unsigned long)decay_ * ticksPassed;
			if(inc >= (unsigned long)(output_ - sustain_))
			{
				state_ = SUSTAINING;
				output_ = sustain_;
				update_ = false;
			}
			else
			{
				output_ -= inc;
			}
			break;
			case SUSTAINING:
			output_ = sustain_;
			update_ = false;
			break;
			case RELEASING:
			inc = (unsigned long)release_ * ticksPassed;
			if(inc >= (unsigned long)output_)
			{
				state_ = IDLE;
				output_ = 0;
				update_ = false;
			}
			else
			{
				output_ -= inc;
			}
			break;
		}
	}
}
