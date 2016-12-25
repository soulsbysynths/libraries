//AteOscHardware.h  Atmegatron Euro Oscitron Hardware control.
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

#ifndef __ATEOSCHARDWARE_H__
#define __ATEOSCHARDWARE_H__

#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdlib.h>
#include <avr/eeprom.h>
#include <util/twi.h>
#include "AteOscHardwareBase.h"
#include "SsHelpers.h"
#include "LedRgb.h"
#include "LedCircular.h"
#include "CvInput.h"
#include "Switch.h"
#include "RotaryEncoder.h"

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

#ifndef HIGH
#define HIGH 0x1
#endif
#ifndef LOW
#define LOW  0x0
#endif


#define INPUTS 8
// #define CV_READ_ORDER_SIZE 12

#define F_SCL 100000UL // SCL frequency
#define I2C_BUFFER_SIZE 130
#define AUDIO_BUFFER_SIZE 256

#define FM24C64B_DEFAULT_ADDRESS        (0x50) /* 1010 + A2 + A1 + A0 = 0x50 default */

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

class AteOscHardware
{
	//variables
	public:
	static const unsigned char FUNCTION = 0;
	static const unsigned char VALUE = 1;
	static const unsigned int HOLD_EVENT_TICKS = 2000;
	static const unsigned char AUDIO_INPUT = 0;
	static const unsigned char LED_FLASH_SCALE = 16;
	static const unsigned int CV_HALF_SCALE = 2048;
	static const unsigned int CV_HALF_SCALE_MASK = CV_HALF_SCALE - 1;
	static const unsigned char CV_INPUTS = INPUTS;

	enum EepromAddresses : unsigned int
	{
		EEPROM_CURRENT_PATCH = 1012,
		EEPROM_CTRL_MODE = 1013,
		EEPROM_CLOCK_MODE = 1014,
		EEPROM_QUANT_KEY = 1015,
		EEPROM_PITCH_LOW = 1016, //1.25V
		EEPROM_PITCH_HIGH = 1018, //3.75V
		EEPROM_FILT_LOW = 1020,  //1.25V
		EEPROM_FILT_HIGH = 1022  //3.75V
	};
	enum AudioBufferStatus : unsigned char 
	{
		BUFFER_IDLE = 0,
		BUFFER_WAITZCROSS = 1,
		BUFFER_CAPTURING = 2,
		BUFFER_OVERFLOW = 3,
		BUFFER_CAPTURED = 4
	};
	enum InputMode : unsigned char
	{
		IP_CV = 0,
		IP_GATE = 1
	};
	enum CvInputName : unsigned char
	{
		CV_FILT = 0,
		CV_RES = 1,
		CV_PITCHPOT = 2,
		CV_FILTPOT = 3,
		CV_PITCH = 4,
		CV_PWM = 5,
		CV_FLANGE = 6,
		CV_CAPTURE = 7
	};
	enum CtrlMode : unsigned char
	{
		CT_NORMAL = 0,
		CT_PITCHATT,
		CT_FILTATT,
		CT_BOTH
	};
	protected:
	private:
	AteOscHardwareBase* base_;
	RotaryEncoder rotEncoder_[2];
	InputMode inputMode_[INPUTS] = {IP_CV};
	CvInput cvInput_[INPUTS];
	bool gateInput_[INPUTS] = {false};
	unsigned int cvCalib_[2][3] = {{0}};  //input (pitch, filt), low/high/high-low
	Switch switch_[2];
	LedCircular ledCircular_[2];
	LedRgb ledSwitch_[2];
	unsigned char ledFlashTickCnt = 0;
	CtrlMode ctrlMode_ = CT_NORMAL;
	//functions
	
	public:
	static AteOscHardware& getInstance()
	{
		static AteOscHardware instance; // Guaranteed to be destroyed.
		return instance;  // Instantiated on first use.
	}
	void construct(AteOscHardwareBase* base);
	RotaryEncoder& getRotEncoder(unsigned char index) { return rotEncoder_[index]; }
	const RotaryEncoder& getRotEncoder(unsigned char index) const { return rotEncoder_[index]; }
	CvInput& getCvInput(unsigned char index) { return cvInput_[index]; }
	const CvInput& getCvInput(unsigned char index) const { return cvInput_[index]; }
	bool getGateInput(){return gateInput_;}
	Switch& getSwitch(unsigned char index) { return switch_[index]; }
	const Switch& getSwitch(unsigned char index) const { return switch_[index]; }
	LedCircular& getLedCircular(unsigned char index) { return ledCircular_[index]; }
	const LedCircular& getLedCircular(unsigned char index) const { return ledCircular_[index]; }
	LedRgb& getLedSwitch(unsigned char index) { return ledSwitch_[index]; }
	const LedRgb& getLedSwitch(unsigned char index) const { return ledSwitch_[index]; }
	void setInputMode(unsigned char input, InputMode newMode) {inputMode_[input] = newMode; }
	InputMode getInputMode(unsigned char input){return inputMode_[input]; }
	void calcCvCalib(unsigned int eepromAddress);
	void readCvCalib(unsigned int eepromAddress);
	char getAudioBuffer(unsigned char sample);
	unsigned char getAudioBufferLength();
	unsigned char getAudioMinLength();
	void setAudioMinLength(unsigned char newLength);
	void setAudioBufferStatus(AudioBufferStatus newValue);
	AudioBufferStatus getAudioBufferStatus();
	unsigned char readEepromByte(unsigned int address);
	void writeEepromByte(unsigned int address, unsigned char newValue);
	CtrlMode getCtrlMode(){return ctrlMode_;}
	void setCtrlMode(CtrlMode newMode){ctrlMode_ = newMode;}
	void refreshLeds();
	void refreshFlash(unsigned char ticksPassed);
	void pollCvInputs(unsigned char ticksPassed);
	void pollMappedCvInput(CvInputName input);
	void pollGateInputs();
	void pollSwitches(unsigned char ticksPassed);
	void pollRotEncoders(unsigned char ticksPassed);
	void pollAudioBufferStatus();
	protected:
	private:
	void beginSpi();
	unsigned char writeSpi(unsigned char cData);
	unsigned int readMCP3208input(unsigned char input);
	void beginI2c();
	void writeI2cByte(unsigned char address, unsigned char reg, unsigned char value);
	void writeI2cWord(unsigned char address, unsigned char reg, unsigned int word);
	AteOscHardware() {}
	AteOscHardware(AteOscHardwareBase* base);
	~AteOscHardware();
	AteOscHardware( const AteOscHardware &c );
	AteOscHardware& operator=( const AteOscHardware &c );

}; //AteOscHardware

#endif //__ATEOSCHARDWARE_H__
