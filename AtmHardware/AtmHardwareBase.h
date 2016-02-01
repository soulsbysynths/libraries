//AtmHardwareBase.h  Atmegatron Hardware control base class.
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


#ifndef ATMHARDWAREBASEH_
#define ATMHARDWAREBASEH_

class AtmHardwareBase
{
	public:
	virtual void hardwareSwitchChanged(unsigned char sw, unsigned char newValue) = 0;
	virtual void hardwareSwitchHeld(unsigned char sw) = 0;
	virtual void hardwareAnalogueControlChanged(unsigned char control, unsigned char newValue) = 0;
	virtual void hardwareRotaryEncoderChanged(unsigned char rotary, unsigned char newValue, bool clockwise) = 0;
	virtual void hardwareMidiChannelChanged(unsigned char channel) = 0;
	virtual void hardwareMidiReceived(unsigned char data) = 0;
	virtual void hardwareMidiError(unsigned char errorType) = 0;
};

#endif /* ATMHARDWAREBASEH_ */