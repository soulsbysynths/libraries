//MulAtmegaHardware.cpp  Atmegatron Hardware control.
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

#include "MulAtmegaHardware.h"

//#define DEBUG 1

static volatile unsigned char midiBuffer[MIDI_BUFFER_SIZE] = {0};
static volatile unsigned char midiWritePos = 0;
static volatile unsigned char midiReadPos = MIDI_BUFFER_MASK;
static volatile MulAtmegaHardware::MidiError midiError = MulAtmegaHardware::MIDIERR_NONE;

static volatile unsigned char rotEncoderIntChanged = 0;

void writeMemory(const void* data, void* startAddr, size_t size)
{
	eeprom_update_block(data, startAddr, size );
}

void readMemory(void* data, const void* startAddr, size_t size)
{
	eeprom_read_block(data,startAddr,size);
}
// default constructor
MulAtmegaHardware::MulAtmegaHardware(MulAtmegaHardwareBase* base)
{
	construct(base);
} //MulAtmegaHardware

// default destructor
MulAtmegaHardware::~MulAtmegaHardware()
{
	if(base_!=NULL)
	{
		delete base_;
	}

} //~MulAtmegaHardware

void MulAtmegaHardware::construct(MulAtmegaHardwareBase* base)
{
	unsigned char i,j,a;
	
	base_ = base;


	//tick
	bitSet(DDRD,PIND7);
	bitClear(PORTD,PIND7);

	cli();
	//setup rotary encoders
	//PB0 = ROT A, PB1 = ROT B
	bitClear(DDRB,PINB0);
	bitClear(PORTB,PINB0);
	bitClear(DDRB,PINB1);
	bitClear(PORTB,PINB1);
	//PCINT0 = ROT A, PCINT1 = ROT B
	bitSet(PCMSK0,PCINT0);
	bitSet(PCMSK0,PCINT1);
	bitSet(PCIFR,PCIF0);
	bitSet(PCICR,PCIE0);
	sei();

	lcd_ = new U8GLIB_NHD_C12832(5,6,U8G_PIN_NONE);
	lcd_->firstPage();
	do
	{
		//lcd_->setFont(u8g_font_helvB12);
		lcd_->setFont(u8g_font_helvB12);
		lcd_->drawStr( 0, 22, "BOOTED");
	} while (lcd_->nextPage());

	beginSpi();

	writeExpByte(EXP_ADDR_LED0, IOCON, ADDR_ENABLE);	 //enable device
	writeExpWord(EXP_ADDR_LED0, IODIRA, 0x0000);                // set all to outputs
	writeExpByte(EXP_ADDR_LED1, IOCON, ADDR_ENABLE);	 //enable device
	writeExpWord(EXP_ADDR_LED1, IODIRA, 0x0000);                // set all to outputs
	writeExpByte(EXP_ADDR_LED2, IOCON, ADDR_ENABLE);	 //enable device
	writeExpWord(EXP_ADDR_LED2, IODIRA, 0x0000);                // set all to outputs
	writeExpByte(EXP_ADDR_SPINBANK, IOCON, ADDR_ENABLE);	 //enable device
	writeExpWord(EXP_ADDR_SPINBANK, IODIRA, 0x0400);                // set all to outputs

	writeExpByte(EXP_ADDR_ROTENC, IOCON, ADDR_ENABLE);	 //enable device
	writeExpWord(EXP_ADDR_ROTENC, IODIRA, 0x0F0F);                // set 0-3 of each bank - inputs, 4-7 - outputs
	writeExpWord(EXP_ADDR_ROTENC, GPPUA, 0x0707);                // set 0-7 pullup on rot encs
	writeExpByte(EXP_ADDR_ROTENC, IPOLA, 0x07);                // set 0-7 invert on rot encs
	writeExpByte(EXP_ADDR_ROTENC, IPOLB, 0x07);                // set 0-7 invert on rot encs
	writeExpByte(EXP_ADDR_ROTENC, GPINTENA, 0x07);                // set 0-7 int enable on rot encs
	writeExpByte(EXP_ADDR_ROTENC, GPINTENB, 0x07);                // set 0-7 int enable on rot encs
	readExpByte(EXP_ADDR_ROTENC, INTCAPA);   //flush int
	readExpByte(EXP_ADDR_ROTENC, INTCAPB);	//flush int

	for(i=0;i<ROTARY_ENCODERS;++i)
	{
		rotEncoder_[i].setDebounceTicks(50);
	}

	writeExpByte(EXP_ADDR_BUTTONS, IOCON, ADDR_ENABLE);	 //enable device
	writeExpWord(EXP_ADDR_BUTTONS, IODIRA, 0x01FF);                // set 0-8 inputs, reset output
	writeExpWord(EXP_ADDR_BUTTONS, GPPUA, 0x00FF);                // set 0-7 pullup on, reset off

	writeExpByte(EXP_ADDR_VOICEEN, IOCON, ADDR_ENABLE);	 //enable device
	writeExpWord(EXP_ADDR_VOICEEN, IODIRA, 0x0000);                // set all to outputs


	//adc
	for(i=0;i<ANL_CTRLS;++i)
	{
		anlControl_[i] = readAdcWord(i) >> 4;
	}

	Wire.begin();

	//cli();
	////setup analogue controls
	////init variables
	//for(i=0;i<6;++i)
	//{
	//ADMUX = i;
	//bitSet(ADMUX,REFS0);
	//bitSet(ADMUX,ADLAR);
	//ADCSRA = 0x87;
	//bitSet(ADCSRA, ADSC);
	//while (bitRead(ADCSRA, ADSC)==1);
	//a = ADCH;
	//anlControlValue[i] = a;
	////anlControl_[i] = new AnalogueControl(a);
	//anlControl_[i] = AnalogueControl(a);
	//}
	////Start_ADC interuupt and first conversion.;
	//ADMUX = 0;  // set the analog reference (high two bits of ADMUX) and select the channel (low 4 bits).  this also sets ADLAR (left-adjust result) to 0 (the default).
	//bitSet(ADMUX,REFS0);
	//bitSet(ADMUX,ADLAR);
	//ADCSRA = 0x8F;
	//bitSet(ADCSRA, ADSC);
	////setup rotary encoders
	//for(i=0;i<2;i++)
	//{
	//for(j=0;j<2;j++)
	//{
	//bitClear(DDRD,pgm_read_byte(&(rotEncoderPins[i][j])));
	//bitSet(PORTD,pgm_read_byte(&(rotEncoderPins[i][j])));
	//}
	//}
	////setup pins
	////PB0 = function, PB1 = value, PD6 = bank, PD3 = audio output
	//bitClear(DDRB,PINB0);
	//bitClear(PORTB,PINB0);
	//bitClear(DDRB,PINB1);
	//bitClear(PORTB,PINB1);
	//bitClear(DDRD,PIND6);
	//bitClear(PORTD,PIND6);
	//bitSet(DDRD,PIND3);
	//bitClear(PORTD,PIND3);
	////initialize variables
	//unsigned char invPinB = ~PINB;
	//switchState[0] = bitRead(invPinB,PINB0);
	//switch_[0] = Switch(bitRead(invPinB,PINB0),HOLD_EVENT_TICKS);
	//if(switch_[0].getState()==HIGH)
	//{
	//midiChannelSelectMode_ = true;
	//}
	//switchState[1] = bitRead(invPinB,PINB1);
	//switch_[1] = Switch(bitRead(invPinB,PINB1),HOLD_EVENT_TICKS);
	//unsigned char invPind = ~PIND;
	//switchState[2] = bitRead(invPind,PIND6);
	//switch_[2] = Switch(bitRead(invPind,PIND6),HOLD_EVENT_TICKS);
	////PCINT0 = function, PCINT1 = value, PCINT22 = bank, PCINT20 = func rotEncoder_ A, PCINT21 = func rotEncoder_ B, PCINT23 = val rotEncoder_ A, PCINT18 = val rotEncoder_ B
	//bitSet(PCMSK0,PCINT0);
	//bitSet(PCMSK0,PCINT1);
	//bitSet(PCIFR,PCIF0);
	//bitSet(PCICR,PCIE0);
	//bitSet(PCMSK2,PCINT18);
	//bitSet(PCMSK2,PCINT20);
	//bitSet(PCMSK2,PCINT21);
	//bitSet(PCMSK2,PCINT22);
	//bitSet(PCMSK2,PCINT23);
	//bitSet(PCIFR,PCIF2);
	//bitSet(PCICR,PCIE2);
	//
	//sei();
	//
	//midiThru_ = eeprom_read_byte((uint8_t*)MIDI_CHANNEL_ADDRESS) & 0x80;
	//midiChannel_ = eeprom_read_byte((uint8_t*)MIDI_CHANNEL_ADDRESS) & 0x0F;
	//
	//if(midiChannelSelectMode_==true)
	//{
	//rotEncoder_[FUNCTION].setContinuous(true);
	//if(midiThru_==MIDI_THRU_ON)
	//{
	//ledSwitch_[FUNCTION].setColour(LedRgb::RED);
	//}
	//else
	//{
	//ledSwitch_[FUNCTION].setColour(LedRgb::YELLOW);
	//}
	//ledSwitch_[VALUE].setColour(LedRgb::OFF);
	//ledSwitch_[BANK].setColour(LedRgb::OFF);
	//ledCircular_[FUNCTION].select(midiChannel_);
	//rotEncoder_[FUNCTION].setValue((char)midiChannel_);
	//}
}

