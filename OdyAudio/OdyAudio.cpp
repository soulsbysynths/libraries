/*
* OdyAudio.cpp
*
* Created: 05/10/2015 11:17:58
* Author: Paul Soulsby
*/


#include "OdyAudio.h"

static volatile unsigned int ocr0a = 142;
static volatile unsigned char timer0_prescale = 0x0B; // B00001011;
static volatile unsigned int ocr1a = 1136;
static volatile unsigned char wtIndexJump[2] = {1,1};
static volatile unsigned char wtIndex[2] = {0};
static volatile bool waveSync = false;

// default constructor
OdyAudio::OdyAudio()
{
} //OdyAudio

// default destructor
OdyAudio::~OdyAudio()
{
} //~OdyAudio

void OdyAudio::initialize()
{
	cli();      //pause interrupts
	//use internal clock
	bitClear(ASSR,EXCLK);    //disable external clock
	bitClear(ASSR,AS2);      //clock timers from internal clock

	//Fast PWM mode
	bitSet(TCCR2A,WGM20);  //TOV flag set on max
	bitClear(TCCR2A,WGM21);  //Update OCRx at bottom
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
	
	bitSet(TIMSK2,TOIE2);
	
	// Set Timer 1 to update sample on interrupt
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
	OCR1A = ocr1a;
	// Timer 1, output compare A match interrupt enable
	bitSet(TIMSK1,OCIE1A);

	// Set Timer 1 to update sample on interrupt


	// Set CTC mode (Clear timer on compare match)
	bitClear(TCCR0A,WGM00);
	bitSet(TCCR0A,WGM01);
	bitClear(TCCR0B,WGM02);

	//prescaler
	bitClear(TCCR0B,CS00);
	bitSet(TCCR0B,CS01);
	bitClear(TCCR0B,CS02);

	// Set the compare register (OCR1A) initial value
	OCR0A = ocr0a;       //initialise with A=440hz.

	// Timer 1, output compare A match interrupt enable
	bitSet(TIMSK0,OCIE0A);

	sei();    //start interrupts
}
void OdyAudio::setSampleFreq(unsigned char oscNum, unsigned long newSf)
{
	const unsigned char SCALE_OFFSET = 15;
	unsigned long ocr, testSf;
	unsigned char scale = 0;
	unsigned char i;
	if (newSf!=sampleFreq_[oscNum])
	{              
		sampleFreq_[oscNum] = newSf;     
		for(i=0;i<7;i++)
		{
			testSf = (unsigned long)1<<(SCALE_OFFSET+i);
			if(sampleFreq_[oscNum]>testSf)
			{
				scale = i+1;
			}
			else
			{
				break;
			}
		}
		wtIndexJump[oscNum] = 1 << scale;
		ocr = F_CPU/(sampleFreq_[oscNum] >> scale);
		if(oscNum==1)
		{
			ocr1a = ocr;                  
		}
		else
		{
			if (ocr>16383)
			{
				ocr0a = ocr >> 8;
				timer0_prescale = 0x0C; //B00001100;
			}
			else if (ocr>2047)
			{
				ocr0a = ocr >> 6;
				timer0_prescale = 0x0B; //B00001011;
			}
			else if (ocr>255)
			{
				ocr0a = ocr >> 3;
				timer0_prescale = 0x0A; //B00001010;
			}
			else
			{
				ocr0a = ocr;
				timer0_prescale = 0x09; //B00001001;
			}
		}
	}
}

unsigned char OdyAudio::getWtIndex(unsigned char oscNum)
{
	return wtIndex[oscNum];
}

//Interrupt loop.  Sets PWM output (i.e. generates the audio)
ISR(TIMER1_COMPA_vect) {
	OCR1A = ocr1a;
	wtIndex[1] += wtIndexJump[1];
}

//Interrupt loop.  Sets PWM output (i.e. generates the audio)
ISR(TIMER0_COMPA_vect) {
	static unsigned char lastIndex = 0;
	TCCR0B = timer0_prescale;			//master_prescale;
	OCR0A = ocr0a;                      //sets pitch
	lastIndex = wtIndex[0];
	wtIndex[0] += wtIndexJump[0];
	if(waveSync==true && wtIndex[0]<lastIndex){
		wtIndex[1] = 0;
	}
}