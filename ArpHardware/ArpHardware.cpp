/*
* ArpHardware.cpp
*
* Created: 11/04/2017 10:51:47
* Author: paulsoulsby
*/


#include "ArpHardware.h"

static volatile unsigned char midiBuffer[MIDI_BUFFER_SIZE] = {0};
static volatile unsigned char midiWritePos = 0;
static volatile unsigned char midiReadPos = MIDI_BUFFER_MASK;
static volatile ArpHardware::MidiError midiError = ArpHardware::MIDIERR_NONE;

static volatile unsigned int cvState[INPUTS] = {0};

static volatile bool i2cTxing = false;
static volatile bool i2cRxing = false;
static volatile unsigned char i2cWritePos = 0;
static volatile unsigned char i2cReadPos = 0;
static volatile unsigned char i2cAddr = 0;
static volatile unsigned char i2cBuffer[I2C_BUFFER_SIZE] = {0};

#define TW_TX_NACK (1 << TWINT) | (1 << TWEN) | (1 << TWIE)
#define TW_TX_ACK (1 << TWINT) | (1 << TWEN) | (1 << TWIE) | (1 << TWEA)
#define TW_STOP (1 << TWSTO) | (1 << TWINT) | (1 << TWEN)

void writeMemory(const void* data, void* startAddr, size_t size)
{
	eeprom_update_block(data, startAddr, size );
}

void readMemory(void* data, const void* startAddr, size_t size)
{
	eeprom_read_block(data,startAddr,size);

}

// default constructor
ArpHardware::ArpHardware(ArpHardwareBase* base)
{
	construct(base);
} //ArpHardware

// default destructor
ArpHardware::~ArpHardware()
{
	if(base_!=NULL)
	{
		delete base_;
	}
} //~ArpHardware

void ArpHardware::construct(ArpHardwareBase* base)
{
	unsigned char i;
	base_ = base;



	cli();
	
	beginSpi();
	_delay_ms(1);
	PORTB &= 0xFB;
	writeSpi(0x30);
	writeSpi(0x00);
	writeSpi(0x00);
	PORTB |= 0x04;
	_delay_ms(1);
	PORTB &= 0xFB;
	writeSpi(0x40);//reset, power down 1k, int ref, 2x vref, standalone
	writeSpi(0x80);
	writeSpi(0x00);
	PORTB |= 0x04;
	_delay_ms(1);

	readAllCalib();


	//setup analogue controls
	//init variables
	for(i=0;i<INPUTS;++i)
	{
		cvState[i] = readCvInput(i);
		long mapped = (((long)cvState[i] - cvCalib_[i][0]) << 11) / cvCalib_[i][2] + 1024;  //outmax = 3072 = 3.75V, outmin = 1024 = 1.25V, max-min = 2048 (<<11)
		if(mapped<0)
		{
			cvInput_[i] = CvInput(0);
		}
		else if (mapped>4095)
		{
			cvInput_[i] = CvInput(4095);
		}
		else
		{
			cvInput_[i] = CvInput((unsigned int)mapped);
		}
		cvInput_[i].setLockOut(false);
	}
	//Start_ADC interuupt and first conversion.;
	ADMUX = 0;  // set the analog reference (high two bits of ADMUX) and select the channel (low 4 bits).  this also sets ADLAR (left-adjust result) to 0 (the default).
	//bitSet(ADMUX,REFS0);
	bitSet(ADMUX,ADLAR);
	ADCSRA = 0x8F;
	bitSet(ADCSRA, ADSC);

	//setup pins
	//PD6 = clock gate, PD7 = clock detect, PB0 = root detect, PB1 = chord detect
	bitClear(DDRD,PIND6);
	bitClear(PORTD,PIND6);
	bitClear(DDRD,PIND7);
	bitClear(PORTD,PIND7);
	bitClear(DDRB,PINB0);
	bitClear(PORTB,PINB0);
	bitClear(DDRB,PINB1);
	bitClear(PORTB,PINB1);
	unsigned char invPinD = ~PIND;
	gateInput_ = Switch(bitRead(invPinD,PIND6),0);
	gateInput_.setDebounceTicks(1);

	sei();
	
	midiChannel_ = readEepromByte(EEPROM_MIDI_CHANNEL);
	
	_delay_ms(1000);  //woah that's a hell of delay to make i2c work!
	beginI2c();

	//argh frustrating - why do I need to do this twice to work from power on!!!
	writeI2cByte(0,MCP23017_IODIRA,0x60);  //ip 5 and 6 are switches
	writeI2cByte(0,MCP23017_GPPUA,0x60);  //require pullup
	writeI2cByte(0,MCP23017_IODIRB,0x00);
	writeI2cByte(0,MCP23017_GPPUB,0x00);

	unsigned char sw = readI2cByte(0,MCP23017_GPIOA);
	for(i=0;i<SWITCHES;++i)
	{
		switch_[i] = Switch(bitRead(sw ,i+SW_PINOFF),HOLD_EVENT_TICKS);
	}
}