void MulAtmegaHardware::refreshTick(unsigned char ticksPassed)
{
	tickBuffer_ += ticksPassed;
	if(tickBuffer_>0)
	{
		do
		{
			PORTD ^= 0x80;  //toggle B1000000
			tickBuffer_--;
			_delay_us(5);
		} while (tickBuffer_>0);
	}
}

void MulAtmegaHardware::refreshLcd()
{
	//static unsigned char i = 0;
	//char s[5];
	//snprintf ( s, 5, "%u", i );
	//lcd_->firstPage();
	//do
	//{
		////lcd_->setFont(u8g_font_helvB12);
		////u8g.setFont(u8g_font_osb21);
		//lcd_->drawStr( 0, 12, s);
	//} while (lcd_->nextPage());
	//i++;

	//static int x = 128;
	//lcd_->firstPage();
	//do
	//{
		//lcd_->drawStr( x, 12, ticker_);
	//} while (lcd_->nextPage());
	//x--;
}

void MulAtmegaHardware::demoLcd(unsigned char demo)
{
	lcd_->firstPage();
	lcd_->firstPage();
	do
	{
		switch(demo)
		{
			case SW_UP:
			lcd_->drawStr(30, 12, "THE");
			lcd_->drawStr(0, 30, "ATMULTITRON");
			break;
			case SW_DOWN:
			lcd_->drawStr(20, 21, "6 VOICE");
			break;
			case SW_LEFT:
			lcd_->drawStr(30, 12, "8 BIT");
			lcd_->drawStr(24, 30, "SYNTH");
			break;
			case SW_RIGHT:
			lcd_->drawStr(20, 12, "COMING");
			lcd_->drawStr(30, 30, "2018");
			break;
			case SW_A:
			lcd_->drawStr(0, 12, "SEQUENCER,");
			lcd_->drawStr(0, 30, "SAMPLING,");
			break;
			case SW_B:
			lcd_->drawStr(20, 12, "...FM");
			lcd_->drawStr(20, 30, "& MORE!");
			break;
		}
	} while (lcd_->nextPage());
}

