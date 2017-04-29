/*
* JamHardware.cpp
*
* Created: 18/06/2016 22:17:27
* Author: paulsoulsby
*/


#include "JamHardware.h"

#define AUDIO_BIAS 244   //(16000000 / 32768) / 2
#define PWM_RES 488 //(16000000 / 32768)

static volatile unsigned char switchState[SWITCHES] = {0};
static volatile unsigned char anlControlValue[SWITCHES] = {0};

static const unsigned char switchPin[SWITCHES] = {PIND5,PIND6,PIND7,PINB0,PINC2,PINC3};
static const unsigned char ledPin[3] = {PIND2,PIND4,PIND3};

CircularBuffer audioBuffer;

void writeMemory(const void* data, void* startAddr, size_t size)
{
	eeprom_update_block(data, startAddr, size );
}

void readMemory(void* data, const void* startAddr, size_t size)
{
	eeprom_read_block(data,startAddr,size);
}

// default constructor
JamHardware::JamHardware(JamHardwareBase* base)
{
	construct(base);
} //JamHardware

// default destructor
JamHardware::~JamHardware()
{
	if(base_!=NULL)
	{
		delete base_;
	}

} //~JamHardware

void JamHardware::construct(JamHardwareBase* base)
{
	unsigned char i,j,a;
	
	audioBuffer = CircularBuffer(256);

	base_ = base;

	cli();
	
	bitSet(PRR,PRTWI);  //turn off twi

	//setup analogue controls
	//init variables
	for(i=0;i<POTS;++i)
	{
		ADMUX = i;
		bitSet(ADMUX,REFS0);
		bitSet(ADMUX,ADLAR);
		ADCSRA = 0x87;
		bitSet(ADCSRA, ADSC);
		while (bitRead(ADCSRA, ADSC)==1);
		a = ADCH;
		anlControlValue[i] = a;
		anlControl_[i] = AnalogueControl(a);
	}

	//Start_ADC interuupt and first conversion.;
	ADMUX = 0;  // set the analog reference (high two bits of ADMUX) and select the channel (low 4 bits).  this also sets ADLAR (left-adjust result) to 0 (the default).
	bitSet(ADMUX,REFS0);
	bitSet(ADMUX,ADLAR);
	ADCSRA = 0x8F;
	bitSet(ADCSRA, ADSC);

	//setup pins
	//

	for(i=0;i<3;++i)
	{
		bitClear(DDRD,switchPin[i]);
		bitSet(PORTD,switchPin[i]);
	}
	bitClear(DDRB,switchPin[3]);
	bitSet(PORTB,switchPin[3]);
	for(i=4;i<6;++i)
	{
		bitClear(DDRC,switchPin[i]);
		bitSet(PORTC,switchPin[i]);
	}

	//LED
	for(i=0;i<3;++i)
	{
		bitSet(DDRD,ledPin[i]);  //func R
		bitClear(PORTD,ledPin[i]);
	}

	//initialize variables

	unsigned char pins[SWITCHES] = {~PIND,~PIND,~PIND,~PINB,~PINC,~PINC};

	for(i=0;i<SWITCHES;++i)
	{
		switch_[i] = Switch(bitRead(pins[i],switchPin[i]),HOLD_EVENT_TICKS);
		switchState[i] = bitRead(pins[i],switchPin[i]);
	}


	//PCINT21 = up, 22 = right, 23 = down, 0 = left, 10 = a, 11 = b
	bitSet(PCMSK0,PCINT0);
	bitSet(PCIFR,PCIF0);
	bitSet(PCICR,PCIE0);
	bitSet(PCMSK1,PCINT10);
	bitSet(PCMSK1,PCINT11);
	bitSet(PCIFR,PCIF1);
	bitSet(PCICR,PCIE1);
	bitSet(PCMSK2,PCINT21);
	bitSet(PCMSK2,PCINT22);
	bitSet(PCMSK2,PCINT23);
	bitSet(PCIFR,PCIF2);
	bitSet(PCICR,PCIE2);
	
	

	sei();
  
	lcd_ = new U8GLIB_NHD_C12832(13,11,10,12,18);
	lcd_->firstPage();
	do 
	{
		lcd_->setFont(u8g_font_osb21);
		//u8g.setFont(u8g_font_osb21);
		lcd_->drawStr( 0, 22, "Hello World!");
	} while (lcd_->nextPage());

}
void JamHardware::initializeAudio()
{
	cli();
  	bitSet(DDRB,PINB1);
  	bitClear(PORTB,PINB1);

  	bitClear(TCCR1A,WGM10);
  	bitSet(TCCR1A,WGM11);
  	bitSet(TCCR1B,WGM12);
  	bitSet(TCCR1B,WGM13);

  	bitClear(TCCR1A,COM1A0);
  	bitSet(TCCR1A,COM1A1);

  	bitSet(TCCR1B,CS10);
  	bitClear(TCCR1B,CS11);
  	bitClear(TCCR1B,CS12);



  	ICR1 = PWM_RES; //
  	OCR1A = AUDIO_BIAS;  //0 output

  	bitSet(TIMSK1,TOIE1);
	sei();
}
bool JamHardware::isAudioBufferFull()
{
	return audioBuffer.isFull();
}

