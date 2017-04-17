/* 
* ArpAtarp.h
*
* Created: 12/04/2017 20:08:39
* Author: paulsoulsby
*/


#ifndef __ARPATARP_H__
#define __ARPATARP_H__

#include "ArpHardware.h"
#include "ArpHardwareBase.h"
#include "ArpAtarpEngine.h"
#include "ArpAtarpEngineBase.h"

#ifndef LOW
#define  LOW 0
#endif
#ifndef HIGH
#define  HIGH 1
#endif

class ArpAtarp : public ArpHardwareBase, ArpAtarpEngineBase
{
//variables
public:
protected:
private:
	static const unsigned char MIDI_UBRR = 63; //(F_CPU / (MIDI_BAUD*8)) - 1
	const char FUNC_DISPLAY[ArpAtarpEngine::FUNCS] = {'P','G','D','N','C','M','R','E'};
	const unsigned char FUNC_MASK[ArpAtarpEngine::FUNCS] = {31,1,15,1,1,15,15,2};
	static const char LED_FLASH_TICKS = 4;
	static const char LED_SHORT_FLASH_TICKS = 2;
	static const unsigned int EEPROM_PATCH_OFFSET = 256;
	ArpHardware& hardware_;
	ArpAtarpEngine& engine_;
	bool funcSelect_ = false;
	bool swHoldBlock_ = false;
	unsigned char gateTimeOut_ = 0;
//functions
public:
	ArpAtarp();
	~ArpAtarp();
	void init();
	void poll(unsigned char ticksPassed);
protected:
private:
	ArpAtarp( const ArpAtarp &c );
	ArpAtarp& operator=( const ArpAtarp &c );
	void refreshNoteSource();
	void hardwareSwitchChanged(unsigned char sw, unsigned char newValue);
	void hardwareSwitchHeld(unsigned char sw);
	void hardwareJackDetectChanged(unsigned char jack, bool newValue);
	void hardwareCvInputChanged(unsigned char input, unsigned int newValue);
	void hardwareGateInputChanged(bool newValue);
	void hardwareMidiChannelChanged(unsigned char channel);
	void hardwareMidiReceived(unsigned char data);
	void hardwareMidiError(unsigned char errorType);
	void engineOutputChanged(unsigned int newOutput);
	void engineFunctionChanged(unsigned char newFunction);
	void engineValueChanged(unsigned char newValue);
	void engineLoadUserPattern(unsigned char patternNum);
	void engineSaveUserPatternStep(unsigned char patternNum, unsigned char step, char value);
}; //ArpAtarp

#endif //__ARPATARP_H__