void MulAtmegaHardware::setVoiceEnable(unsigned char voice, AudioChannel channel, bool way)
{
	if(way==true)
	{
		bitSet(cd4066Control_[getCD4066Index(voice)],getCD4066Bit(voice,channel));
	}
	else
	{
		bitClear(cd4066Control_[getCD4066Index(voice)],getCD4066Bit(voice,channel));
	}
}
bool MulAtmegaHardware::getVoiceEnable(unsigned char voice, AudioChannel channel)
{
	return (bool)bitRead(cd4066Control_[getCD4066Index(voice)],getCD4066Bit(voice,channel));
}

void MulAtmegaHardware::refreshFlash(unsigned char ticksPassed)
{
	unsigned char i;
	unsigned char tickInc = 0;

	ledFlashTickCnt_ += ticksPassed;
	while (ledFlashTickCnt_>=LED_FLASH_SCALE)
	{
		ledFlashTickCnt_ -= LED_FLASH_SCALE;
		tickInc++;
	}
	if(tickInc>0)
	{
		for(i=0;i<ROTARY_ENCODERS;++i)
		{
			ledCircular_[i].refreshFlash(tickInc);
		}

		for(i=0;i<LED_RGBS;++i)
		{
			ledRgb_[i].refreshFlash(tickInc);
		}
		for(i=0;i<LEDS;++i)
		{
			led_[i].refreshFlash(tickInc);
		}
		
	}
}
void MulAtmegaHardware::refreshExpanders()
{
	unsigned char i;
	LedRgb::LedRgbColour col;
	unsigned int output = 0;
	static unsigned int expState[EXPANDERS];
	static unsigned char bankToggle = 0;
	const unsigned char RED_BIAS = 1;

	for(i=0;i<ROTARY_ENCODERS;++i)
	{
		output = ledCircular_[i].getState();
		if(output!=expState[i])
		{
			writeExpWord(i, GPIOA, output);
			expState[i] = output;
		}
	}

	output = readExpWord(EXP_ADDR_SPINBANK, GPIOA) & 0xFCFF;
	col = ledRgb_[RGB_BANK].getColour();
	if(col==LedRgb::RED)
	{
		bitSet(output,8);
	}
	if(col==LedRgb::GREEN)
	{
		bitSet(output,9);
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
			bitSet(output,8);
		}
		else
		{
			bitSet(output,9);
		}
	}
	if(output!=expState[EXP_ADDR_SPINBANK])
	{
		writeExpWord(EXP_ADDR_SPINBANK, GPIOA, output);
		expState[EXP_ADDR_SPINBANK] = output;
	}

	if(rotEncoderIntChanged==0)
	{
		output = readExpWord(EXP_ADDR_ROTENC, GPIOA);
		for(i=0;i<3;++i)
		{
			col = ledRgb_[i].getColour();
			if(col==LedRgb::RED || col==LedRgb::YELLOW)
			{
				bitSet(output,5 + (i<<3));
			}
			else
			{
				bitClear(output,5 + (i<<3));
			}
			if(col==LedRgb::GREEN || col==LedRgb::YELLOW)
			{
				bitSet(output,4 + (i<<3));
			}
			else
			{
				bitClear(output,4 + (i<<3));
			}
		}
		if(output!=expState[EXP_ADDR_ROTENC])
		{
			writeExpWord(EXP_ADDR_ROTENC,GPIOA,output);
			expState[EXP_ADDR_ROTENC] = output;
		}
	}


	output = readExpWord(EXP_ADDR_BUTTONS, GPIOA);
	col = ledRgb_[RGB_GLOBAL].getColour();
	if(col==LedRgb::RED || col==LedRgb::YELLOW)
	{
		bitSet(output,10);
	}
	else
	{
		bitClear(output,10);
	}
	if(col==LedRgb::GREEN || col==LedRgb::YELLOW)
	{
		bitSet(output,9);
	}
	else
	{
		bitClear(output,9);
	}
	for(i=0;i<LEDS;++i)
	{
		if(led_[i].getState()==true)
		{
			bitSet(output,i+11);
		}
		else
		{
			bitClear(output,i+11);
		}
	}
	for(i=0;i<3;++i)
	{
		if(bitRead((unsigned char)ledRgb_[RGB_LCD].getColour(),i)==1)
		{
			bitClear(output,i+13);
		}
		else
		{
			bitSet(output,i+13);
		}
	}
	if(output!=expState[EXP_ADDR_BUTTONS])
	{
		writeExpWord(EXP_ADDR_BUTTONS,GPIOA,output);
		expState[EXP_ADDR_BUTTONS] = output;
	}

	output = 0;
	for(unsigned char i=0;i<3;++i)
	{
		output += cd4066Control_[i] << (i<<2);
	}
	if(output!=expState[EXP_ADDR_VOICEEN])
	{
		writeExpWord(EXP_ADDR_VOICEEN,GPIOA,output);
		expState[EXP_ADDR_VOICEEN] = output;
	}
}

