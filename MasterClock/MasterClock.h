//MasterClock.h  Master timing class
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

#ifndef __MASTERCLOCK_H__
#define __MASTERCLOCK_H__

class MasterClock
{
	//variables
	public:
	protected:
	private:
	unsigned long ticks_ = 0;
	unsigned long ticksPerCycle_ = 32000;
	unsigned int output_ = 0;
	//functions
	public:
	MasterClock();
	~MasterClock();
	void refresh(unsigned char ticksPassed);
	void reset();
	unsigned int getOutput(){return output_;}
	void setTicksPerCycle(unsigned long ticks);  // 1 cycle = 16 bars = 64 beats : the longest cycle time handled by atmegatron
	protected:
	private:
	MasterClock( const MasterClock &c );
	MasterClock& operator=( const MasterClock &c );
	unsigned char bsMultConstrain(unsigned long ip);
	unsigned char bsScaleConstrain(unsigned long ip);
}; //MasterClock

#endif //__MASTERCLOCK_H__
