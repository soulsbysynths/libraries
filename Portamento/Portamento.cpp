//Portamento.cpp  Portamento class
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


#include "Portamento.h"

// default constructor
Portamento::Portamento()
{
	refreshPortaTicks();
} //Portamento

// default destructor
Portamento::~Portamento()
{
} //~Portamento

void Portamento::setInput(unsigned int new_inp)
{
	if(new_inp!=input_ && new_inp!=output_)
	{
		input_ = new_inp;
		lastOutput_ = output_;
		tick_ = 0;
		refreshPortaTicks();		
	}
}
void Portamento::setProportional(bool new_prop)
{
	proportional_ = new_prop;
	refreshPortaTicks();
}

void Portamento::setSpeed(unsigned int new_speed)
{
	speed_ = new_speed;
	refreshPortaTicks();
}

void Portamento::refreshPortaTicks()
{
	unsigned long p;
	if (speed_==0)
	{
		portaTicks_ = 0;
	}
	else if (proportional_==true)
	{
		if (input_>lastOutput_)
		{
			p = (unsigned long)(input_-output_) * speed_ / output_;
		}
		else
		{
			p = (unsigned long)(output_-input_) * speed_ / input_;
		}
		portaTicks_ = (unsigned int)p;
	}
	else
	{
		portaTicks_ = speed_;
	}	
}
void Portamento::refresh(unsigned char ticksPassed)
{

	if (output_!=input_)
	{
		if (portaTicks_==0)
		{
			output_ = input_;
		}
		else
		{
			tick_ += ticksPassed;
			if (tick_>portaTicks_)
			{
				output_ = input_;
			}
			else
			{
				output_ = ((tick_ * ((long)input_ - (long)lastOutput_)) / portaTicks_) + lastOutput_;
			}
		}
	}
	else if (lastOutput_!=output_)
	{
		lastOutput_=output_;
	}

}