/* 
* OdyNoise.cpp
*
* Created: 01/10/2015 15:21:54
* Author: Paul Soulsby
*/


#include "OdyNoise.h"

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
	readIndex_ += pgm_read_byte(&(PRIME_NUMBERS[readIndexInc_]));
	return wavetable_[readIndex_];
}

void OdyNoise::refresh()
{
	
	char max = pgm_read_byte(&(NOISE_MAX[level_]));
	char min = pgm_read_byte(&(NOISE_MIN[level_]));
	char noise = randomValue(min,max);
	if(pink_==true){
		noise = filterPink(noise);
	}
	wavetable_[writeIndex_] = noise;
	writeIndex_++;
	
	if(readIndexInc_>=PRIME_SIZE-1){
		readIndexInc_=0;
	}
	else{
		readIndexInc_++;
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
