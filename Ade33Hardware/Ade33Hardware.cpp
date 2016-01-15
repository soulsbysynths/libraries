/*
* Ade33Hardware.cpp
*
* Created: 02/01/2016 13:35:28
* Author: Paul Soulsby
*/


#include "Ade33Hardware.h"

static volatile unsigned char analState[4] = {0};
static volatile unsigned char analControlReadOrder[4] = {0};
static volatile bool analDigiPin[4] = {false};
static volatile unsigned char digiState[3] = {0};

void writeMemory(const void* data, void* startAddr, size_t size)
{
	eeprom_update_block(data, startAddr, size );
}

void readMemory(void* data, const void* startAddr, size_t size)
{
	eeprom_read_block(data,startAddr,size);
}

// default constructor
Ade33Hardware::Ade33Hardware(Ade33HardwareBase* base)
{
	construct(base, SW_LOGIC);
} //Ade33Hardware

// default destructor
Ade33Hardware::~Ade33Hardware()
{
	if(base_!=NULL)
	{
		delete base_;
	}
} //~Ade33Hardware

void Ade33Hardware::construct(Ade33HardwareBase* base, SwVersion swVersion)
{
	unsigned char i,a;
	
	base_ = base;
	swVersion_ = swVersion;
	
	cli();

	if(swVersion_==SW_LOGIC)
	{
		//setup poll order
		for(i=0;i<3;++i)
		{
			analDigiPin[i] = true;
		}
		analDigiPin[ANAL_MODE] = false;
		for(i=0;i<4;++i)
		{
			analControlReadOrder[i] = ANAL_MODE;
		}
		//setup analogue controls
		//init variables
		ADMUX = ANAL_MODE;
		bitSet(ADMUX,REFS0);
		bitSet(ADMUX,ADLAR);
		ADCSRA = 0x87;
		bitSet(ADCSRA, ADSC);
		while (bitRead(ADCSRA, ADSC)==1);
		a = ADCH;
		analInput_[ANAL_MODE] = a;
		analState[ANAL_MODE] = a;

		//Start_ADC interuupt and first conversion.;
		ADMUX = ANAL_MODE;  // set the analog reference (high two bits of ADMUX) and select the channel (low 4 bits).  this also sets ADLAR (left-adjust result) to 0 (the default).
		bitSet(ADMUX,REFS0);
		bitSet(ADMUX,ADLAR);
		ADCSRA = 0x8F;
		bitSet(ADCSRA, ADSC);
	}
	else
	{
		////setup analogue controls
		////init variables
		//for(i=0;i<ANAL_INPUTS;++i)
		//{
		//ADMUX = i;
		//bitSet(ADMUX,REFS0);
		//bitSet(ADMUX,ADLAR);
		//ADCSRA = 0x87;
		//bitSet(ADCSRA, ADSC);
		//while (bitRead(ADCSRA, ADSC)==1);
		//a = ADCH;
		//analInput_[i] = a;
		//analState[i] = a;
		//}
		////Start_ADC interuupt and first conversion.;
		//ADMUX = 0;  // set the analog reference (high two bits of ADMUX) and select the channel (low 4 bits).  this also sets ADLAR (left-adjust result) to 0 (the default).
		//bitSet(ADMUX,REFS0);
		//bitSet(ADMUX,ADLAR);
		//ADCSRA = 0x8F;
		//bitSet(ADCSRA, ADSC);
	}


	//setup pins
	//PD2 = mode, PB1 = mode sw, PB2 = mode ext, PD3 = output, PD5/6/7 demultiplexer, PD4 led a1, PB0 led a2
	bitClear(DDRD,PIND2);
	bitClear(PORTD,PIND2);
	bitClear(DDRB,PINB1);
	bitClear(PORTB,PINB1);
	bitClear(DDRB,PINB2);
	bitClear(PORTB,PINB2);
	bitSet(DDRD,PIND3);
	bitClear(PORTD,PIND3);
	bitSet(DDRD,PIND5);
	bitClear(PORTD,PIND5);
	bitSet(DDRD,PIND6);
	bitClear(PORTD,PIND6);
	bitSet(DDRD,PIND7);
	bitClear(PORTD,PIND7);
	bitSet(DDRD,PIND4);
	bitClear(PORTD,PIND4);
	bitSet(DDRB,PINB0);
	bitClear(PORTB,PINB0);
	
	//C0 = a1, C1 = a2, C2 = b
	if(swVersion_==SW_LOGIC)
	{
		bitClear(DDRC,PINC0);
		bitClear(PORTC,PINC0);
		bitClear(DDRC,PINC1);
		bitClear(PORTC,PINC1);
		bitClear(DDRC,PINC2);
		bitClear(PORTC,PINC2);
	}
	
	//initialize variables
	unsigned char invPind = PIND;
	digiState[DIGI_MODE] = bitRead(invPind,PIND2);
	digiInput_[DIGI_MODE] = Switch(bitRead(invPind,PIND2),HOLD_EVENT_TICKS);
	
	unsigned char invPinB = PINB;
	digiState[DIGI_A_SW] = bitRead(invPinB,PINB1);
	digiInput_[DIGI_A_SW] = Switch(bitRead(invPinB,PINB1),HOLD_EVENT_TICKS);
	digiState[DIGI_A_EXT] = bitRead(invPinB,PINB2);
	digiInput_[DIGI_A_EXT] = Switch(bitRead(invPinB,PINB2),HOLD_EVENT_TICKS);
	digiInput_[DIGI_A_EXT].setDebounceTicks(0);
	if(swVersion_==SW_LOGIC)
	{
		unsigned char invPinC = PINC;
		analState[ANAL_A1] = bitRead(invPinC,PINC0);
		analInput_[ANAL_A1] = bitRead(invPinC,PINC0);
		analState[ANAL_A2] = bitRead(invPinC,PINC1);
		analInput_[ANAL_A2] = bitRead(invPinC,PINC1);
		analState[ANAL_B] = bitRead(invPinC,PINC2);
		analInput_[ANAL_B] = bitRead(invPinC,PINC2);
	}

	//PCINT18 = mode, PCINT1 = mode sw, PCINT2 = mode ext
	bitSet(PCMSK2,PCINT18);
	bitSet(PCIFR,PCIF2);
	bitSet(PCICR,PCIE2);
	
	bitSet(PCMSK0,PCINT1);
	bitSet(PCMSK0,PCINT2);
	bitSet(PCIFR,PCIF0);
	bitSet(PCICR,PCIE0);
	if(swVersion_==SW_LOGIC)
	{
		bitSet(PCMSK1,PCINT8);
		bitSet(PCMSK1,PCINT9);
		bitSet(PCMSK1,PCINT10);
		bitSet(PCIFR,PCIF1);
		bitSet(PCICR,PCIE1);
	}
	sei();
	
}

