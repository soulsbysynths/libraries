/*
* OdyAudio.cpp
*
* Created: 05/10/2015 11:17:58
* Author: Paul Soulsby
*/


#include "OdyAudio.h"

static volatile unsigned char ocr0a = 142;
static volatile unsigned char timer0_prescale = 0x0B; // B00001011;
static volatile unsigned int ocr1a = 1136;
static volatile unsigned char wtIndexJumpBuffer[2] = {1,1};
static volatile unsigned char wtIndex[2] = {0};
static volatile bool waveSync = false;
static volatile bool updateOcr[2] = {false};
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
	bitClear(TCCR0B,CS01);
	bitSet(TCCR0B,CS02);

	// Set the compare register (OCR1A) initial value
	OCR0A = ocr0a;       //initialise with A=440hz.

	// Timer 1, output compare A match interrupt enable
	bitSet(TIMSK0,OCIE0A);

	sei();    //start interrupts
}
void OdyAudio::setSampleFreq(unsigned char oscNum, unsigned long newSf)
{

	unsigned int ocr;
	unsigned char i;

	if (newSf!=sampleFreq_[oscNum])
	{

		sampleFreq_[oscNum] = newSf;

		for(i=0;i<8;++i)
		{
			ocr = (F_CPU<<i)/sampleFreq_[oscNum];
			if(ocr>1280)
			{
				wtIndexJumpBuffer[oscNum] = 1 << i;
				break;
			}
		}
		
		if(oscNum==0)
		{
			ocr1a = ocr;
			updateOcr[0] = true;
		}
		else
		{
			for(i=0;i<9;++i)
			{
				if(ocr<256)
				{
					switch(i)
					{
						case 0:
						timer0_prescale = 1;
						break;
						case 1:
						timer0_prescale = 1;
						wtIndexJumpBuffer[oscNum] >>= 1;
						break;
						case 2:
						timer0_prescale = 1;
						wtIndexJumpBuffer[oscNum] >>= 2;
						break;
						case 3:
						timer0_prescale = 2;
						break;
						case 4:
						timer0_prescale = 2;
						wtIndexJumpBuffer[oscNum] >>= 1;
						break;
						case 5:
						timer0_prescale = 2;
						wtIndexJumpBuffer[oscNum] >>= 2;
						break;
						case 6:
						timer0_prescale = 3;
						break;
						case 7:
						timer0_prescale = 3;
						wtIndexJumpBuffer[oscNum] >>= 1;
						break;
						case 8:
						timer0_prescale = 4;
						break;
					}
					ocr0a = (unsigned char)ocr;
					updateOcr[1] = true;
					break;
				}
				else
				{
					ocr >>= 1;
				}
			}
			
		}
	}
}

unsigned char OdyAudio::getWtIndex(unsigned char oscNum)
{
	return wtIndex[oscNum];
}

void OdyAudio::setWaveSync(bool newSync)
{
	waveSync = newSync;
}
bool OdyAudio::getWaveSync()
{
	return waveSync;
}

ISR(TIMER1_COMPA_vect)
{
	static unsigned char jump = 0;
	if(updateOcr[0]==true)
	{
		OCR1A = ocr1a;
		jump = wtIndexJumpBuffer[0];
		updateOcr[0] = false;
	}
	unsigned char lastIndex = wtIndex[0];
	wtIndex[0] += jump;
	if(waveSync==true && wtIndex[0]<lastIndex)
	{
		wtIndex[1] = 0;
	}
}


ISR(TIMER0_COMPA_vect)
{
	static unsigned char jump = 0;
	if(updateOcr[1]==true)
	{
		TCCR0B = timer0_prescale;
		jump = wtIndexJumpBuffer[1];
		OCR0A = ocr0a;
		updateOcr[1] = false;
	}
	wtIndex[1] += jump;
}