void MulAtmegaHardware::pollAnlControls(unsigned char ticksPassed)
{
	unsigned char a;
	for(a=0;a<ANL_CTRLS;++a)
	{
		anlControl_[a].setValue(readAdcWord(a) >> 4);
		if(anlControl_[a].hasChanged(ticksPassed)==true)
		{
			base_->hardwareAnalogueControlChanged(a,anlControl_[a].getValue());
		}
	}

}

void MulAtmegaHardware::pollSwitches(unsigned char ticksPassed)
{
	unsigned int input;
	input = ~readExpWord(EXP_ADDR_SPINBANK, GPIOA);
	switch_[SW_BANK].setState(bitRead(input,10));
	if(rotEncoderIntChanged==0)
	{
		input = ~readExpWord(EXP_ADDR_ROTENC, GPIOA);
		switch_[SW_FUNCTION].setState(bitRead(input,3));
		switch_[SW_VALUE].setState(bitRead(input,11));
	}

	input = ~readExpWord(EXP_ADDR_BUTTONS, GPIOA);
	for(unsigned char i=0;i<8;++i)
	{
		switch_[i+SW_LEFT].setState(bitRead(input,i));
	}
	switch_[SW_GLOBAL].setState(bitRead(input,8));
	for(unsigned char i=0;i<SWITCHES;++i)
	{
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

void MulAtmegaHardware::pollRotEncoders(unsigned char ticksPassed)
{

	unsigned char port, pin, i, gpio;


	if(rotEncoderIntChanged>0)
	{
		port = rotEncoderIntChanged - 1;  //so happens pins from PINB = index + 1
		rotEncoderIntChanged = 3;  //pop it up to value that won't be processed
		if(port<2)
		{
			pin = readExpByte(EXP_ADDR_ROTENC, INTFA + port);
			//cap = readExpByte(EXP_ADDR_ROTENC, INTCAPA + port) & pin;
			for(i=0;i<ROTARY_ENCODERS;++i)
			{
				if(bitRead(pin,i))
				{
					if(port==0)
					{
						rotEncoder_[i].setCount(rotEncoder_[i].getCount() + 4);
					}
					else
					{
						rotEncoder_[i].setCount(rotEncoder_[i].getCount() - 4);
					}
				}
			}
		}

	}
	for(i=0;i<ROTARY_ENCODERS;++i)
	{
		if(rotEncoder_[i].hasChanged(ticksPassed)==true)
		{
			base_->hardwareRotaryEncoderChanged(i,rotEncoder_[i].getValue(),rotEncoder_[i].getDirection());
		}
	}

	gpio = readExpByte(EXP_ADDR_ROTENC,GPIOA) & 0x07;
	gpio |= (readExpByte(EXP_ADDR_ROTENC,GPIOB) & 0x07);
	if(gpio==0)
	{
		rotEncoderIntChanged = 0;
	}
	
}

void MulAtmegaHardware::pollMidi()
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
			if(midiThru_==MIDI_THRU_ON)
			{
				txMidi(midiBuffer[midiReadPos]);
			}
		}
	}

	if(midiError>MIDIERR_NONE)
	{
		base_->hardwareMidiError((unsigned char)midiError);
		midiError = MIDIERR_NONE;
	}
}
void MulAtmegaHardware::beginMidi(unsigned int ubrr)
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
void MulAtmegaHardware::txMidi(unsigned char data )
{
	/* Wait for empty transmit buffer */
	while ( !( UCSR0A & (1<<UDRE0)) );
	/* Put data into buffer, sends the data */
	UDR0 = data;
}
void MulAtmegaHardware::flushMidi()
{
	unsigned char dummy;
	while ( UCSR0A & (1<<RXC0) ) dummy = UDR0;
	midiReadPos = 0;
	midiWritePos = 0;
}