unsigned int ArpHardware::readCvInput(unsigned char input)
{
	ADCSRA = 0;   //stop all conversions
	ADMUX = input;
	bitSet(ADMUX,ADLAR);
	ADCSRA = 0x87;
	
	bitSet(ADCSRA, ADSC);
	while (bitRead(ADCSRA, ADSC)==1);
	unsigned char l = ADCL;
	unsigned char h = ADCH;
	unsigned int i = ((unsigned int)h << 4) | (l >> 4);
	//Start_ADC interuupt and first conversion.;
	ADMUX = 0;  // set the analog reference (high two bits of ADMUX) and select the channel (low 4 bits).  this also sets ADLAR (left-adjust result) to 0 (the default).
	bitSet(ADMUX,ADLAR);
	ADCSRA = 0x8F;
	bitSet(ADCSRA, ADSC);
	return i;
}

void ArpHardware::pollSwitches(unsigned char ticksPassed)
{
	unsigned char sw = ~readI2cByte(0,MCP23017_GPIOA);
	for(unsigned char i=0;i<SWITCHES;++i)
	{
		switch_[i].setState(bitRead(sw,i+SW_PINOFF));
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
void ArpHardware::pollDigiPins(unsigned char ticksPassed)
{
	unsigned char invPin = ~PIND;
	bool newVal;
	gateInput_.setState((bool)bitRead(invPin,PIND6));
	if(gateInput_.hasChanged(ticksPassed)==true)
	{
		base_->hardwareGateInputChanged(gateInput_.getState());
	}
	newVal = (bool)bitRead(invPin,PIND7);
	if(newVal!=jackDetect_[CV_CLOCK])
	{
		jackDetect_[CV_CLOCK] = newVal;
		base_->hardwareJackDetectChanged(CV_CLOCK,newVal);
	}
	invPin = ~PINB;
	for(unsigned char i=0;i<2;++i)
	{
		newVal = (bool)bitRead(invPin,i);
		if(newVal!=jackDetect_[i])
		{
			jackDetect_[i] = newVal;
			base_->hardwareJackDetectChanged(i,newVal);
		}
	}
}

void ArpHardware::pollCalibCvInputs()
{
	long mapped;
	for(unsigned char i=0;i<INPUTS;++i)
	{
		mapped = (((long)cvState[i] - cvCalib_[i][0]) << 11) / cvCalib_[i][2] + 1024;  //outmax = 3072 = 3.75V, outmin = 1024 = 1.25V, max-min = 2048 (<<11)
		if(mapped<0)
		{
			cvInput_[i].setValue(0);
		}
		else if (mapped>4095)
		{
			cvInput_[i].setValue(4095);
		}
		else
		{
			cvInput_[i].setValue((unsigned int)mapped);
		}
		//cvInput_[i].setValue(cvState[i]);  //unmapped version
		if(cvInput_[i].hasChanged(0)==true)  //ticks not used coz lockout off
		{
			base_->hardwareCvInputChanged(i,cvInput_[i].getOutput());
		}
	}
}
void ArpHardware::pollMidi()
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
void ArpHardware::refreshFlash(unsigned char ticksPassed)
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
void ArpHardware::refreshLeds()
{
	unsigned char i;
	unsigned int mcpState = readI2cWord(0,MCP23017_GPIOA);
	unsigned int newState = led_.getState() | (mcpState & 0x0060);
	if(newState!=mcpState)
	{
		writeI2cWord(0,MCP23017_GPIOA,newState);
	}
}

void ArpHardware::beginI2c()
{
	// initialize twi prescaler and bit rate
	bitClear(TWSR, TWPS0);
	bitClear(TWSR, TWPS1);
	TWBR = ((F_CPU / F_SCL) - 16) / 2;
	
	// enable twi module, acks, and twi interrupt
	TWCR = ((1 << TWEN) | (1 << TWIE) | (1 << TWEA));
}
unsigned char ArpHardware::readI2cByte(unsigned char address, unsigned char reg)
{
	while (i2cTxing==true || i2cRxing==true || bitRead(TWCR,TWSTO)==true)
	{
	}

	i2cBuffer[0] = reg;
	i2cWritePos = 1;
	i2cTxing = true;
	i2cAddr = TW_WRITE;
	i2cAddr |= (MCP23017_ADDRESS | address) << 1;
	TWCR = ((1 << TWSTA) | (1 << TWINT) | (1 << TWEN) | (1 << TWIE) | (1 << TWEA));
	while (i2cTxing==true || bitRead(TWCR,TWSTO)==true)
	{
	}
	i2cWritePos = 1;
	i2cRxing = true;
	i2cAddr = TW_READ;
	i2cAddr |= (MCP23017_ADDRESS | address) << 1;
	TWCR = ((1 << TWSTA) | (1 << TWINT) | (1 << TWEN) | (1 << TWIE) | (1 << TWEA));
	while (i2cRxing==true || bitRead(TWCR,TWSTO)==true)
	{
	}
	return i2cBuffer[0];
}
unsigned int ArpHardware::readI2cWord(unsigned char address, unsigned char reg)
{
	while (i2cTxing==true || i2cRxing==true || bitRead(TWCR,TWSTO)==true)
	{
	}

	i2cBuffer[0] = reg;
	i2cWritePos = 1;
	i2cTxing = true;
	i2cAddr = TW_WRITE;
	i2cAddr |= (MCP23017_ADDRESS | address) << 1;
	TWCR = ((1 << TWSTA) | (1 << TWINT) | (1 << TWEN) | (1 << TWIE) | (1 << TWEA));
	while (i2cTxing==true || bitRead(TWCR,TWSTO)==true)
	{
	}
	i2cWritePos = 2;
	i2cRxing = true;
	i2cAddr = TW_READ;
	i2cAddr |= (MCP23017_ADDRESS | address) << 1;
	TWCR = ((1 << TWSTA) | (1 << TWINT) | (1 << TWEN) | (1 << TWIE) | (1 << TWEA));
	while (i2cRxing==true || bitRead(TWCR,TWSTO)==true)
	{
	}
	unsigned int output = ((unsigned int)i2cBuffer[1] << 8) | i2cBuffer[0];
	return output;
}
void ArpHardware::writeI2cByte(unsigned char address, unsigned char reg, unsigned char value)
{
	while (i2cTxing==true || i2cRxing==true || bitRead(TWCR,TWSTO)==true)
	{
	}
	i2cBuffer[0] = reg;
	i2cBuffer[1] = value;
	i2cWritePos = 2;
	i2cTxing = true;
	i2cAddr = TW_WRITE;
	i2cAddr |= (MCP23017_ADDRESS | address) << 1;
	TWCR = ((1 << TWSTA) | (1 << TWINT) | (1 << TWEN) | (1 << TWIE) | (1 << TWEA));
}
void ArpHardware::writeI2cWord(unsigned char address, unsigned char reg, unsigned int word)
{
	while (i2cTxing==true || i2cRxing==true || bitRead(TWCR,TWSTO)==true)
	{
	}
	i2cBuffer[0] = reg;
	i2cBuffer[1] = word & 0xFF;
	i2cBuffer[2] = word >> 8;
	i2cWritePos = 3;
	i2cTxing = true;
	i2cAddr = TW_WRITE;
	i2cAddr |= (MCP23017_ADDRESS | address) << 1;
	TWCR = ((1 << TWSTA) | (1 << TWINT) | (1 << TWEN) | (1 << TWIE) | (1 << TWEA));
}
void ArpHardware::beginSpi()
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
	// Sets the SPI bus timing mode (MODE 1)
	bitClear(SPCR,CPOL);
	bitSet(SPCR,CPHA);
}
unsigned char ArpHardware::writeSpi(unsigned char cData)
{
	SPDR = cData;  /* START transmission */
	while(!(SPSR & (1<<SPIF)));  /* Wait for transmission complete */
	return SPDR;
}
void ArpHardware::setOutput(unsigned int newValue)
{
	if(newValue!=output_)
	{
		output_ = newValue;
		writeCalibOutput(newValue);
	}
}

void ArpHardware::writeCalibOutput(unsigned int newValue)
{
	long mapped = (((long)newValue - outCalib_[0]) << 11) / outCalib_[2] + 1024;  //outmax = 3072 = 3.75V, outmin = 1024 = 1.25V, max-min = 2048 (<<11)
	if(mapped<0)
	{
		mapped = 0;
	}
	else if (mapped>4095)
	{
		mapped = 4095;
	}

	unsigned char outArray[3];
	////16 bit
	//outArray[0] = 0x30 | (output_ >> 12);
	//outArray[1] = (output_ >> 4) & 0xFF;
	//outArray[2] = (output_ & 0x04) << 4;
	outArray[0] = 0x30 | ((unsigned int)mapped >> 8);
	outArray[1] = (unsigned int)mapped & 0xFF;
	outArray[2] = 0;
	PORTB &= 0xFB;                                 // Direct port manipulation speeds taking Slave select LOW before SPI action
	for(unsigned char i=0;i<3;++i)
	{
		writeSpi(outArray[i]);
	}
	PORTB |= 0x04;									// Direct port manipulation speeds taking Slave select HIGH after SPI action
}
void ArpHardware::setOutCalib(unsigned char index, unsigned int newValue)
{
	outCalib_[index] = newValue;
	outCalib_[2] = outCalib_[1] - outCalib_[0];
	writeCalibOutput(output_);
}
void ArpHardware::writeOutCalib(unsigned int eepromAddress)
{
	if(eepromAddress==EEPROM_OUTPUT_LOW)
	{
		writeEepromUInt(eepromAddress,outCalib_[0]);
	}
	else
	{
		writeEepromUInt(eepromAddress,outCalib_[1]);
	}
}
void ArpHardware::calcCvCalib(unsigned int eepromAddress)
{
	const unsigned char CV_READS = 8;
	unsigned long calibRead = 0;
	unsigned char cvIp, cvHl;
	switch(eepromAddress)
	{
		case EEPROM_ROOT_LOW:
		cvIp = 0;
		cvHl = 0;
		break;
		case EEPROM_ROOT_HIGH:
		cvIp = 0;
		cvHl = 1;
		break;
		case EEPROM_CHORD_LOW:
		cvIp = 1;
		cvHl = 0;
		break;
		case EEPROM_CHORD_HIGH:
		cvIp = 1;
		cvHl = 1;
		break;
	}
	for(unsigned char i=0;i<CV_READS;++i)
	{
		calibRead += readCvInput(cvIp);
	}
	calibRead = calibRead / CV_READS;
	writeEepromUInt(eepromAddress, (unsigned int)calibRead);
	cvCalib_[cvIp][cvHl] = (unsigned int)calibRead;
	cvCalib_[cvIp][2] = cvCalib_[cvIp][1] - cvCalib_[cvIp][0];  //used in mapping equation
}
void ArpHardware::readAllCalib()
{
	cvCalib_[CV_ROOT][0] = readEepromUInt(EEPROM_ROOT_LOW);
	cvCalib_[CV_ROOT][1] = readEepromUInt(EEPROM_ROOT_HIGH);
	cvCalib_[CV_ROOT][2] = cvCalib_[CV_ROOT][1] - cvCalib_[CV_ROOT][0];  //used in mapping equation

	cvCalib_[CV_CHORD][0] = readEepromUInt(EEPROM_CHORD_LOW);
	cvCalib_[CV_CHORD][1] = readEepromUInt(EEPROM_CHORD_HIGH);
	cvCalib_[CV_CHORD][2] = cvCalib_[CV_CHORD][1] - cvCalib_[CV_CHORD][0];  //used in mapping equation

	outCalib_[0] = readEepromUInt(EEPROM_OUTPUT_LOW);
	outCalib_[1] = readEepromUInt(EEPROM_OUTPUT_HIGH);
	outCalib_[2] = outCalib_[1] - outCalib_[0];  //used in mapping equation
}
unsigned int ArpHardware::readEepromUInt(unsigned int address)
{
	unsigned char data[2];
	readMemory((void*)data,(const void*)address,sizeof(data));
	unsigned int i = ((unsigned int)data[0]<<8) | data[1];
	return i;
}
unsigned char ArpHardware::readEepromByte(unsigned int address)
{
	unsigned char data[1];
	readMemory((void*)data,(const void*)address,sizeof(data));
	return data[0];
}
void ArpHardware::writeEepromByte(unsigned int address, unsigned char newValue)
{
	unsigned char data[1] = {newValue};
	writeMemory((const void*)data,(void*)address,sizeof(data));
}
void ArpHardware::writeEepromUInt(unsigned int address, unsigned int newValue)
{
	unsigned char data[2];
	data[0] = (unsigned char)((newValue >> 8) & 0xFF);  //msb
	data[1] = (unsigned char)(newValue & 0xFF);  //lsb
	writeMemory((const void*)data,(void*)address,sizeof(data));
}
void ArpHardware::beginMidi(unsigned int ubrr)
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

ISR(ADC_vect)
{
	static volatile unsigned char anlControlReadNum = 0;
	unsigned char l = ADCL;
	unsigned char h = ADCH;
	cvState[anlControlReadNum] = ((unsigned int)h << 4) | (l >> 4);
	anlControlReadNum++;
	anlControlReadNum &= 0x01;
	ADMUX = anlControlReadNum;  // set the analog reference (high two bits of ADMUX) and select the channel (low 4 bits).  this also sets ADLAR (left-adjust result) to 0 (the default).
	bitSet(ADMUX,ADLAR);
	ADCSRA = 0x8F;
	bitSet(ADCSRA, ADSC);
}

ISR(TWI_vect)
{
	switch(TW_STATUS)
	{

		// All Master
		case TW_START:     // sent start condition
		case TW_REP_START: // sent repeated start condition
		// copy device address and r/w bit to output register and ack
		TWDR = i2cAddr;
		TWCR = TW_TX_ACK;
		i2cReadPos = 0;
		break;

		// Master Transmitter
		case TW_MT_SLA_ACK:  // slave receiver acked address
		case TW_MT_DATA_ACK: // slave receiver acked data
		// if there is data to send, send it, otherwise stop
		if(i2cReadPos<i2cWritePos)
		{
			// copy data to output register and ack
			TWDR = i2cBuffer[i2cReadPos];
			i2cReadPos++;
			TWCR = TW_TX_ACK;
		}
		else
		{
			TWCR = TW_STOP;
			i2cTxing = false;
		}
		break;

		case TW_MT_SLA_NACK:  // address sent, nack received
		case TW_MT_DATA_NACK: // data sent, nack received
		case TW_BUS_ERROR: // bus error, illegal stop/start
		TWCR = TW_STOP;
		i2cTxing = false;
		break;

		case TW_MT_ARB_LOST: // lost bus arbitration
		TWCR = ((1 << TWSTO) | (1 << TWINT) | (1 << TWEN) | (1 << TWIE));
		i2cTxing = false;
		break;

		// Master Receiver
		case TW_MR_DATA_ACK:  // data received, ack sent  // put byte into buffer
		i2cBuffer[i2cReadPos] = TWDR;
		i2cReadPos++;
		//****NO BREAK HERE - IT SENDS THE ACK
		case TW_MR_SLA_ACK:  // address sent, ack received
		// ack if more bytes are expected, otherwise nack
		if(i2cReadPos<i2cWritePos)
		{
			// copy data to output register and ack
			TWCR = TW_TX_ACK;
		}
		else
		{
			TWCR = TW_TX_NACK;
		}
		break;

		case TW_MR_DATA_NACK: // data received, nack sent
		// put final byte into buffer
		i2cBuffer[i2cReadPos] = TWDR;
		TWCR = TW_STOP;
		i2cRxing = false;
		break;

		case TW_MR_SLA_NACK: // address sent, nack received
		TWCR = TW_STOP;
		i2cRxing = false;
		break;

		// All
		case TW_NO_INFO:   // no state information
		break;

	}
}

ISR(USART_RX_vect){

	unsigned char error = UCSR0A; //have to check for error before
	unsigned char data = UDR0;
	// Check for error
	if(bitSet(error,FE0)==1)
	{
		midiError = ArpHardware::MIDIERR_FRAME;

	}
	else if(bitSet(error,DOR0)==1)
	{
		midiError = ArpHardware::MIDIERR_OVERRUN;
	}
	else if(bitSet(error,UPE0)==1)
	{
		midiError = ArpHardware::MIDIERR_PARITY;
	}
	else
	{
		unsigned char nextPos = (midiWritePos+1) & MIDI_BUFFER_MASK;
		if(nextPos==midiReadPos)
		{
			midiError = ArpHardware::MIDIERR_BUFFERFULL;
		}
		else
		{
			midiBuffer[midiWritePos] = data;
			midiWritePos = nextPos;
		}
	}
}
