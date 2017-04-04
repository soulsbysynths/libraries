//AtmEngineBase.h  Audio engine base class for Atmegatron
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

#ifndef MULATMEGAATMENGINEBASE_H_
#define MULATMEGAATMENGINEBASE_H_
#include <stdlib.h>

class MulAtmegaAtmEngineBase
{
	public:
	virtual void engineVoiceChanged(unsigned char voice) = 0;
	virtual void engineFunctionChanged(unsigned char func) = 0;
	virtual void engineValueChanged(unsigned char val) = 0;
	virtual void engineOptionChanged(bool opt) = 0;
	virtual void engineBankChanged(unsigned char bank) = 0;
	virtual void engineMidiTransmit(unsigned char data) = 0;
	virtual void engineSlaveTransmit(unsigned char id, unsigned char * data, size_t length) = 0;
	virtual void engineSysexComplete() = 0;
	virtual void engineVoiceTriggered(unsigned char voice, bool on) = 0;
};



#endif /* MULATMEGAATMENGINEBASE_H_ */