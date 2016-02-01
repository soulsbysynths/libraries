/*
* Ade33Hardware.h
*
* Created: 02/01/2016 13:35:28
* Author: Paul Soulsby
*/


#ifndef __ADE33HARDWARE_H__
#define __ADE33HARDWARE_H__

#include <avr/io.h>
#include <avr/interrupt.h>
//#include <stdlib.h>
#include <avr/eeprom.h>

#include "Ade33HardwareBase.h"
#include "Switch.h"
#include "CD4051.h"
#include "Led.h"

#ifndef bitRead
#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#endif
#ifndef bitSet
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#endif
#ifndef bitClear
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#endif
#ifndef bitWrite
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))
#endif

#define ANAL_INPUTS 4

class Ade33Hardware
{
	//variables
	public:
	enum DigiInput : unsigned char
	{
		DIGI_MODE,
		DIGI_A_SW,
		DIGI_A_EXT
	};
	enum AnalInput : unsigned char
	{
		ANAL_A1,
		ANAL_A2,
		ANAL_B,
		ANAL_MODE
	};
	enum SwVersion : unsigned char
	{
		SW_LOGIC
	};
	enum AnalDigiValue : char
	{
		FLOATING = -1,
		LOW = 0,
		HIGH = 1
	};
	static const unsigned int HOLD_EVENT_TICKS = 500;

	protected:
	private:
	static const int CLOCK_EDGE_MAX_TICKS = 10;
	Ade33HardwareBase* base_;
	SwVersion swVersion_ = SW_LOGIC;
	Switch digiInput_[3];
	unsigned char analInput_[ANAL_INPUTS] = {0};
	unsigned char analTicksFloating_[ANAL_INPUTS] = {0};
	CD4051 demplex_;
	Led led_[2];
	bool output_ = false;
	//functions
	public:
	static Ade33Hardware& getInstance()
	{
		static Ade33Hardware instance; // Guaranteed to be destroyed.
		return instance;  // Instantiated on first use.
	}
	void construct(Ade33HardwareBase* base, SwVersion swVersion);
	Switch& getDigiInput(unsigned char index) { return digiInput_[index]; }
	const Switch& getDigiInput(unsigned char index) const { return digiInput_[index]; }
	unsigned char getAnalInput(unsigned char index) { return analInput_[index]; }
	CD4051& getDemultiplexer() { return demplex_; }
	const CD4051& getDemultiplexer() const { return demplex_; }
	Led& getLed(unsigned char index) { return led_[index]; }
	const Led& getLed(unsigned char index) const { return led_[index]; }
	void setOutput(bool newValue);
	bool getOutput(){return output_;}
	void refreshLeds();
	void refreshFlash(unsigned char ticksPassed);
	void pollAnalInputs(unsigned char ticksPassed);
	void pollDigiInputs(unsigned char ticksPassed);
	protected:
	private:
	Ade33Hardware() {}
	Ade33Hardware(Ade33HardwareBase* base);
	~Ade33Hardware();
	Ade33Hardware( const Ade33Hardware &c );
	Ade33Hardware& operator=( const Ade33Hardware &c );
	AnalDigiValue convertAnalDigi(unsigned char anal);
}; //Ade33Hardware

#endif //__ADE33HARDWARE_H__
