/*
//AteOdy.h  Odytron for Oscitron main class
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


#ifndef __ATEODY_H__
#define __ATEODY_H__

#include "AteOdyEngineBase.h"
#include "AteOscHardwareBase.h"
#include "AteOdyEngine.h"
#include "AteOscHardware.h"
//#include "OdyOscillatorProgmem.h"

class AteOdy : public AteOscHardwareBase, AteOdyEngineBase
{
//variables
public:
protected:
private:
	enum ClockMode : unsigned char
	{
		CM_OSC0FM_TOG = 0,
		CM_OSC1FM_TOG,
		CM_OSC1PWM_TOG,
		CM_OSCLEVELFX_TOG,
		CM_OSCLEVEL0_TOG,
		CM_OSCLEVEL1_TOG,
		CM_FILTFM_TOG,
		CM_PATCH_INC,
		CM_OSC0FM_CV,
		CM_OSC1FM_CV,
		CM_OSC1PWM_CV,
		CM_OSCLEVELFX_CV,
		CM_OSCLEVEL0_CV,
		CM_OSCLEVEL1_CV,
		CM_FILTFM_CV,
		CM_QUANT_CV
	};
	AteOscHardware& hardware_;
	AteOdyEngine& engine_;
	const unsigned char LED_FLASH_TICKS = 4;
	const AteOdyEngine::Ctrl CV_INPUT_TO_CTRL[AteOscHardware::CV_INPUTS] = {AteOdyEngine::CTRL_NULL,AteOdyEngine::CTRL_MODA,AteOdyEngine::CTRL_VCO1, AteOdyEngine::CTRL_VCO2,AteOdyEngine::CTRL_NULL,AteOdyEngine::CTRL_Q,AteOdyEngine::CTRL_MODB,AteOdyEngine::CTRL_NULL};
	bool valueSecondaryMode_ = false;
	ClockMode clockMode_ = CM_OSC0FM_TOG;
//functions
public:
	AteOdy();
	~AteOdy();
	void initialize();
	void poll();
	void hardwareSwitchChanged(unsigned char sw, unsigned char newValue);
	void hardwareSwitchHeld(unsigned char sw);
	void hardwareCvInputChanged(unsigned char input, unsigned int newValue);
	void hardwareGateInputChanged(unsigned char input, bool newValue);
	void hardwareRotaryEncoderChanged(unsigned char rotary, unsigned char newValue, bool clockwise);
	void hardwareAudioBufferStatusChanged(unsigned char newStatus);
	void engineFunctionChanged(unsigned char func, unsigned char val);
	void engineOptionChanged(unsigned char func, bool opt);
protected:
private:
	AteOdy( const AteOdy &c );
	AteOdy& operator=( const AteOdy &c );
	void toggleOption(AteOdyEngine::Func func);
	void setClockMode(ClockMode newMode);
}; //AteOdy

#endif //__ATEODY_H__
