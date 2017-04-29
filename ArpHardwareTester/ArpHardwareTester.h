/*
* ArpHardwareTester.h
*
* Created: 11/04/2017 22:45:03
* Author: paulsoulsby
*/


#ifndef __ARPHARDWARETESTER_H__
#define __ARPHARDWARETESTER_H__

#include "ArpHardware.h"
#include "ArpHardwareBase.h"
#include "Midi.h"
#include "Midi/MidiBase.h"
#ifndef HIGH
#define HIGH 0x1
#endif

class ArpHardwareTester : public ArpHardwareBase, MidiBase 
{
	//variables
	public:
	enum CalibMode : unsigned char
	{
		CB_ROOT = 0,
		CB_CHORD = 1,
		CB_OUTPUT = 2
	};
	protected:
	private:
	static const unsigned char MIDI_UBRR = 63; //(F_CPU / (MIDI_BAUD*8)) - 1
	static const unsigned char SYSEX_PROD_ID = 0;
	ArpHardware& hardware_;
	Midi* midi_;
	CalibMode calibMode_ = CB_OUTPUT;
	unsigned char midiNote_ = 0;
	//functions
	public:
	ArpHardwareTester();
	~ArpHardwareTester();
	void init();
	void initMemory();
	void poll(unsigned char ticksPassed);
	void hardwareSwitchChanged(unsigned char sw, unsigned char newValue);
	void hardwareSwitchHeld(unsigned char sw);
	void hardwareJackDetectChanged(unsigned char jack, bool newValue);
	void hardwareCvInputChanged(unsigned char input, unsigned int newValue);
	void hardwareGateInputChanged(bool newValue);
	void hardwareMidiChannelChanged(unsigned char channel);
	void hardwareMidiReceived(unsigned char data);
	void hardwareMidiError(unsigned char errorType);
	void midiControlChangeReceived(unsigned char cc, unsigned char val){}
	void midiNoteOnReceived(unsigned char note, unsigned char velocity);
	void midiNoteOffReceived(unsigned char note);
	void midiClockStartReceived(void){}
	void midiClockStopReceived(void){}
	void midiSysexStartReceived(void){}
	void midiSysexDataReceived(unsigned char index, unsigned char data){}
	void midiSysexStopReceived(void){}
	void midiSysexWrite(unsigned char data){}
	void midiPitchBendReceived(char bend){}
	protected:
	private:
	ArpHardwareTester( const ArpHardwareTester &c );
	ArpHardwareTester& operator=( const ArpHardwareTester &c );

}; //ArpHardwareTester

#endif //__ARPHARDWARETESTER_H__