void Ade33Hardware::setOutput(bool newValue)
{
	output_ = !newValue;  //temp bug fix
	bitWrite(PORTD,PORTD3,output_);
}

void Ade33Hardware::refreshFlash(unsigned char ticksPassed)
{
	unsigned char i;
	static unsigned char tickMult = 0;
	unsigned char tickInc = 0;

	tickMult += ticksPassed;
	
	if(tickMult>=64)
	{
		tickInc = tickMult >> 6;
		tickMult -= (tickInc << 6);
	}
	
	demplex_.refreshFlash(tickInc);
	for(i=0;i<2;++i)
	{
		led_[i].refreshFlash(tickInc);
	}
}

void Ade33Hardware::refreshLeds()
{

	unsigned char i;
	for(i=0;i<3;++i)
	{
		bitWrite(PORTD,PORTD5+i,demplex_.getInput(i));
	}
	bitWrite(PORTD,PORTD4,led_[0].getState()) ;
	bitWrite(PORTB,PORTB0,led_[1].getState());
}

void Ade33Hardware::pollAnalInputs(unsigned char ticksPassed)
{
	if(swVersion_==SW_LOGIC)
	{
		for(unsigned char i=0;i<ANAL_INPUTS;++i)
		if(analState[i]!=analInput_[i])
		{
			analInput_[i] = analState[i];
			base_->hardwareAnalogueInputChanged(i,analInput_[i]);
		}
	}

	//static AnalDigiValue lastClockEdge[4] = {LOW};
	//
	//if(swVersion_==SW_LOGIC)
	//{
	////I'm unconviced that this will work for other scenarios.  Need to do tests that show feed back of result of if statement
	//for(unsigned char i=0;i<4;++i)
	//{
	//if(convertAnalDigi(analState[i])==HIGH && lastClockEdge[i]==LOW && analTicksFloating_[i]<CLOCK_EDGE_MAX_TICKS)
	//{
	//base_->hardwareAnalogueClockEdge(i,true);
	//lastClockEdge[i] = HIGH;
	//}
	//else if(convertAnalDigi(analState[i])==LOW && lastClockEdge[i]==HIGH && analTicksFloating_[i]<CLOCK_EDGE_MAX_TICKS)
	//{
	//base_->hardwareAnalogueClockEdge(i,false);
	//lastClockEdge[i] = LOW;
	//}
	//else if(convertAnalDigi(analState[i])!=convertAnalDigi(analInput_[i]))
	//{
	//analTicksFloating_[i] = 0;
	//}
	//else if(convertAnalDigi(analState[i])==FLOATING && analTicksFloating_[i]<CLOCK_EDGE_MAX_TICKS)
	//{
	//analTicksFloating_[i] += ticksPassed;
	//}
	//else
	//{
	//if(analState[i]!=analInput_[i])
	//{
	//base_->hardwareAnalogueInputChanged(i,analState[i]);
	//}
	//}
	//
	//analInput_[i] = analState[i];
	//}
	//}
}
Ade33Hardware::AnalDigiValue Ade33Hardware::convertAnalDigi(unsigned char anal)
{
	//random invert somewhere?
	if(anal==255)
	{
		return LOW;
	}
	else if (anal==0)
	{
		return HIGH;
	}
	else
	{
		return FLOATING;
	}
}
void Ade33Hardware::pollDigiInputs(unsigned char ticksPassed)
{
	for(unsigned char i=0;i<3;++i)
	{
		digiInput_[i].setState(digiState[i]);
		if(digiInput_[i].hasHeld(ticksPassed)==true)
		{
			base_->hardwareSwitchHeld(i);
		}
		if(digiInput_[i].hasChanged(ticksPassed)==true)
		{
			base_->hardwareSwitchChanged(i,digiInput_[i].getState());
		}
	}
}

