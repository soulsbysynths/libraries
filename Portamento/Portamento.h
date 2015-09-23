//Portamento.h  Portamento class
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


#ifndef __PORTAMENTO_H__
#define __PORTAMENTO_H__

class Portamento
{
//variables
public:
protected:
private:
	unsigned int input_ = 440;
	unsigned int lastOutput_ = 440;
	unsigned int output_ = 440;
	bool proportional_ = false;
	unsigned int speed_ = 0;
	unsigned int portaTicks_ = 0;
	unsigned int tick_ = 0; 
//functions
public:
	Portamento();
	~Portamento();
	void refresh(unsigned char ticksPassed);
	void setInput(unsigned int new_inp);
	void setProportional(bool new_prop);
	bool getProportional(){return proportional_;}
	void setSpeed(unsigned int new_speed);
	unsigned int getSpeed(){return speed_;}
	unsigned int getOutput(){return output_;}
protected:
private:
	Portamento( const Portamento &c );
	Portamento& operator=( const Portamento &c );
	void refreshPortaTicks();
}; //Portamento

#endif //__PORTAMENTO_H__
