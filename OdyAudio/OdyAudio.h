//OdyAudio.h  Odytron output PWM audio
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


#ifndef __ODYAUDIO_H__
#define __ODYAUDIO_H__

#include <avr/io.h>
#include <avr/interrupt.h>

#ifndef bitRead
#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#endif
#ifndef bitSet
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#endif
#ifndef bitClear
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#endif
#ifndef bitWrite
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))
#endif

class OdyAudio
{
//variables
public:
protected:
private:
	unsigned long sampleFreq_[2] = {112640,112640};
//functions
public:
	OdyAudio();
	~OdyAudio();
	void setSampleFreq(unsigned char oscNum, unsigned long newSf);
	unsigned long getSampleFreq(unsigned char oscNum){return sampleFreq_[oscNum];}
	static unsigned char getWtIndex(unsigned char oscNum);
	static void setWaveSync(bool newSync);
	static bool getWaveSync();
	void initialize();
protected:
private:
	OdyAudio( const OdyAudio &c );
	OdyAudio& operator=( const OdyAudio &c );

}; //OdyAudio

#endif //__ODYAUDIO_H__
