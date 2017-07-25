//ArpHardwareBase.h  AT-ARP hardware class
//Copyright (C) 2017  Paul Soulsby info@soulsbysynths.com
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


#ifndef ARPHARDWAREBASE_H_
#define ARPHARDWAREBASE_H_

class ArpHardwareBase
{
	public:
	virtual void hardwareSwitchChanged(unsigned char sw, unsigned char newValue) = 0;
	virtual void hardwareSwitchHeld(unsigned char sw) = 0;
	virtual void hardwareJackDetectChanged(unsigned char jack, bool newValue) = 0; 
	virtual void hardwareCvInputChanged(unsigned char input, unsigned int newValue) = 0;
	virtual void hardwareGateInputChanged(bool newValue) = 0;
	virtual void hardwareMidiChannelChanged(unsigned char channel) = 0;
	virtual void hardwareMidiReceived(unsigned char data) = 0;
	virtual void hardwareMidiError(unsigned char errorType) = 0;
};



#endif /* ARPHARDWAREBASE_H_ */