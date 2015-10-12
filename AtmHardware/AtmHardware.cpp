//AtmHardware.cpp  Atmegatron Hardware control.
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

#include "AtmHardware.h"

//#define DEBUG 1

static const char rotEncoderIncrementAmount[16] PROGMEM = {0,1,-1,2,-1,0,-2,1,1,-2,0,-1,2,-1,1,0};
static const unsigned char rotEncoderBitMask[2] PROGMEM = {0x30,0x84};
static const unsigned char rotEncoderPins[2][2] PROGMEM = {{PIND4,PIND5},{PIND7,PIND2}};

static volatile unsigned char midiBuffer[MIDI_BUFFER_SIZE] = {0};
static volatile unsigned char midiWritePos = 0;
static volatile unsigned char midiReadPos = 0;
static volatile AtmHardware::MidiError midiError = AtmHardware::MIDIERR_NONE;

static volatile int rotEncoderCount[2] = {0};
static volatile unsigned char switchState[3] = {0};
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
AtmHardware::AtmHardware(AtmHardwareBase* base)
{
	construct(base);
} //AtmHardware

// default destructor
AtmHardware::~AtmHardware()
{
	unsigned char i;
	if(base_!=NULL)
	{
		delete base_;
	}

} //~AtmHardware

void AtmHardware::construct(AtmHardwareBase* base)
{
	unsigned char i,j,a;
	
	base_ = base;

	cli();
	
	beginSpi();
	
	for(i=0;i<3;i++)
	{
		writeByte(i, IOCON, ADDR_ENABLE);
		writeWord(i, IODIRA, 0x00);                // set all to outputs
	}
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
	//setup rotary encoders
	for(i=0;i<2;i++)
	{
		for(j=0;j<2;j++)
		{
			bitClear(DDRD,pgm_read_byte(&(rotEncoderPins[i][j])));
			bitSet(PORTD,pgm_read_byte(&(rotEncoderPins[i][j])));
		}
	}
	//setup pins
	//PB0 = function, PB1 = value, PD6 = bank, PD3 = audio output
	bitClear(DDRB,PINB0);
	bitClear(PORTB,PINB0);
	bitClear(DDRB,PINB1);
	bitClear(PORTB,PINB1);
	bitClear(DDRD,PIND6);
	bitClear(PORTD,PIND6);
	bitSet(DDRD,PIND3);
	bitClear(PORTD,PIND3);
	//initialize variables
	unsigned char invPinB = ~PINB;
	switchState[0] = bitRead(invPinB,PINB0);
	switch_[0] = Switch(bitRead(invPinB,PINB0),HOLD_EVENT_TICKS);
	if(switch_[0].getState()==HIGH)
	{
		midiChannelSelectMode_ = true;
	}
	switchState[1] = bitRead(invPinB,PINB1);
	switch_[1] = Switch(bitRead(invPinB,PINB1),HOLD_EVENT_TICKS);
	unsigned char invPind = ~PIND;
	switchState[2] = bitRead(invPind,PIND6);
	switch_[2] = Switch(bitRead(invPind,PIND6),HOLD_EVENT_TICKS);
	//PCINT0 = function, PCINT1 = value, PCINT22 = bank, PCINT20 = func rotEncoder_ A, PCINT21 = func rotEncoder_ B, PCINT23 = val rotEncoder_ A, PCINT18 = val rotEncoder_ B
	bitSet(PCMSK0,PCINT0);
	bitSet(PCMSK0,PCINT1);
	bitSet(PCIFR,PCIF0);
	bitSet(PCICR,PCIE0);
	bitSet(PCMSK2,PCINT18);
	bitSet(PCMSK2,PCINT20);
	bitSet(PCMSK2,PCINT21);
	bitSet(PCMSK2,PCINT22);
	bitSet(PCMSK2,PCINT23);
	bitSet(PCIFR,PCIF2);
	bitSet(PCICR,PCIE2);
	
	sei();
	
	midiThru_ = eeprom_read_byte((uint8_t*)MIDI_CHANNEL_ADDRESS) & 0x80;
	midiChannel_ = eeprom_read_byte((uint8_t*)MIDI_CHANNEL_ADDRESS) & 0x0F;
	
	if(midiChannelSelectMode_==true)
	{
		rotEncoder_[FUNCTION].setContinuous(true);
		if(midiThru_==MIDI_THRU_ON)
		{
			ledSwitch_[FUNCTION].setColour(LedRgb::RED);
		}
		else
		{
			ledSwitch_[FUNCTION].setColour(LedRgb::YELLOW);
		}
		ledSwitch_[VALUE].setColour(LedRgb::OFF);
		ledSwitch_[BANK].setColour(LedRgb::OFF);
		ledCircular_[FUNCTION].select(midiChannel_);
		rotEncoder_[FUNCTION].setValue((char)midiChannel_);
	}
}

