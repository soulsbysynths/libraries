/* 
* OdyNoise.cpp
*
* Created: 01/10/2015 15:21:54
* Author: Paul Soulsby
*/


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