void MulAtmegaHardware::writeMidiSettings()
{
	eeprom_update_byte((uint8_t*)MIDI_CHANNEL_ADDRESS,midiChannel_ | midiThru_);
}

bool MulAtmegaHardware::queueI2cMessage(unsigned char id, unsigned char * data, size_t length)
{
	if(i2cMessages_>=I2C_BUFFER_SIZE) return false;
	i2cBuffer_[i2cMessages_].id = id;
	for(unsigned char i=0;i<length;++i)
	{
		i2cBuffer_[i2cMessages_].message[i] = data[i];
	}
	i2cBuffer_[i2cMessages_].messageLength = length;
	i2cMessages_++;
	return true;
}

//void MulAtmegaHardware::txSlave(unsigned char id, unsigned char * data, size_t length)
//{
	//Wire.beginTransmission(id);
	//while (Wire.write(data,length)==0)
	//{
	//}
	//Wire.endTransmission();
//}

void MulAtmegaHardware::pollI2cBuffer(unsigned char ticksPassed)
{
	static unsigned char freeToTransmit = 0;
	if (freeToTransmit>5)
	{
		if(i2cMessages_>0)
		{
			i2cMessages_--;
			Wire.beginTransmission(i2cBuffer_[i2cMessages_].id);
			while (Wire.write(i2cBuffer_[i2cMessages_].message,i2cBuffer_[i2cMessages_].messageLength)==0)
			{
			}
			Wire.endTransmission();
			freeToTransmit = 0;
		}
	} 
	else
	{
		freeToTransmit += ticksPassed;
	}
	 
}

