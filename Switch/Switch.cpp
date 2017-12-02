//Switch.cpp  Hardware switch/button with debounce
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

#include "Switch.h"

// default constructor
Switch::Switch(unsigned char initValue, unsigned int holdTicks)
{
	value_ = initValue;
	valueActive_ = initValue;
	holdEventTicks_ = holdTicks;
} //Switch

Switch::Switch(unsigned char index, unsigned char initValue, unsigned int holdTicks, SwitchBase* base)
{
	base_ = base;
	index_ = index;
	value_ = initValue;
	valueActive_ = initValue;
	holdEventTicks_ = holdTicks;
} //Switch

// default destructor
Switch::~Switch()
{
	if (base_ != NULL)
	{
		delete base_;
	}
} //~Switch

bool Switch::hasHeld(unsigned char ticksPassed)
{
	bool held = false;
	if(value_==S_HIGH)
	{
		holdTime_ += ticksPassed;
		if(holdTime_>=holdEventTicks_ && holdEventFired_==false)
		{
			held = true;
			holdEventFired_ = true;
		}
		
	}
	return held;
}
bool Switch::hasChanged(unsigned char ticksPassed)
{
	bool changed = false;

	if(ticks_<debounceTicks_)
	{
		ticks_ += ticksPassed;
	}
	if(ticks_>=debounceTicks_)
	{
		if(value_!=valueActive_)
		{
			if(value_==S_HIGH && valueActive_==S_LOW)
			{
				holdTime_ = 0;
				holdEventFired_ = false;
				changed = true;
			}
			else if(value_==S_LOW && valueActive_==S_HIGH)
			{
				changed = true;
			}
			valueActive_ = value_;
			ticks_ = 0;
		}
	}
	return changed;
}

void Switch::poll(unsigned char ticksPassed)
{
	if (ticks_ < debounceTicks_)
	{
		ticks_ += ticksPassed;
		if (value_ != valueActive_)
		{
			//listBox1.Items.Insert(0, totalTicks_.ToString() + ":DEBOUNCE");
		}
	}
	else
	{
		if (value_ != valueActive_)
		{
			if (value_ == true && valueActive_ == false)
			{
				if (click_ == 0)
				{
					click_ = 1;
					holdTime_ = 0;
					holdEventFired_ = false;
					dclickTime_ = 0;
				}
				else if (dclickTime_ < dclickEventTicks_)
				{
					click_ = 0;
					//listBox1.Items.Insert(0, totalTicks_.ToString() + ":DOUBLE");
					base_->swDoubleClicked(index_);
				}
				//listBox1.Items.Insert(0, totalTicks_.ToString() + ":DOWN");
				base_->swValueChanged(index_,value_);
			}
			else if (value_ == false && valueActive_ == true)
			{
				//listBox1.Items.Insert(0, totalTicks_.ToString() + ":UP");
				base_->swValueChanged(index_,value_);
			}
			valueActive_ = value_;
			ticks_ = 0;
		}
	}
	if (valueActive_ == true)
	{
		if(holdTime_ < holdEventTicks_)
		{
			holdTime_ += ticksPassed;
		}
		else if(holdEventFired_==false)
		{
			//listBox1.Items.Insert(0, totalTicks_.ToString() + ":HELD");
			base_->swHeld(index_);
			holdEventFired_ = true;
		}
	}
	if (dclickTime_ < dclickEventTicks_)
	{
		dclickTime_ += ticksPassed;
	}
	else if (click_ == 1 && valueActive_ == false)
	{
		if (holdEventFired_ == false)
		{
			//listBox1.Items.Insert(0,totalTicks_.ToString() + ":CLICK");
			base_->swClicked(index_);
		}
		click_ = 0;
	}
	
}

//void Switch::poll(unsigned char ticksPassed)
//{
	//if(ticks_<debounceTicks_)
	//{
		//ticks_ += ticksPassed;
	//}
	//else
	//{
		//if(value_!=valueActive_)
		//{
			//if(value_==S_HIGH && valueActive_==S_LOW)
			//{
				//holdTime_ = 0;
				//holdEventFired_ = false;
				//if(click_==0)
				//{
					//click_ = 1;
				//}
				//else if(dclickTime_<dclickEventTicks_)
				//{
					//click_ = 2;
				//}
				//base_->swValueChanged(index_,value_);
			//}
			//else if(value_==S_LOW && valueActive_==S_HIGH)
			//{
				//if(click_==1)
				//{
					//dclickTime_ = 0;
				//}
				//else if(click_==2)
				//{
					//if(holdEventFired_==false)
					//{
						//base_->swDoubleClicked(index_);
					//}
					//click_ = 0;
				//}
				//base_->swValueChanged(index_,value_);
			//}
			//valueActive_ = value_;
			//ticks_ = 0;
		//}
	//}
	//if(valueActive_==S_HIGH)
	//{
		//holdTime_ += ticksPassed;
		//if(holdTime_>=holdEventTicks_ && holdEventFired_==false)
		//{
			//base_->swHeld(index_);
			//holdEventFired_ = true;
		//}
	//}
	//if(dclickTime_<dclickEventTicks_)
	//{
		//dclickTime_ += ticksPassed;
	//}
	//else if (click_==1 && valueActive_==S_LOW)
	//{
		//if(holdEventFired_==false)
		//{
			//base_->swClicked(index_);
		//}
		//click_ = 0;
	//}
//}