bool AtmHardware::refreshFlash(unsigned char ticksPassed)
{
	unsigned char i;
	bool flashing = false;
	static unsigned char tickMult = 0;
	unsigned char tickInc = 0;

	tickMult += ticksPassed;
	
	if(tickMult>=64)
	{
		tickInc = tickMult >> 6;
		tickMult -= (tickInc << 6);
	}
	
	for(i=0;i<2;++i)
	{
		if(ledCircular_[i].refreshFlash(tickInc)==true)
		{
			flashing = true;
		}
	}

	for(i=0;i<3;++i)
	{
		if(ledSwitch_[i].refreshFlash(tickInc)==true)
		{
			flashing = true;
		}
	}
	
	if(ledMidi_.refreshFlash(tickInc)==true)
	{
		flashing = true;
	}
	
	return flashing;
}
void AtmHardware::refreshLeds()
{
	unsigned char i;
	LedRgb::LedRgbColour col;
	unsigned int output = 0;
	unsigned int circ = 0;
	static unsigned int mcpState[3];
	static unsigned char bankToggle = 0;
	const unsigned char RED_BIAS = 2;
	for(i=0;i<2;++i)
	{
		circ = ledCircular_[i].getState();
		if(circ!=mcpState[i])
		{
			writeWord(i, GPIOA, circ);
			mcpState[i] = circ;
		}
		
	}
	col = ledSwitch_[FUNCTION].getColour();
	if(col==LedRgb::RED || col==LedRgb::YELLOW)
	{
		bitSet(output,1);
	}
	if(col==LedRgb::GREEN || col==LedRgb::YELLOW)
	{
		bitSet(output,0);
	}
	col = ledSwitch_[VALUE].getColour();
	if(col==LedRgb::RED || col==LedRgb::YELLOW)
	{
		bitSet(output,3);
	}
	if(col==LedRgb::GREEN || col==LedRgb::YELLOW)
	{
		bitSet(output,2);
	}
	col = ledSwitch_[BANK].getColour();
	if(col==LedRgb::RED)
	{
		bitSet(output,5);
	}
	if(col==LedRgb::GREEN)
	{
		bitSet(output,6);
	}
	if(col==LedRgb::YELLOW)
	{
		bankToggle++;
		if(bankToggle>RED_BIAS)
		{
			bankToggle = 0;
		}
		if(bankToggle<RED_BIAS)
		{
			bitSet(output,5);
		}
		else
		{
			bitSet(output,6);
		}
	}
	if(ledMidi_.getState()==true)
	{
		bitSet(output,4);
	}
	if(output!=mcpState[2])
	{
		writeWord(2,GPIOA,output);
		mcpState[2] = output;
	}

}

void AtmHardware::pollAnlControls(unsigned char ticksPassed)
{
	unsigned char a;
	for(a=0;a<6;++a)
	{
		anlControl_[a].setValue(anlControlValue[a]);
		if(anlControl_[a].hasChanged(ticksPassed)==true && midiChannelSelectMode_==false)
		{
			base_->hardwareAnalogueControlChanged(a,anlControl_[a].getValue());
		}
	}

}

