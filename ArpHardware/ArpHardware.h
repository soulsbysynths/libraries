//ArpHardware.h  AT-ARP hardware class
//Copyright (C) 2017  Paul Soulsby info@soulsbysynths.com
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


#ifndef __ARPHARDWARE_H__
#define __ARPHARDWARE_H__

#include "ArpHardwareBase.h"
#include "CvInput.h"
#include "Switch.h"
#include "SevenSegLed.h"
#include <avr/eeprom.h>
#include <util/twi.h>
#include <util/delay.h>
#include <avr/interrupt.h>

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

#define F_SCL 100000UL // SCL frequency
#define MCP23017_ADDRESS 0x20
// registers
#define MCP23017_IODIRA 0x00
#define MCP23017_IPOLA 0x02
#define MCP23017_GPINTENA 0x04
#define MCP23017_DEFVALA 0x06
#define MCP23017_INTCONA 0x08
#define MCP23017_IOCONA 0x0A
#define MCP23017_GPPUA 0x0C
#define MCP23017_INTFA 0x0E
#define MCP23017_INTCAPA 0x10
#define MCP23017_GPIOA 0x12
#define MCP23017_OLATA 0x14


#define MCP23017_IODIRB 0x01
#define MCP23017_IPOLB 0x03
#define MCP23017_GPINTENB 0x05
#define MCP23017_DEFVALB 0x07
#define MCP23017_INTCONB 0x09
#define MCP23017_IOCONB 0x0B
#define MCP23017_GPPUB 0x0D
#define MCP23017_INTFB 0x0F
#define MCP23017_INTCAPB 0x11
#define MCP23017_GPIOB 0x13
#define MCP23017_OLATB 0x15

#define MCP23017_INT_ERR 255

#define MCP3208_SINGLE    (0b00000110)
#define MCP3208_DIFFERENTIAL    (0b00000100)
#define MCP3208_OFFSET_EEPROM_ADDR 1019

#define I2C_BUFFER_SIZE 16
#define INPUTS 2
#define JACKS 3
#define SWITCHES 2
#define SW_PINOFF 5

#define MIDI_BUFFER_SIZE 64
#define MIDI_BUFFER_MASK 63

class ArpHardware
{
//variables
public:
	enum InputMode : unsigned char
	{
		IP_CV = 0,
		IP_GATE = 1
	};
	enum CvInputName : unsigned char
	{
		CV_ROOT = 0,
		CV_CHORD = 1,
		CV_CLOCK = 2,
	};
	enum SwitchName : unsigned char
	{
		SW_UP = 0,
		SW_DOWN = 1
	};
	enum EepromAddresses : unsigned int
	{	
		EEPROM_PPQN = 1010,
		EEPROM_MIDI_CHANNEL = 1011,
		EEPROM_OUTPUT_LOW = 1012, //1.25V
		EEPROM_OUTPUT_HIGH = 1014, //3.75V
		EEPROM_ROOT_LOW = 1016, //1.25V
		EEPROM_ROOT_HIGH = 1018, //3.75V
		EEPROM_CHORD_LOW = 1020,  //1.25V
		EEPROM_CHORD_HIGH = 1022  //3.75V
	};
	enum MidiError : unsigned char
	{
		MIDIERR_NONE,
		MIDIERR_FRAME,
		MIDIERR_OVERRUN,
		MIDIERR_PARITY,
		MIDIERR_BUFFERFULL
	};
	static const unsigned int HOLD_EVENT_TICKS = 1000;
	static const unsigned char SYSEX_PRODUCT = 0;
protected:
private:
	ArpHardwareBase* base_;
	InputMode inputMode_[INPUTS] = {IP_CV};
	CvInput cvInput_[INPUTS];
	SevenSegLed led_;
	Switch gateInput_;
	unsigned int cvCalib_[INPUTS][3] = {{0}};  //input (pitch, filt), low/high/high-low
	unsigned int outCalib_[3] = {0};
	Switch switch_[SWITCHES];
	bool jackDetect_[JACKS] = {false};
	unsigned char ledFlashTickCnt = 0;
	static const unsigned char LED_FLASH_SCALE = 16;
	unsigned char midiChannel_ = 0;
	unsigned char ppqnMultiply_ = 2;
	unsigned int output_  = 0;
//functions
public:
	static ArpHardware& getInstance()
	{
		static ArpHardware instance; // Guaranteed to be destroyed.
		return instance;  // Instantiated on first use.
	}
	void construct(ArpHardwareBase* base);
	CvInput& getCvInput(unsigned char index) { return cvInput_[index]; }
	const CvInput& getCvInput(unsigned char index) const { return cvInput_[index]; }
	Switch& getGateInput() { return gateInput_; }
	const Switch& getGateInput() const { return gateInput_; }
	Switch& getSwitch(unsigned char index) { return switch_[index]; }
	const Switch& getSwitch(unsigned char index) const { return switch_[index]; }
	SevenSegLed& getLed() { return led_; }
	const SevenSegLed& getLed() const { return led_; }
	void setMidiClksOutPerPulse(unsigned char newValue);
	unsigned char getMidiChannel(){return midiChannel_;}
	bool getJackDetect(unsigned char index){return jackDetect_[index];}
	void setOutput(unsigned int newValue);
	unsigned int getOutput(){return output_;}
	void setOutCalib(unsigned char index, unsigned int newValue);
	unsigned int getOutCalib(unsigned char index){return outCalib_[index];}
	void writeOutCalib(unsigned int eepromAddress);
	void calcCvCalib(unsigned int eepromAddress);
	void readAllCalib();
	unsigned char readEepromByte(unsigned int address);
	unsigned int readEepromUInt(unsigned int address);
	void writeEepromByte(unsigned int address, unsigned char newValue);
	void writeEepromUInt(unsigned int address, unsigned int newValue);
	void pollCalibCvInputs();
	void pollDigiPins(unsigned char ticksPassed);
	void pollSwitches(unsigned char ticksPassed);
	unsigned char pollTickCount();
	unsigned char pollMidiCount();
	void refreshLeds();
	void refreshFlash(unsigned char ticksPassed);
	void beginMidi(unsigned int ubrr);
	void beginTick();
	void pollMidi();
protected:
private:
	void beginSpi();
	unsigned char writeSpi(unsigned char cData);
	void beginI2c();
	void writeI2cByte(unsigned char address, unsigned char reg, unsigned char value);
	void writeI2cWord(unsigned char address, unsigned char reg, unsigned int word);
	unsigned char readI2cByte(unsigned char address, unsigned char reg);
	unsigned int readI2cWord(unsigned char address, unsigned char reg);
	unsigned int readCvInput(unsigned char input);
	void writeCalibOutput(unsigned int newValue);
	ArpHardware() {}
	ArpHardware(ArpHardwareBase* base);
	~ArpHardware();
	ArpHardware( const ArpHardware &c );
	ArpHardware& operator=( const ArpHardware &c );

}; //ArpHardware

#endif //__ARPHARDWARE_H__
