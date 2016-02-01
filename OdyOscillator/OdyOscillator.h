//OdyOscillator.h  Saw and square oscillator with pwm
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




#ifndef __ODYOSCILLATOR_H__
#define __ODYOSCILLATOR_H__

#include "OdyOscillatorProgmem.h"

class OdyOscillator
{
//variables
public:
	enum OscWave : unsigned char
	{
		SAW,
		PULSE
	};
	enum PwmSource : unsigned char
	{
		LFO,
		ENV
	};
protected:
private:
	OscWave waveform_ = SAW;
	unsigned char pulseWidth_ = 127;  //31;
	unsigned char pwmAmount_ = 0;
	PwmSource pwmSource_ = LFO;
	static const unsigned char PWM_MAX = 246;  // 61;
	static const unsigned char PWM_MIN = 9;
	unsigned char pulseIndex_ = 127;  //31;
	unsigned char level_ = 7;  //output level 0-7
//functions
public:
	OdyOscillator();
	~OdyOscillator();
	void setWaveform(OscWave newWave){waveform_ = newWave;}
	OscWave getWaveform(){return waveform_;}
	void setLevel(unsigned char newLevel){level_= newLevel;}
	unsigned char getLevel(){return level_;}
	void setPulseWidth(unsigned char newWidth){pulseWidth_ = newWidth;}
	unsigned char getPulseWidth(){return pulseWidth_;}
	void setPwmAmout(unsigned char newPwm){pwmAmount_ = newPwm;}
	unsigned char getPwmAmount(){return pwmAmount_;}
	void setPwmSource(PwmSource newPwmSource){pwmSource_ = newPwmSource;}
	PwmSource getPwmSource(){return pwmSource_;}
	unsigned char getPulseIndex(){return pulseIndex_;}
	char getOutput(unsigned char index);
	void refresh(char envOutput, char lfoOutput);
protected:
private:
	OdyOscillator( const OdyOscillator &c );
	OdyOscillator& operator=( const OdyOscillator &c );

}; //OdyOscillator

#endif //__ODYOSCILLATOR_H__
