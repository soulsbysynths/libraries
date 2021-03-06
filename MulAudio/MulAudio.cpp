//MulAudio.cpp  Atmegatron Audio output (328P PWM pin)
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

#include "MulAudio.h"
#include "avr/iotnx4.h"

static volatile unsigned char audioWaveLength = 0;
static volatile unsigned char audioWaveLengthMask = 255;
static volatile unsigned int ocr1a = 568;//1136;//568;
static volatile unsigned char bufferJump = 1;
static Wavetable* audioBuffer = NULL;
static volatile bool updateOcr = false;
#ifdef UPDATE_ON_ZERO
static volatile bool doPaste = false;
static volatile bool pasting = false;
static Wavetable* audioDoubleBuffer = NULL;
#endif

// default constructor
MulAudio::MulAudio(unsigned char waveLen)
{
	resizeWavetable(waveLen);
	audioBuffer = new Wavetable(waveLen);
	#ifdef UPDATE_ON_ZERO
	audioDoubleBuffer = new Wavetable(waveLen);
	#endif
} //MulAudio

// default destructor
MulAudio::~MulAudio()
{
	if(audioBuffer != NULL)
	{
		delete audioBuffer;
	}
	#ifdef UPDATE_ON_ZERO
	if(audioDoubleBuffer != NULL)
	{
		delete audioDoubleBuffer;
	}
	#endif
} //~MulAudio

void MulAudio::initialize()
{
	//Fast PWM mode
	bitSet(TCCR0A,WGM00);  //TOV flag set on max
	bitSet(TCCR0A,WGM01);  //Update OCRx at bottom
	bitClear(TCCR0B,WGM02);  //Top = 0xFF
	
	//Set non-inverting PWM on OC0A pin
	bitClear(TCCR0A,COM0A0);
	bitSet(TCCR0A,COM0A1);
	//set normal operation on OC0B pin
	bitClear(TCCR0A,COM0B0);
	bitClear(TCCR0A,COM0B1);
	
	//no prescaler
	bitSet(TCCR0B,CS00);
	bitClear(TCCR0B,CS01);
	bitClear(TCCR0B,CS02);
	
	OCR0A = 253;

	// Set Timer 1 to update sample on interrupt

	cli();      //pause interrupts

	// Set CTC mode (Clear timer on compare match)
	bitClear(TCCR1A,WGM10);
	bitClear(TCCR1A,WGM11);
	bitSet(TCCR1B,WGM12);
	bitClear(TCCR1B,WGM13);

	//No prescaler
	bitSet(TCCR1B,CS10);
	bitClear(TCCR1B,CS11);
	bitClear(TCCR1B,CS12);

	// Set the compare register (OCR1A) initial value
	OCR1A = ocr1a;       //initialise with A=440hz.

	// Timer 1, output compare A match interrupt enable
	bitSet(TIMSK1,OCIE1A);

	sei();    //start interrupts
}

void MulAudio::setSampleFreq(unsigned long newSf)
{
	unsigned int ocr;
	unsigned char i;

	if (newSf!=sampleFreq_)
	{
		sampleFreq_ = newSf;

		for(i=0;i<8;++i)
		{
			ocr = (F_CPU<<i)/sampleFreq_;
			if(ocr>255)  //1280 //5120  ****VERY TENTATIVELY 255, ORIG=100, def too low, but why so high as 1280???
			{
				bufferJump = 1 << i;
				break;
			}
		}
		ocr1a = ocr-1;
		updateOcr = true;
		#ifdef UPDATE_ON_ZERO
		if(bufferJump<pasteJump_)
		{
			pasteJump_ = bufferJump;
			doPaste = true;  //this is because orig paste happened at bigger jump, so missing values
		}
		#endif
	}
}

void MulAudio::resizeWavetable(unsigned char newWaveLen)
{
	audioWaveLength = newWaveLen;
	audioWaveLengthMask = audioWaveLength -1;
}

void MulAudio::pasteWavetable(Wavetable& sourceWavetable)
{
	unsigned char i;
	#ifdef UPDATE_ON_ZERO
	if(pasting==false)
	{
		for(i=0;i<audioWaveLength;++i)
		{
			audioDoubleBuffer->setSample(i,sourceWavetable.getSample(i));
		}
		pasteJump_ = bufferJump;
		doPaste = true;
	}
	#else
	for(i=0;i<audioWaveLength;++i)
	{
		audioBuffer->setSample(i,sourceWavetable.getSample(i));
	}
	#endif
}

ISR(TIM1_COMPA_vect)
{
	static unsigned char jump = 1;
	static unsigned char bufferIndex = 0;

	//if(updateOcr && !bufferIndex)
	if(updateOcr && !(bufferIndex%bufferJump))
	{
		OCR1A = ocr1a;
		jump = bufferJump;
		updateOcr = false;
	}
	#ifdef UPDATE_ON_ZERO
	if(!bufferIndex)
	{
		if(doPaste)
		{
			pasting = true;
			doPaste = false;
		}
		else if(pasting)
		{
			pasting = false;
		}
	}
	if(pasting)
	{
		char samp = audioDoubleBuffer->getSample(bufferIndex);
		audioBuffer->setSample(bufferIndex, samp);
		OCR0A = samp + 128;
	}
	else
	{
		OCR0A = audioBuffer->getSample(bufferIndex) + 128;
	}
	#else
	OCR0A = audioBuffer->getSample(bufferIndex) + 128;
	#endif
	
	bufferIndex += jump;
	bufferIndex &= audioWaveLengthMask;

}