void MulAtmegaHardware::beginSpi()
{
	//setup expanders
	// START up the SPI bus

	bitSet(PORTB,PINB2);
	bitSet(DDRB,PINB2);
	bitSet(PORTC,PINC1);
	bitSet(DDRC,PINC1);
	bitSet(PORTC,PINC2);
	bitSet(DDRC,PINC2);
	bitSet(PORTC,PINC3);
	bitSet(DDRC,PINC3);
	bitSet(PORTD,PIND5);
	bitSet(DDRD,PIND5);

	bitSet(SPCR,MSTR);
	bitSet(SPCR,SPE);
	bitSet(DDRB,PINB3);
	bitSet(DDRB,PINB5);
	bitClear(DDRB,PINB4); //?
	bitClear(PORTB,PINB4); //?
	
	// Sets the SPI bus speed  Fosc/2
	bitSet(SPSR,SPI2X);
	bitClear(SPCR,SPR1);
	bitSet(SPCR,SPR0);
	
	// Sets SPI bus bit order (this is the default, setting it for good form!)
	bitClear(SPCR,DORD);
	// Sets the SPI bus timing mode (this is the default, setting it for good form!)
	bitClear(SPCR,CPOL);
	bitClear(SPCR,CPHA);
}
unsigned char MulAtmegaHardware::writeSpi(unsigned char cData)
{
	cli();
	/* START transmission */
	SPDR = cData;
	/* Wait for transmission complete */
	while(!(SPSR & (1<<SPIF)))
	;
	sei();
	return SPDR;
}
// GENERIC BYTE WRITE - will write a unsigned char to a register, arguments are register address and the value to write
void MulAtmegaHardware::writeExpByte(unsigned char address, unsigned char reg, unsigned char value)
{      // Accept the register and unsigned char
	PORTC &= 0xFB;                                 // Direct port manipulation speeds taking Slave select LOW before SPI action
	writeSpi(OPCODEW | (address << 1));             // Send the MCP23S17 opcode, chip address, and write bit
	writeSpi(reg);                                   // Send the register we want to write
	writeSpi(value);                                 // Send the unsigned char
	PORTC |= 0x04;									// Direct port manipulation speeds taking Slave select HIGH after SPI action
}

// GENERIC WORD WRITE - will write a word to a register pair, LSB to first register, MSB to next higher value register
void MulAtmegaHardware::writeExpWord(unsigned char address, unsigned char reg, unsigned int word)
{  // Accept the start register and word
	PORTC &= 0xFB;                                 // Direct port manipulation speeds taking Slave select LOW before SPI action
	writeSpi(OPCODEW | (address << 1));             // Send the MCP23S17 opcode, chip address, and write bit
	writeSpi(reg);                                   // Send the register we want to write
	writeSpi((unsigned char) (word));                      // Send the low unsigned char (register address pointer will auto-increment after write)
	writeSpi((unsigned char) (word >> 8));                 // Shift the high unsigned char down to the low unsigned char location and send
	PORTC |= 0x04;                                 // Direct port manipulation speeds taking Slave select HIGH after SPI action
}

unsigned char MulAtmegaHardware::readExpByte(unsigned char address, unsigned char reg) {        // This function will read a single register, and return it
	uint8_t value = 0;                        // Initialize a variable to hold the read values to be returned
	PORTC &= 0xFB;                      // Direct port manipulation speeds taking Slave Select LOW before SPI action
	writeSpi(OPCODER | (address << 1));  // Send the MCP23S17 opcode, chip address, and read bit
	writeSpi(reg);                        // Send the register we want to read
	value =writeSpi(0x00);               // Send any byte, the function will return the read value
	PORTC |= 0x04;	                      // Direct port manipulation speeds taking Slave Select HIGH after SPI action
	return value;                             // Return the constructed word, the format is 0x(register value)
}

