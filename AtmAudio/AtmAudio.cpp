//AtmAudio.cpp  Atmegatron Audio output (328P PWM pin)
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

#include "AtmAudio.h"

static volatile unsigned char audioWaveLength;

static volatile unsigned int ocr1a = 1136;
static volatile unsigned char bufferJump = 1;
static Wavetable* audioBuffer = NULL;
#if UPDATE_ON_ZERO==1
static volatile bool doPaste = false;
static volatile bool pasting = false;
static Wavetable* audioDoubleBuffer = NULL;
#endif

// default constructor
AtmAudio::AtmAudio(unsigned char waveLen)
{
	audioWaveLength = waveLen;
	audioBuffer = new Wavetable(waveLen);
	#if UPDATE_ON_ZERO==1
	audioDoubleBuffer = new Wavetable(waveLen);
	#endif
} //AtmAudio

// default destructor
AtmAudio::~AtmAudio()
{
	if(audioBuffer != NULL)
	{
		delete audioBuffer;
	}
	#if UPDATE_ON_ZERO==1
	if(audioDoubleBuffer != NULL)
	{
		delete audioDoubleBuffer;
	}
	#endif
} //~AtmAudio

void AtmAudio::initialize()
{
	//use internal clock
	bitClear(ASSR,EXCLK);    //disable external clock
	bitClear(ASSR,AS2);      //clock timers from internal clock

	//Fast PWM mode
	bitSet(TCCR2A,WGM20);  //TOV flag set on max
	bitSet(TCCR2A,WGM21);  //Update OCRx at bottom
	bitClear(TCCR2B,WGM22);  //Top = 0xFF
	
	//Set non-inverting PWM on OC2B pin
	bitClear(TCCR2A,COM2B0);
	bitSet(TCCR2A,COM2B1);
	//set normal operation on OC2A pin
	bitClear(TCCR2A,COM2A0);
	bitClear(TCCR2A,COM2A1);
	
	//no prescaler
	bitSet(TCCR2B,CS20);
	bitClear(TCCR2B,CS21);
	bitClear(TCCR2B,CS22);
	
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

void AtmAudio::setSampleFreq(unsigned long newSf)
{
	if (newSf!=sampleFreq_)
	{
		sampleFreq_ = newSf;
		if (sampleFreq_>225280)
		{
			if(sampleFreq_>450560)
			{
				sampleFreq_ = 450560;
			}
			ocr1a= (F_CPU<<3)/sampleFreq_;
			bufferJump = 8;
		}
		else if (sampleFreq_>112640)
		{
			ocr1a= (F_CPU<<2)/sampleFreq_;
			bufferJump = 4;
		}
		else if (sampleFreq_>56320)
		{
			ocr1a= (F_CPU<<1)/sampleFreq_;
			bufferJump = 2;
		}
		else
		{
			if(sampleFreq_<245)
			{
				sampleFreq_ = 245;
			}
			ocr1a = F_CPU/sampleFreq_;
			bufferJump = 1;
		}
	}
}

void AtmAudio::pasteWavetable(Wavetable& sourceWavetable)
{
	#if UPDATE_ON_ZERO==1
	if(pasting==false)
	{
		doPaste = true;
		for(unsigned char i=0;i<audioWaveLength;++i)
		{
			audioDoubleBuffer->setSample(i,sourceWavetable.getSample(i));
		}
	}
	#else
	for(unsigned char i=0;i<audioWaveLength;++i)
	{
		audioBuffer->setSample(i,sourceWavetable.getSample(i));
	}
	#endif


}

ISR(TIMER1_COMPA_vect)
{
	static unsigned char bufferIndex = 0;
	#if UPDATE_ON_ZERO==1
	if(doPaste==true && bufferIndex==0)
	{
		pasting = true;
		doPaste = false;
	}
	if(pasting==true)
	{
		audioBuffer->setSample(bufferIndex, audioDoubleBuffer->getSample(bufferIndex));
	}
	#endif
	OCR2B = audioBuffer->getSample(bufferIndex) + 128;
	OCR1A = ocr1a;
	bufferIndex += bufferJump;
	if(bufferIndex>=audioWaveLength)
	{
		bufferIndex -= audioWaveLength;
		#if UPDATE_ON_ZERO==1
		pasting = false;
		#endif
	}
}