void JamHardware::writeAudioBuffer(unsigned char data)
{
	audioBuffer.write(data);
}

void JamHardware::refreshFlash(unsigned char ticksPassed)
{
	unsigned char i;
	unsigned char tickInc = 0;

	ledFlashTickCnt += ticksPassed;
	while (ledFlashTickCnt>=LED_FLASH_SCALE)
	{
		ledFlashTickCnt -= LED_FLASH_SCALE;
		tickInc++;
	}
	if(tickInc>0)
	{
		led_.refreshFlash(tickInc);
	}
}
void JamHardware::refreshLEDs()
{
	for(unsigned char i=0;i<3;++i)
	{
		if(bitRead((unsigned char)led_.getColour(),i)==1)
		{
			bitSet(PORTD,ledPin[i]);
		}
		else
		{
			bitClear(PORTD,ledPin[i]);
		}
	}
}
void JamHardware::pollAnlControls(unsigned char ticksPassed)
{
	unsigned char a;
	for(a=0;a<POTS;++a)
	{
		anlControl_[a].setValue(anlControlValue[a]);
		if(anlControl_[a].hasChanged(ticksPassed)==true)
		{
			base_->hardwareAnalogueControlChanged(a,anlControl_[a].getValue());
		}
	}
}
void JamHardware::pollSwitches(unsigned char ticksPassed)
{
	for(unsigned char i=0;i<SWITCHES;++i)
	{
		switch_[i].setState(switchState[i]);
		if(switch_[i].hasHeld(ticksPassed)==true)
		{
			base_->hardwareSwitchHeld(i);
		}
		if(switch_[i].hasChanged(ticksPassed)==true)
		{
			base_->hardwareSwitchChanged(i,switch_[i].getState());
		}
	}
}
ISR(ADC_vect)
{
	static volatile unsigned char anlCtrlRead = 0;
	anlControlValue[anlCtrlRead] = ADCH;
	anlCtrlRead++;
	if(anlCtrlRead>=POTS)
	{
		anlCtrlRead = 0;
	}
	ADMUX = anlCtrlRead;  // set the analog reference (high two bits of ADMUX) and select the channel (low 4 bits).  this also sets ADLAR (left-adjust result) to 0 (the default).
	bitSet(ADMUX,REFS0);
	bitSet(ADMUX,ADLAR);
	ADCSRA = 0x8F;
	bitSet(ADCSRA, ADSC);
}
ISR (PCINT0_vect)
{
	unsigned char inv_pinb = ~PINB;
	switchState[3] = bitRead(inv_pinb,switchPin[3]);
}
ISR (PCINT1_vect)
{
	unsigned char inv_pinc = ~PINC;
	for(unsigned char i=4;i<6;++i)
	{
		switchState[i] = bitRead(inv_pinc,switchPin[i]);
	}
}
ISR (PCINT2_vect)
{
	unsigned char inv_pind = ~PIND;
	for(unsigned char i=0;i<3;++i)
	{
		switchState[i] = bitRead(inv_pind,switchPin[i]);
	}
}

ISR(TIMER1_OVF_vect, ISR_BLOCK)
{
	//unsigned char overrunToggle = 0;
	static unsigned char test = 0;
	OCR1A = (unsigned int)test + AUDIO_BIAS;//output_buffer.read();
	test++;
	//if(audioBuffer.isAvailable())
	//{
	//		OCR1A = audioBuffer.read() + AUDIO_BIAS;
	//}
	//else
	//{
	//	OCR1A = overrunToggle + AUDIO_BIAS;
	//	overrunToggle = ~overrunToggle;
	//}
}