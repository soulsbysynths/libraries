//OdyNoise.cpp  Odytron white noise simulation. pink filter not used.  creates white-noise-esque noise super quick.
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


#include "OdyNoise.h"
static volatile char wavetable[256];
static volatile unsigned char readIndex = 0;
static volatile unsigned char readIndexInc = 0;
// default constructor
OdyNoise::OdyNoise()
{
} //OdyNoise

// default destructor
OdyNoise::~OdyNoise()
{
} //~OdyNoise

char OdyNoise::getOutput()
{
	readIndex += pgm_read_byte(&(PRIME_NUMBERS[readIndexInc]));
	return wavetable[readIndex];
}

void OdyNoise::refresh()
{
	
	char max = pgm_read_byte(&(NOISE_MAX[level_]));
	char min = pgm_read_byte(&(NOISE_MIN[level_]));
	char noise = randomValue(min,max);
	if(pink_==true){
		noise = filterPink(noise);
	}
	wavetable[writeIndex_] = noise;
	writeIndex_++;
	
	if(readIndexInc>=PRIME_SIZE-1){
		readIndexInc=0;
	}
	else{
		readIndexInc++;
	}
}

char OdyNoise::filterPink(char in){
	int pink, white;
	static int b0,b1,b2;
	white = (int)in;
	b0 = ((255 * b0)>>8) + ((white * 25)>>8);
	b1 = ((246 * b1)>>8) + ((white * 75)>>8);
	b2 = ((145 * b2)>>8) + ((white * 269)>>8);
	pink = b0 + b1 + b2 + ((white * 47)>>8);
	if(pink>127){
		return 127;
	}
	else if(pink<-127){
		return -127;
	}
	else{
		return (char)pink;
	}
}