unsigned int MulAtmegaHardware::readExpWord(unsigned char address, unsigned char reg) {       // This function will read all 16 bits of I/O, and return them as a word in the format 0x(portB)(portA)
	unsigned int value = 0;                   // Initialize a variable to hold the read values to be returned
	PORTC &= 0xFB;                      // Direct port manipulation speeds taking Slave Select LOW before SPI action
	writeSpi(OPCODER | (address << 1));  // Send the MCP23S17 opcode, chip address, and read bit
	writeSpi(reg);                      // Send the register we want to read
	value = writeSpi(0x00);               // Send any byte, the function will return the read value (register address pointer will auto-increment after write)
	value |= ((unsigned int)writeSpi(0x00) << 8);       // Read in the "high byte" (portB) and shift it up to the high location and merge with the "low byte"
	PORTC |= 0x04;	                      // Direct port manipulation speeds taking Slave Select HIGH after SPI action
	return value;                             // Return the constructed word, the format is 0x(portB)(portA)
}

unsigned int MulAtmegaHardware::readAdcWord(unsigned char input)
{
	unsigned char h,l,addr;
	int out;
	PORTC &= 0xFD;                                 // Direct port manipulation speeds taking Slave select LOW before SPI action
	addr = bitRead(input,2);
	h = writeSpi(MCP3208_SINGLE | addr);
	addr = (input & 0x03) << 6;
	h = writeSpi(addr);       //unsure of top 4 bits, so chop them off
	l = writeSpi(0x00);
	PORTC |= 0x02;									// Direct port manipulation speeds taking Slave select HIGH after SPI action
	out = h & 0x0F;
	out <<= 8;
	out |= l;
	if(out<0)
	{
		return 0;
	}
	else if(out>4095)
	{
		return 4095;
	}
	else
	{
		return out;
	}
}

//ISR(ADC_vect)
//{
//static volatile unsigned char anlControlReadNum = 0;
//static const unsigned char anlControlReadOrder[10] PROGMEM = {0,1,0,2,0,3,0,4,0,5};	//read cutoff twice as often as others, becuase it needs to be smooth
//static unsigned char anlControlReadPos = 0;
//anlControlValue[anlControlReadNum] = ADCH;
//anlControlReadPos++;
//if(anlControlReadPos>9)
//{
//anlControlReadPos = 0;
//}
//anlControlReadNum = pgm_read_byte(&(anlControlReadOrder[anlControlReadPos]));
//ADMUX = anlControlReadNum;  // set the analog reference (high two bits of ADMUX) and select the channel (low 4 bits).  this also sets ADLAR (left-adjust result) to 0 (the default).
//bitSet(ADMUX,REFS0);
//bitSet(ADMUX,ADLAR);
//ADCSRA = 0x8F;
//bitSet(ADCSRA, ADSC);
//}

ISR (PCINT0_vect)
{
	unsigned char expInt = ~PINB;
	expInt &= 0x03;

	if(rotEncoderIntChanged==0)
	{
		rotEncoderIntChanged = expInt;
	}
}

ISR(USART_RX_vect){

	unsigned char error = UCSR0A; //have to check for error before
	unsigned char data = UDR0;
	// Check for error
	if(bitSet(error,FE0)==1)
	{
		midiError = MulAtmegaHardware::MIDIERR_FRAME;

	}
	else if(bitSet(error,DOR0)==1)
	{
		midiError = MulAtmegaHardware::MIDIERR_OVERRUN;
	}
	else if(bitSet(error,UPE0)==1)
	{
		midiError = MulAtmegaHardware::MIDIERR_PARITY;
	}
	else
	{
		unsigned char nextPos = (midiWritePos+1) & MIDI_BUFFER_MASK;
		if(nextPos==midiReadPos)
		{
			midiError = MulAtmegaHardware::MIDIERR_BUFFERFULL;
		}
		else
		{
			midiBuffer[midiWritePos] = data;
			midiWritePos = nextPos;
		}
	}
}
