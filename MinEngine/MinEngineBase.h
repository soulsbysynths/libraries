//MinEngineBase.h  Audio engine base class for miniAtmegatron
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


#ifndef MINENGINEBASE_H_
#define MINENGINEBASE_H_

class MinEngineBase
{
	public:
	virtual void engineValueChanged(unsigned char func, unsigned char val, bool opt) = 0;
	virtual void engineBankChanged(unsigned char bank) = 0;
	virtual void engineMidiTransmit(unsigned char data) = 0;
};



#endif /* MINENGINEBASE_H_ */