void AtmHardware::pollSwitches(unsigned char ticksPassed)
{
	for(unsigned char i=0;i<3;++i)
	{
		switch_[i].setState(switchState[i]);
		if(switch_[i].hasHeld(ticksPassed)==true && midiChannelSelectMode_==false)
		{
			base_->hardwareSwitchHeld(i);
		}
		if(switch_[i].hasChanged(ticksPassed)==true)
		{
			if(midiChannelSelectMode_==false)
			{
				base_->hardwareSwitchChanged(i,switch_[i].getState());
			}
			else if(midiChannelSelectMode_==true && i==FUNCTION && switch_[i].getState()==HIGH)
			{
				if(midiThru_==MIDI_THRU_ON)
				{
					midiThru_ = 0;
					ledSwitch_[FUNCTION].setColour(LedRgb::YELLOW);
				}
				else
				{
					midiThru_ = MIDI_THRU_ON;
					ledSwitch_[FUNCTION].setColour(LedRgb::RED);					
				}
				writeMidiSettings();
			}
		}
	}
}

void AtmHardware::pollRotEncoders(unsigned char ticksPassed)
{
	unsigned char i;
	for(i=0;i<2;++i)
	{
		rotEncoder_[i].setCount(rotEncoderCount[i]);
		if(rotEncoder_[i].hasChanged(ticksPassed)==true)
		{
			if(midiChannelSelectMode_==false)
			{
				base_->hardwareRotaryEncoderChanged(i,rotEncoder_[i].getValue(),rotEncoder_[i].getDirection());
			}
			else if(midiChannelSelectMode_==true && i==FUNCTION)
			{
				midiChannel_ = rotEncoder_[i].getValue();
				base_->hardwareMidiChannelChanged(midiChannel_);
				ledCircular_[FUNCTION].select(midiChannel_);
				writeMidiSettings();
			}
		}	
	}
}

void AtmHardware::pollMidi()
{
	for(unsigned char i=0;i<MIDI_BUFFER_SIZE;++i)
	{
		if(midiReadPos==midiWritePos)
		{
			break;
		}
		else
		{
			base_->hardwareMidiReceived(midiBuffer[midiReadPos]);
			if(midiThru_==MIDI_THRU_ON)
			{
				writeMidi(midiBuffer[midiReadPos]);
			}
			midiReadPos++;
			if(midiReadPos>=MIDI_BUFFER_SIZE)
			{
				midiReadPos = 0;
			}			
		}
	}

	if(midiError>MIDIERR_NONE)
	{
		base_->hardwareMidiError((unsigned char)midiError);
		midiError = MIDIERR_NONE;
	}
}
void AtmHardware::beginMidi(unsigned int ubrr)
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
void AtmHardware::writeMidi(unsigned char data )
{
	/* Wait for empty transmit buffer */
	while ( !( UCSR0A & (1<<UDRE0)) );
	/* Put data into buffer, sends the data */
	UDR0 = data;
}
void AtmHardware::flushMidi()
{
	unsigned char dummy;
	while ( UCSR0A & (1<<RXC0) ) dummy = UDR0;
	midiReadPos = 0;
	midiWritePos = 0;
}

void AtmHardware::writeMidiSettings()
{
	eeprom_update_byte((uint8_t*)MIDI_CHANNEL_ADDRESS,midiChannel_ | midiThru_);
}

void AtmHardware::beginSpi()
{
	//setup expanders
	// START up the SPI bus
	bitSet(PORTB,PINB2);
	bitSet(DDRB,PINB2);
	bitSet(SPCR,MSTR);
	bitSet(SPCR,SPE);
	bitSet(DDRB,PINB3);
	bitSet(DDRB,PINB5);
	
	// Sets the SPI bus speed  Fosc/2
	bitSet(SPSR,SPI2X);
	bitClear(SPCR,SPR1);
	bitClear(SPCR,SPR0);
	
	// Sets SPI bus bit order (this is the default, setting it for good form!)
	bitClear(SPCR,DORD);
	// Sets the SPI bus timing mode (this is the default, setting it for good form!)
	bitClear(SPCR,CPOL);
	bitClear(SPCR,CPHA);
}
void AtmHardware::writeSpi(unsigned char cData)
{
	/* START transmission */
	SPDR = cData;
	/* Wait for transmission complete */
	while(!(SPSR & (1<<SPIF)))
	;
}
// GENERIC BYTE WRITE - will write a unsigned char to a register, arguments are register address and the value to write
void AtmHardware::writeByte(unsigned char address, unsigned char reg, unsigned char value)
{      // Accept the register and unsigned char
	PORTB &= 0xFB;                                 // Direct port manipulation speeds taking Slave select LOW before SPI action
	writeSpi(OPCODEW | (address << 1));             // Send the MCP23S17 opcode, chip address, and write bit
	writeSpi(reg);                                   // Send the register we want to write
	writeSpi(value);                                 // Send the unsigned char
	PORTB |= 0x04;									// Direct port manipulation speeds taking Slave select HIGH after SPI action
}

