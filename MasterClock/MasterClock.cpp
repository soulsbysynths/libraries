//MasterClock.cpp  Master timing class
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

#include "MasterClock.h"

// default constructor
MasterClock::MasterClock()
{
} //MasterClock

// default destructor
MasterClock::~MasterClock()
{
} //~MasterClock

void MasterClock::refresh(unsigned char ticksPassed)
{	
	ticks_ += ticksPassed;
	if(ticks_>=ticksPerCycle_)
	{
		ticks_ -= ticksPerCycle_;
	}
	unsigned char bs = bsScaleConstrain(ticks_);
	output_ = (ticks_ << (16-bs)) / (ticksPerCycle_ >> bs);		
}
void MasterClock::setTicksPerCycle(unsigned long ticks)
{
	ticksPerCycle_ = ticks;
	unsigned char bs = bsMultConstrain(ticksPerCycle_);
	ticks_ = (((unsigned long)output_>>bs) * (ticksPerCycle_>>bs)) >> (16-(bs<<1));	
}
void MasterClock::reset()
{
	ticks_ = 0;
	output_ = 0;
}
unsigned char MasterClock::bsMultConstrain(unsigned long ip)
{
	//bs for multiplying with number max 65536. handles anything up to 4194304
	if(ip>1048575)
	{
		return 3;
	}
	else if (ip>262143)
	{
		return 2;
	}
	else if(ip>65535)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
unsigned char MasterClock::bsScaleConstrain(unsigned long ip)
{
	//for scaling number up.
	if(ip>1048575)
	{
		return 5;
	}
	else if (ip>524287)
	{
		return 4;
	}
	else if(ip>262143)
	{
		return 3;
	}
	else if(ip>131071)
	{
		return 2;
	}
	else if(ip>65535)
	{
		return 1;
	}
	else
	{
		return 4;
	}
}