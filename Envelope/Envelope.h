//Envelope.h  ADSR Envelope class
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

#ifndef __ENVELOPE_H__
#define __ENVELOPE_H__

#include <stdlib.h>
#include "SsHelpers.h"

class Envelope
{
//variables
public:
	enum EnvState : unsigned char
	{
		IDLE,
		ATTACKING,
		DECAYING,
		SUSTAINING,
		RELEASING
	};
protected:
private:
	static const int MAX_VALUE = 32767; 
	unsigned int attack_ = 32767;               
	unsigned int decay_ = 32767;           
	int sustain_ = 32767;              
	unsigned int release_ = 372;            
	int output_ = 0;          
	bool invert_ = false; 
	bool retrigger_ = false; 
	EnvState state_ = IDLE;         
	bool update_ = false;   
//functions
public:
	Envelope();
	~Envelope();
	void setAttack(unsigned int inc);
	unsigned int getAttack(){return attack_;}
	void setDecay(unsigned int inc);
	unsigned int getDecay(){return decay_;}
	void setSustain(int level);
	int getSustain(){return sustain_;}
	void setRelease(unsigned int inc);
	unsigned int getRelease(){return release_;}
	void setInvert(bool inverted);
	bool getInvert(){return invert_;}
	void setRetrigger(bool retrigger);
	bool getRetrigger(){return retrigger_;}
	char getOutput();
	void setState(EnvState newState){state_ = newState;}
	EnvState getState(){return state_;}
	char getExpOutput();
	void trigger();
	void release();
	void refresh(unsigned char ticksPassed);
protected:
private:
	Envelope( const Envelope &c );
	Envelope& operator=( const Envelope &c );
}; //Envelope

#endif //__ENVELOPE_H__