ISR(ADC_vect)
{
	static volatile unsigned char analControlReadNum = 0;	//read cutoff twice as often as others, becuase it needs to be smooth
	static unsigned char analControlReadPos = 0;
	analState[analControlReadNum] = ADCH;
	analControlReadPos++;
	if(analControlReadPos>3)
	{
		analControlReadPos = 0;
	}
	analControlReadNum = analControlReadOrder[analControlReadPos];
	ADMUX = analControlReadNum;  // set the analog reference (high two bits of ADMUX) and select the channel (low 4 bits).  this also sets ADLAR (left-adjust result) to 0 (the default).
	bitSet(ADMUX,REFS0);
	bitSet(ADMUX,ADLAR);
	ADCSRA = 0x8F;
	bitSet(ADCSRA, ADSC);
}
ISR (PCINT0_vect)
{
	unsigned char invPinB = PINB;
	digiState[1] = bitRead(invPinB,PINB1);
	digiState[2] = bitRead(invPinB,PINB2);
}
ISR (PCINT1_vect)
{
	unsigned char i;
	unsigned char invPinC = PINC;

	for(i=0;i<4;++i)
	{
		if(analDigiPin[i]==true)
		{
			analState[i] = bitRead(invPinC,PINC0+i);
		}
	}
}
ISR (PCINT2_vect)
{
	unsigned char invPind = PIND;
	digiState[0] = bitRead(invPind,PIND2);
}