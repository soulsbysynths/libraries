//MulAtmegaHardware.h  Atmegatron Hardware control.
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

#ifndef __MUL_ATMEGA_HARDWARE_H__
#define __MUL_ATMEGA_HARDWARE_H__

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <avr/eeprom.h>
#include <Wire.h>
#include <avr/pgmspace.h>
#include <string.h>
#include <stdio.h>
#include "MulAtmegaHardwareBase.h"
#include "SsHelpers.h"
#include "LedRgb.h"
#include "LedCircular.h"
#include "Led.h"
#include "AnalogueControl.h"
#include "Switch.h"
#include "RotaryEncoder.h"
#include "U8glib.h"

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

//#define NULL 0

#ifndef HIGH
#define HIGH 0x01
#endif
#ifndef LOW
#define LOW 0x00
#endif
#define IODIRA    (0x00)      // MCP23x17 I/O Direction Register
#define IODIRB    (0x01)      // 1 = Input (default), 0 = Output

#define IPOLA     (0x02)      // MCP23x17 Input Polarity Register
#define IPOLB     (0x03)      // 0 = Normal (default)(low reads as 0), 1 = Inverted (low reads as 1)

#define GPINTENA  (0x04)      // MCP23x17 Interrupt on Change Pin Assignements
#define GPINTENB  (0x05)      // 0 = No Interrupt on Change (default), 1 = Interrupt on Change

#define DEFVALA   (0x06)      // MCP23x17 Default Compare Register for Interrupt on Change
#define DEFVALB   (0x07)      // Opposite of what is here will trigger an interrupt (default = 0)

#define INTCONA   (0x08)      // MCP23x17 Interrupt on Change Control Register
#define INTCONB   (0x09)      // 1 = pin is compared to DEFVAL, 0 = pin is compared to previous state (default)

#define IOCON     (0x0A)      // MCP23x17 Configuration Register
//                   (0x0B)      //     Also Configuration Register

#define GPPUA     (0x0C)      // MCP23x17 Weak Pull-Up Resistor Register
#define GPPUB     (0x0D)      // INPUT ONLY: 0 = No Internal 100k Pull-Up (default) 1 = Internal 100k Pull-Up

#define INTFA     (0x0E)      // MCP23x17 Interrupt Flag Register
#define INTFB     (0x0F)      // READ ONLY: 1 = This Pin triggered the Interrupt

#define INTCAPA   (0x10)      // MCP23x17 Interrupt Captured Value for Port Register
#define INTCAPB   (0x11)      // READ ONLY: State of the Pin at the Time the Interrupt Occurred

#define GPIOA     (0x12)      // MCP23x17 GPIO Port Register
#define GPIOB     (0x13)      // Value on the Port - Writing Sets Bits in the Output Latch

#define OLATA     (0x14)      // MCP23x17 Output Latch Register
#define OLATB     (0x15)      // 1 = Latch High, 0 = Latch Low (default) Reading Returns Latch State, Not Port Value!

// Control unsigned char and configuration register information - Control Byte: "0100 A2 A1 A0 R/W" -- W=0

#define    OPCODEW       (0b01000000)  // Opcode for MCP23S17 with LSB (bit0) set to write (0), address OR'd in later, bits 1-3
#define    OPCODER       (0b01000001)  // Opcode for MCP23S17 with LSB (bit0) set to read (1), address OR'd in later, bits 1-3
#define    ADDR_ENABLE   (0b00001000)  // Configuration register for MCP23S17, the only thing we change is enabling hardware addressing

#define MCP3208_SINGLE    (0b00000110)
#define MCP3208_DIFFERENTIAL    (0b00000100)
#define MCP3208_OFFSET_EEPROM_ADDR 1019

#define SPI_DEVICES 5
#define EXPANDERS 8
#define LED_RGBS 5
#define LEDS 2
#define ROTARY_ENCODERS 3
#define SWITCHES 12
#define ANL_CTRLS 8
#define VOICES 6
#define I2C_BUFFER_SIZE 6

#define MIDI_BUFFER_SIZE 64
#define MIDI_BUFFER_MASK 63

class MulAtmegaHardware
{
	//variables
	public:
	static const unsigned int HOLD_EVENT_TICKS = 2000;
	static const unsigned char SYSEX_PRODUCT = 0;
	static const unsigned int MIDI_CHANNEL_ADDRESS = 1023;
	static const unsigned char LED_FLASH_SCALE = 16;

	enum RotEncFunc : unsigned char
	{
		RE_FUNCTION,
		RE_VALUE,
		RE_GLOBAL
	};

	enum LedRgbFunc : unsigned char
	{
		RGB_FUNCTION,
		RGB_VALUE,
		RGB_GLOBAL,
		RGB_BANK,
		RGB_LCD
	};

	enum LedFunc : unsigned char
	{
		LED_OCTDOWN,
		LED_OCTUP
	};

	enum SwitchFunc : unsigned char
	{
		SW_FUNCTION,
		SW_VALUE,
		SW_GLOBAL,
		SW_BANK,
		SW_LEFT,
		SW_UP,
		SW_RIGHT,
		SW_DOWN,
		SW_A,
		SW_B,
		SW_OCTDOWN,
		SW_OCTUP
	};

	enum SpiDevice : unsigned char
	{
		SPI_ATTINY,
		SPI_ADC,
		SPI_EXP,
		SPI_DAC,
		SPI_LCD
	};

