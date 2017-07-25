/*
//AteDrum.h  Oscitron Drum main class
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
*/


#ifndef __ATEDRUM_H__
#define __ATEDRUM_H__

#include "AteDrumEngine.h"
#include "AteDrumEngineBase.h"
#include "AteOscHardware/AteOscHardware.h"
#include "AteOscHardware/AteOscHardwareBase.h"
#include "SsHelpers/SsHelpers.h"
#include "AteOscPitch.h"

class AteDrum : public AteOscHardwareBase, AteDrumEngineBase
{
//variables
public:
protected:
private:
	AteOscHardware& hardware_;
	AteDrumEngine& engine_;
	const unsigned char LED_FLASH_SHORT_TICKS = 4;
	const unsigned char LED_FLASH_LONG_TICKS = 8;
	const AteDrumEngine::Ctrl CV_INPUT_TO_CTRL[AteOscHardware::CV_INPUTS] = {AteDrumEngine::CTRL_DUMMY,AteDrumEngine::CTRL_PITCH,AteDrumEngine::CTRL_DUMMY, AteDrumEngine::CTRL_DELAY,AteDrumEngine::CTRL_DUMMY,AteDrumEngine::CTRL_VOLUME,AteDrumEngine::CTRL_LENGTH,AteDrumEngine::CTRL_DUMMY};
	const unsigned char MIDI_NOTE_ACTION_DEBOUNCE = 3;
	const unsigned char MIDI_NOTE_ACTION_RESET = 21;
	const unsigned int RUNNING_TIME_OUT_DEFAULT  = 2000; //( for 60bpm)
	unsigned int runningTimeOutTick_ = RUNNING_TIME_OUT_DEFAULT;  
	unsigned int runningTick_ = 0;
	unsigned char midiNoteAction_ = 0;
	unsigned char midiNoteDebounce_ = 0;
	unsigned char cvInputBuffer[AteOscHardware::CV_INPUTS] = {0};
//functions
public:
	AteDrum();
	~AteDrum();
	void initialize();
	void poll(unsigned char ticksPassed);
	void hardwareSwitchChanged(unsigned char sw, unsigned char newValue);
	void hardwareSwitchHeld(unsigned char sw);
	void hardwareCvInputChanged(unsigned char input, unsigned int newValue);
	void hardwareGateInputChanged(unsigned char input, bool newValue);
	void hardwareRotaryEncoderChanged(unsigned char rotary, unsigned char newValue, bool clockwise);
	void hardwareAudioBufferStatusChanged(unsigned char newStatus);
	void engineInstrChanged(unsigned char instr);
	void engineEditStepChanged(unsigned char step);
	void enginePlayStepChanged(unsigned char step);
	void enginePatternChanged(unsigned char step, bool newValue);
	void enginePatchChanged(unsigned char patchNum);
protected:
private:
	AteDrum( const AteDrum &c );
	AteDrum& operator=( const AteDrum &c );
	void readPatch(unsigned char patchNum);
	void stopReset();
	void stopFunctionFlash();
	void refreshLeds();
	void refreshRunning(unsigned char ticksPassed);
	void refreshMidiNoteAction();
	void toggleFuncLed();
}; //AteDrum

#endif //__ATEDRUM_H__
