//AtmHardware.h  Atmegatron Hardware control.
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

#ifndef __ATM_HARDWARE_H__
#define __ATM_HARDWARE_H__

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <avr/eeprom.h>
#include "AtmHardwareBase.h"
#include "SsHelpers.h"
#include "LedRgb.h"
#include "LedCircular.h"
#include "Led.h"
#include "AnalogueControl.h"
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

//#define NULL 0

#define HIGH 0x1
#define LOW  0x0
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

#define MIDI_BUFFER_SIZE 64

class AtmHardware
{
	//variables
	public:
	static const unsigned char FUNCTION = 0;
	static const unsigned char VALUE = 1;
	static const unsigned char BANK = 2;
	static const unsigned int HOLD_EVENT_TICKS = 2000;
	static const unsigned char SYSEX_PRODUCT = 0;
	static const unsigned int MIDI_CHANNEL_ADDRESS = 1023;
	
	enum MidiError : unsigned char
	{
		MIDIERR_NONE,
		MIDIERR_FRAME,
		MIDIERR_OVERRUN,
		MIDIERR_PARITY,
		MIDIERR_BUFFERFULL
	};
	protected:
	private:
	AtmHardwareBase* base_;
	RotaryEncoder rotEncoder_[2];
	AnalogueControl anlControl_[6];
	Switch switch_[3];
	LedCircular ledCircular_[2];
	LedRgb ledSwitch_[3];
	Led ledMidi_;
	bool midiChannelSelectMode_ = false;
	static const unsigned char MIDI_THRU_ON = 0x80;
	unsigned char midiThru_ = 0;
	unsigned char midiChannel_ = 0;
	//functions
	
	public:
	static AtmHardware& getInstance()
	{
		static AtmHardware instance; // Guaranteed to be destroyed.
		return instance;  // Instantiated on first use.
	}
	void construct(AtmHardwareBase* base);
	RotaryEncoder& getRotEncoder(unsigned char index) { return rotEncoder_[index]; }
	const RotaryEncoder& getRotEncoder(unsigned char index) const { return rotEncoder_[index]; }	
	AnalogueControl& getAnlControl(unsigned char index) { return anlControl_[index]; }
	const AnalogueControl& getAnlControl(unsigned char index) const { return anlControl_[index]; }
	Switch& getSwitch(unsigned char index) { return switch_[index]; }
	const Switch& getSwitch(unsigned char index) const { return switch_[index]; } 
	LedCircular& getLedCircular(unsigned char index) { return ledCircular_[index]; }
	const LedCircular& getLedCircular(unsigned char index) const { return ledCircular_[index]; }
	LedRgb& getLedSwitch(unsigned char index) { return ledSwitch_[index]; }
	const LedRgb& getLedSwitch(unsigned char index) const { return ledSwitch_[index]; }
	Led& getLedMidi() { return ledMidi_; }
	const Led& getLedMidi() const { return ledMidi_; }
	bool getMidiChannelSelectMode() {return midiChannelSelectMode_;}
	unsigned char getMidiChannel(){return midiChannel_;}
	void refreshLeds();
	void refreshFlash(unsigned char ticksPassed);
	void pollAnlControls(unsigned char ticksPassed);
	void pollSwitches(unsigned char ticksPassed);
	void pollRotEncoders(unsigned char ticksPassed);
	void beginMidi(unsigned int ubrr);
	void pollMidi();
	void writeMidi(unsigned char data);
	static void flushMidi();
	protected:
	private:
	void writeWord(unsigned char address, unsigned char reg, unsigned int word);   // Typically only used internally, but allows the user to write any register pair if needed, so it's public
	void writeByte(unsigned char address, unsigned char reg, unsigned char value);        // Typically only used internally, but allows the user to write any register if needed, so it's public
	void beginSpi();
	void writeSpi(unsigned char cData);
	void writeMidiSettings();
	AtmHardware() {}
	AtmHardware(AtmHardwareBase* base);
	~AtmHardware();
	AtmHardware( const AtmHardware &c );
	AtmHardware& operator=( const AtmHardware &c );

}; //AtmHardware

#endif //__ATM_HARDWARE_H__