	enum ExanderAddress : unsigned char
	{
		EXP_ADDR_LED0,
		EXP_ADDR_LED1,
		EXP_ADDR_LED2,
		EXP_ADDR_SPINBANK,
		EXP_ADDR_ROTENC,
		EXP_ADDR_BUTTONS,
		EXP_ADDR_UNUSED,
		EXP_ADDR_VOICEEN
	};

	enum MidiError : unsigned char
	{
		MIDIERR_NONE,
		MIDIERR_FRAME,
		MIDIERR_OVERRUN,
		MIDIERR_PARITY,
		MIDIERR_BUFFERFULL
	};
	enum AudioChannel : unsigned char
	{
		AUDCH_LEFT,
		AUDCH_RIGHT
	};
	protected:
	private:
	struct I2CMessage {
		unsigned char id;
		unsigned char message[10];
		unsigned char messageLength;
	} ;
	I2CMessage i2cBuffer_[I2C_BUFFER_SIZE];
	unsigned char i2cMessages_ = 0;
	MulAtmegaHardwareBase* base_;
	U8GLIB_NHD_C12832* lcd_;
	RotaryEncoder rotEncoder_[ROTARY_ENCODERS];
	LedCircular ledCircular_[ROTARY_ENCODERS];
	Switch switch_[SWITCHES];
	LedRgb ledRgb_[LED_RGBS];
	Led led_[LEDS];
	unsigned char cd4066Control_[3] = {0};
	AnalogueControl anlControl_[ANL_CTRLS];
	static const unsigned char MIDI_THRU_ON = 0x80;
	unsigned char midiThru_ = 0;
	unsigned char midiChannel_ = 0;
	unsigned char ledFlashTickCnt_ = 0;
	unsigned char tickBuffer_ = 0;
	//functions
	public:
	static MulAtmegaHardware& getInstance()
	{
		static MulAtmegaHardware instance; // Guaranteed to be destroyed.
		return instance;  // Instantiated on first use.
	}
	void construct(MulAtmegaHardwareBase* base);
	RotaryEncoder& getRotEncoder(unsigned char index) { return rotEncoder_[index]; }
	const RotaryEncoder& getRotEncoder(unsigned char index) const { return rotEncoder_[index]; }
	AnalogueControl& getAnlControl(unsigned char index) { return anlControl_[index]; }
	const AnalogueControl& getAnlControl(unsigned char index) const { return anlControl_[index]; }
	Switch& getSwitch(SwitchFunc index) { return switch_[index]; }
	const Switch& getSwitch(SwitchFunc index) const { return switch_[index]; }
	LedCircular& getLedCircular(RotEncFunc index) { return ledCircular_[index]; }
	const LedCircular& getLedCircular(RotEncFunc index) const { return ledCircular_[index]; }
	LedRgb& getLedRgb(LedRgbFunc index) { return ledRgb_[index]; }
	const LedRgb& getLedRgb(LedRgbFunc index) const { return ledRgb_[index]; }
	Led& getLed(LedFunc index) { return led_[index]; }
	const Led& getLed(LedFunc index) const { return led_[index]; }
	U8GLIB_NHD_C12832* getLcdPtr() { return lcd_; }
	const U8GLIB_NHD_C12832* getLcdPtr() const { return lcd_; }
	unsigned char getMidiChannel(){return midiChannel_;}
	void setVoiceEnable(unsigned char voice, AudioChannel channel, bool way);
	bool getVoiceEnable(unsigned char voice, AudioChannel channel);
	void refreshExpanders();
	void refreshLcd();
	void demoLcd(unsigned char demo);
	void refreshFlash(unsigned char ticksPassed);
	void refreshTick(unsigned char ticksPassed);
	void pollAnlControls(unsigned char ticksPassed);
	void pollSwitches(unsigned char ticksPassed);
	void pollRotEncoders(unsigned char ticksPassed);
	void pollI2cBuffer(unsigned char ticksPassed);
	void beginMidi(unsigned int ubrr);
	void pollMidi();
	void txMidi(unsigned char data);
	bool queueI2cMessage(unsigned char id, unsigned char * data, size_t length);
	static void flushMidi();
	protected:
	private:
	void writeExpWord(unsigned char address, unsigned char reg, unsigned int word);
	void writeExpByte(unsigned char address, unsigned char reg, unsigned char value);
	unsigned int readExpWord(unsigned char address, unsigned char reg);
	unsigned char readExpByte(unsigned char address, unsigned char reg);
	unsigned int readAdcWord(unsigned char input);
	void beginSpi();
	unsigned char writeSpi(unsigned char cData);
	void writeMidiSettings();
	unsigned char getCD4066Index(unsigned char voice){return voice >> 1;}
	unsigned char getCD4066Bit(unsigned char voice, AudioChannel channel){return ((voice << 1) + (unsigned char)channel) & 0x03;}
	//void txSlave(unsigned char id, unsigned char * data, size_t length);
	MulAtmegaHardware() {}
	MulAtmegaHardware(MulAtmegaHardwareBase* base);
	~MulAtmegaHardware();
	MulAtmegaHardware( const MulAtmegaHardware &c );
	MulAtmegaHardware& operator=( const MulAtmegaHardware &c );

}; //MulAtmegaHardware

#endif //__MUL_ATMEGA_HARDWARE_H__
