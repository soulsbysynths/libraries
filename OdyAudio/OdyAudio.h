/* 
* OdyAudio.h
*
* Created: 05/10/2015 11:17:58
* Author: Paul Soulsby
*/


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
	unsigned char getWtIndex(unsigned char oscNum);
	void initialize();
protected:
private:
	OdyAudio( const OdyAudio &c );
	OdyAudio& operator=( const OdyAudio &c );

}; //OdyAudio

#endif //__ODYAUDIO_H__
