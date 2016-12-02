/* 
* JamHardware.h
*
* Created: 18/06/2016 22:17:27
* Author: paulsoulsby
*/


#ifndef __JAMHARDWARE_H__
#define __JAMHARDWARE_H__

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include "JamHardwareBase.h"
#include "SsHelpers.h"
#include "LedRgb.h"
#include "AnalogueControl.h"
#include "Switch.h"
#include "U8glib.h"
#include "CircularBuffer.h"

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

#ifndef NULL
#define NULL 0x00
#endif

#ifndef HIGH
#define HIGH 0x01
#endif
#ifndef LOW
#define LOW 0x00
#endif

#define SWITCHES 6
#define POTS 2

class JamHardware
{
//variables
public:
	enum SwitchOp : unsigned char
	{
		SW_UP = 0,
		SW_RIGHT,
		SW_DOWN,
		SW_LEFT,
		SW_A,
		SW_B
	};
	enum PotOp : unsigned char
	{
		POT_X = 0,
		POT_Y
	};
	static const unsigned int HOLD_EVENT_TICKS = 2000;
	static const unsigned char LED_FLASH_SCALE = 16;
protected:
private:
	JamHardwareBase* base_;
	AnalogueControl anlControl_[POTS];
	Switch switch_[SWITCHES];
	LedRgb led_;
	U8GLIB_NHD_C12832* lcd_;
	unsigned char ledFlashTickCnt = 0;
	
//functions
public:
	static JamHardware& getInstance()
	{
		static JamHardware instance; // Guaranteed to be destroyed.
		return instance;  // Instantiated on first use.
	}
	void construct(JamHardwareBase* base);
	AnalogueControl& getAnlControl(unsigned char index) { return anlControl_[index]; }
	const AnalogueControl& getAnlControl(unsigned char index) const { return anlControl_[index]; }
	Switch& getSwitch(unsigned char index) { return switch_[index]; }
	const Switch& getSwitch(unsigned char index) const { return switch_[index]; }
	LedRgb& getLed() { return led_; }
	const LedRgb& getLed() const { return led_; }
	U8GLIB_NHD_C12832* getLcdPtr() { return lcd_; }
	const U8GLIB_NHD_C12832* getLcdPtr() const { return lcd_; }
	bool isAudioBufferFull();
	void writeAudioBuffer(unsigned char data);
	void refreshLEDs();
	void refreshFlash(unsigned char ticksPassed);
	void pollAnlControls(unsigned char ticksPassed);
	void pollSwitches(unsigned char ticksPassed);
	void initializeAudio();
protected:
private:
	JamHardware() {}
	JamHardware(JamHardwareBase* base);
	~JamHardware();
	JamHardware( const JamHardware &c );
	JamHardware& operator=( const JamHardware &c );

}; //JamHardware

#endif //__JAMHARDWARE_H__
