//ArpAtarpEngineBase.h  AT-ARP data processing engine
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

#ifndef ARPATARPENGINEBASE_H_
#define ARPATARPENGINEBASE_H_

class ArpAtarpEngineBase
{
	public:
	virtual void engineOutputChanged(unsigned int newOutput) = 0;
	virtual void engineFunctionChanged(unsigned char newFunction) = 0;
	virtual void engineValueChanged(unsigned char newValue) = 0;
	virtual void engineLoadUserPattern(unsigned char patternNum) = 0;
	virtual void engineSaveUserPatternStep(unsigned char patternNum, unsigned char step, char value) = 0;
};



#endif /* ARPATARPENGINEBASE_H_ */