//MinHardware.cpp  Hardware control for midiAtmegatron
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

#include "MinHardware.h"

//#define DEBUG 1

static volatile unsigned char midiBuffer[MIDI_BUFFER_SIZE] = {0};
static volatile unsigned char midiWritePos = 0;
static volatile unsigned char midiReadPos = 0;
static volatile MinHardware::MidiError midiError = MinHardware::MIDIERR_NONE;

static volatile unsigned char switchState[6] = {0};
static volatile unsigned char anlControlValue[6] = {0};

void writeMemory(const void* data, void* startAddr, size_t size)
{
	eeprom_update_block(data, startAddr, size );
}

void readMemory(void* data, const void* startAddr, size_t size)
{
	eeprom_read_block(data,startAddr,size);
}

// default constructor
MinHardware::MinHardware(MinHardwareBase* base)
{
	construct(base);
} //MinHardware

// default destructor
MinHardware::~MinHardware()
{
	unsigned char i;
	if(base_!=NULL)
	{
		delete base_;
	}
} //~MinHardware

void MinHardware::construct(MinHardwareBase* base)
{
	unsigned char i,j,a;
	
	base_ = base;

	cli();
	
	//setup analogue controls
	//init variables
	for(i=0;i<6;++i)
	{
		ADMUX = i;
		bitSet(ADMUX,REFS0);
		bitSet(ADMUX,ADLAR);
		ADCSRA = 0x87;
		bitSet(ADCSRA, ADSC);
		while (bitRead(ADCSRA, ADSC)==1);
		a = ADCH;
		anlControlValue[i] = a;
		//anlControl_[i] = new AnalogueControl(a);
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
	bitClear(DDRD,PIND2);  //play
	bitClear(PORTD,PIND2);
	bitClear(DDRB,PINB5);  //func dec
	bitClear(PORTB,PINB5);
	bitClear(DDRB,PINB4);  //func inc
	bitClear(PORTB,PINB4);
	bitClear(DDRB,PINB0);  //val dec
	bitClear(PORTB,PINB0);
	bitClear(DDRD,PIND7);  //val inc
	bitClear(PORTD,PIND7);
	bitClear(DDRD,PIND4);  //bank
	bitClear(PORTD,PIND4);

	bitSet(DDRB,PINB1);  //func R
	bitClear(PORTB,PINB1);
	bitSet(DDRB,PINB3);  //func G
	bitClear(PORTB,PINB3);
	bitSet(DDRB,PINB2);  //func B
	bitClear(PORTB,PINB2);
	bitSet(DDRD,PIND5);  //val R
	bitClear(PORTD,PIND5);
	bitSet(DDRD,PIND6);  //val G
	bitClear(PORTD,PIND6);

	bitSet(DDRD,PIND3);  //output
	bitClear(PORTD,PIND3);

	//initialize variables
	unsigned char inv_pinb = PINB;
	unsigned char inv_pind = PIND;
	switch_[0] = Switch(bitRead(inv_pind,PIND2),HOLD_EVENT_TICKS);
	switch_[0].setDebounceTicks(25);  //shorter for the play button
	switch_[1] = Switch(bitRead(inv_pinb,PINB5),HOLD_EVENT_TICKS);
	switch_[2] = Switch(bitRead(inv_pinb,PINB4),HOLD_EVENT_TICKS);
	switch_[3] = Switch(bitRead(inv_pinb,PINB0),HOLD_EVENT_TICKS);
	switch_[4] = Switch(bitRead(inv_pind,PIND7),HOLD_EVENT_TICKS);
	switch_[5] = Switch(bitRead(inv_pind,PIND4),HOLD_EVENT_TICKS);
	switchState[0] = bitRead(inv_pind,PIND2);
	switchState[1] = bitRead(inv_pinb,PINB5);
	switchState[2] = bitRead(inv_pinb,PINB4);
	switchState[3] = bitRead(inv_pinb,PINB0);
	switchState[4] = bitRead(inv_pind,PIND7);
	switchState[5] = bitRead(inv_pind,PIND4);
	//PCINT18 = play, PCINT5 = func dec, PCINT4 = fnc inc, PCINT0 = val dec,  PCINT23 = val inc, PCINT20 = bank
	bitSet(PCMSK0,PCINT5);
	bitSet(PCMSK0,PCINT4);
	bitSet(PCMSK0,PCINT0);
	bitSet(PCIFR,PCIF0);
	bitSet(PCICR,PCIE0);
	bitSet(PCMSK2,PCINT18);
	bitSet(PCMSK2,PCINT23);
	bitSet(PCMSK2,PCINT20);
	bitSet(PCIFR,PCIF2);
	bitSet(PCICR,PCIE2);
	
	sei();
	
}


void MinHardware::refreshFlash(unsigned char ticksPassed)
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
		for(i=0;i<2;++i)
		{
			led_[i].refreshFlash(tickInc);
		}
	}
}