// GENERIC WORD WRITE - will write a word to a register pair, LSB to first register, MSB to next higher value register
void AtmHardware::writeWord(unsigned char address, unsigned char reg, unsigned int word)
{  // Accept the start register and word
	PORTB &= 0xFB;                                 // Direct port manipulation speeds taking Slave select LOW before SPI action
	writeSpi(OPCODEW | (address << 1));             // Send the MCP23S17 opcode, chip address, and write bit
	writeSpi(reg);                                   // Send the register we want to write
	writeSpi((unsigned char) (word));                      // Send the low unsigned char (register address pointer will auto-increment after write)
	writeSpi((unsigned char) (word >> 8));                 // Shift the high unsigned char down to the low unsigned char location and send
	PORTB |= 0x04;                                 // Direct port manipulation speeds taking Slave select HIGH after SPI action
}

ISR(ADC_vect)
{
	static volatile unsigned char anlControlReadNum = 0;
	static const unsigned char anlControlReadOrder[10] PROGMEM = {0,1,0,2,0,3,0,4,0,5};	//read cutoff twice as often as others, becuase it needs to be smooth
	static unsigned char anlControlReadPos = 0;
	anlControlValue[anlControlReadNum] = ADCH;
	anlControlReadPos++;
	if(anlControlReadPos>9)
	{
		anlControlReadPos = 0;
	}
	anlControlReadNum = pgm_read_byte(&(anlControlReadOrder[anlControlReadPos]));
	ADMUX = anlControlReadNum;  // set the analog reference (high two bits of ADMUX) and select the channel (low 4 bits).  this also sets ADLAR (left-adjust result) to 0 (the default).
	bitSet(ADMUX,REFS0);
	bitSet(ADMUX,ADLAR);
	ADCSRA = 0x8F;
	bitSet(ADCSRA, ADSC);
}
ISR (PCINT0_vect)
{
	unsigned char invPinB = ~PINB;
	//func button
	switchState[0] = bitRead(invPinB,PINB0);
	//val button
	switchState[1] = bitRead(invPinB,PINB1);
}
ISR (PCINT2_vect)
{
	unsigned char i,j;
	static unsigned char rotEncoderStateLast[2] = {0,0};
	static unsigned char rotEncoderState[2] = {0,0};
	
	unsigned char new_rot;
	unsigned char invPind = ~PIND;
	//bank button
	switchState[2] = bitRead(invPind,PIND6);
	
	//encoders
	for(i=0;i<2;i++)
	{
		new_rot = invPind & pgm_read_byte(&(rotEncoderBitMask[i]));
		if(new_rot!=rotEncoderStateLast[i])
		{
			rotEncoderState[i] >>= 2;
			for(j=0;j<2;j++)
			{
				rotEncoderState[i] |= (bitRead(invPind,pgm_read_byte(&(rotEncoderPins[i][j]))) << (j+2));
			}
			rotEncoderCount[i] -= (char)pgm_read_byte(&(rotEncoderIncrementAmount[rotEncoderState[i]]));
			rotEncoderStateLast[i] = new_rot;
		}
	}
}
ISR(USART_RX_vect){
	
	 
	unsigned char error = UCSR0A; //have to check for error before
	unsigned char data = UDR0; 
	// Check for error
	if(bitSet(error,FE0)==1)
	{
		midiError = AtmHardware::MIDIERR_FRAME;

	}
	else if(bitSet(error,DOR0)==1)
	{
		midiError = AtmHardware::MIDIERR_OVERRUN;
	}
	else if(bitSet(error,UPE0)==1)
	{
		midiError = AtmHardware::MIDIERR_PARITY;
	}
	else
	{
		midiBuffer[midiWritePos] = data;
		midiWritePos++;
		if(midiWritePos>=MIDI_BUFFER_SIZE)
		{
			midiWritePos = 0;
		}
		if(midiWritePos==midiReadPos)
		{
			midiError = AtmHardware::MIDIERR_BUFFERFULL;
		}
	}
}