void MinHardware::refreshLEDs()
{
#if PCB_ISSUE == 3
	if(bitRead((unsigned char)led_[LED_FUNC].getColour(),1)==1)
	{
		bitSet(PORTB,PINB1);
	}
	else
	{
		bitClear(PORTB,PINB1);
	}

	if(bitRead((unsigned char)led_[LED_FUNC].getColour(),2)==1)
	{
		bitSet(PORTB,PINB3);
	}
	else
	{
		bitClear(PORTB,PINB3);
	}

	if(bitRead((unsigned char)led_[LED_FUNC].getColour(),0)==1)
	{
		bitSet(PORTB,PINB2);
	}
	else
	{
		bitClear(PORTB,PINB2);
	}
	
	if(bitRead((unsigned char)led_[LED_VALUE].getColour(),1)==1)
	{
		bitSet(PORTD,PIND5);
	}
	else
	{
		bitClear(PORTD,PIND5);
	}
	
	if(bitRead((unsigned char)led_[LED_VALUE].getColour(),0)==1)
	{
		bitSet(PORTD,PIND6);
	}
	else
	{
		bitClear(PORTD,PIND6);
	}
#else
	if(bitRead((unsigned char)led_[LED_FUNC].getColour(),0)==1)
	{
		bitSet(PORTB,PINB1);
	}
	else
	{
		bitClear(PORTB,PINB1);
	}

	if(bitRead((unsigned char)led_[LED_FUNC].getColour(),1)==1)
	{
		bitSet(PORTB,PINB3);
	}
	else
	{
		bitClear(PORTB,PINB3);
	}

	if(bitRead((unsigned char)led_[LED_FUNC].getColour(),2)==1)
	{
		bitSet(PORTB,PINB2);
	}
	else
	{
		bitClear(PORTB,PINB2);
	}
	
	if(bitRead((unsigned char)led_[LED_VALUE].getColour(),0)==1)
	{
		bitSet(PORTD,PIND5);
	}
	else
	{
		bitClear(PORTD,PIND5);
	}
	
	if(bitRead((unsigned char)led_[LED_VALUE].getColour(),1)==1)
	{
		bitSet(PORTD,PIND6);
	}
	else
	{
		bitClear(PORTD,PIND6);
	}
#endif
}

void MinHardware::pollAnlControls(unsigned char ticksPassed)
{
	unsigned char a;
	for(a=0;a<6;++a)
	{
		anlControl_[a].setValue(anlControlValue[a]);
		if(anlControl_[a].hasChanged(ticksPassed)==true)
		{
			base_->hardwareAnalogueControlChanged(a,anlControl_[a].getValue());
		}
	}
}

void MinHardware::pollSwitches(unsigned char ticksPassed)
{
	for(unsigned char i=0;i<6;++i)
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


void MinHardware::pollMidi()
{
	for(unsigned char i=0;i<MIDI_BUFFER_SIZE;++i)
	{
		unsigned char nextPos = (midiReadPos+1) & MIDI_BUFFER_MASK;
		if(nextPos==midiWritePos)
		{
			break;
		}
		else
		{
			midiReadPos = nextPos;
			base_->hardwareMidiReceived(midiBuffer[midiReadPos]);
		}
	}

	if(midiError>MIDIERR_NONE)
	{
		base_->hardwareMidiError((unsigned char)midiError);
		midiError = MIDIERR_NONE;
	}
}
void MinHardware::beginMidi(unsigned int ubrr)
{
	/*Set baud rate */
	bitSet(UCSR0A,U2X0);
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)ubrr;
	bitSet(UCSR0B,RXEN0);
	bitSet(UCSR0B,TXEN0);
	bitSet(UCSR0B,RXCIE0);
	bitClear(UCSR0B, UDRIE0);
}
void MinHardware::writeMidi(unsigned char data )
{
	/* Wait for empty transmit buffer */
	while ( !( UCSR0A & (1<<UDRE0)) );
	/* Put data into buffer, sends the data */
	UDR0 = data;
}
void MinHardware::flushMidi()
{
	unsigned char dummy;
	while ( UCSR0A & (1<<RXC0) ) dummy = UDR0;
}
bool MinHardware::getFirstBoot()
{
	if(eeprom_read_byte((unsigned char*)FIRST_BOOT_ADDR)==FIRST_BOOT_VAL)  //anything that it's unlikely to be
	{
		return false;
	}
	else
	{
		return true;
	}
}
void MinHardware::setFirstBoot(bool newValue)
{
	if(newValue==true)
	{
		eeprom_update_byte((unsigned char*)FIRST_BOOT_ADDR,0);
	}
	else
	{
		eeprom_update_byte((unsigned char*)FIRST_BOOT_ADDR,FIRST_BOOT_VAL);
	}
}
ISR(ADC_vect)
{
	static volatile unsigned char anlCtrlRead = 0;
	anlControlValue[anlCtrlRead] = ADCH;
	anlCtrlRead++;
	if(anlCtrlRead>5)
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
	unsigned char inv_pinb = PINB;

	switchState[1] = bitRead(inv_pinb,PINB5);
	switchState[2] = bitRead(inv_pinb,PINB4);
	switchState[3] = bitRead(inv_pinb,PINB0);
}
ISR (PCINT2_vect)
{
	unsigned char inv_pind = PIND;

	switchState[0] = bitRead(inv_pind,PIND2);
	switchState[4] = bitRead(inv_pind,PIND7);
	switchState[5] = bitRead(inv_pind,PIND4);
}

ISR(USART_RX_vect){

	unsigned char error = UCSR0A; //have to check for error before
	unsigned char data = UDR0;
	// Check for error
	if(bitSet(error,FE0)==1)
	{
		midiError = MinHardware::MIDIERR_FRAME;

	}
	else if(bitSet(error,DOR0)==1)
	{
		midiError = MinHardware::MIDIERR_OVERRUN;
	}
	else if(bitSet(error,UPE0)==1)
	{
		midiError = MinHardware::MIDIERR_PARITY;
	}
	else
	{
		unsigned char nextPos = (midiWritePos+1) & MIDI_BUFFER_MASK;
		if(nextPos==midiReadPos)
		{
			midiError = MinHardware::MIDIERR_BUFFERFULL;
		}
		else
		{
			midiBuffer[midiWritePos] = data;
			midiWritePos = nextPos;
		}
	}
}