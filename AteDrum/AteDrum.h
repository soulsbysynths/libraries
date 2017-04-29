/* 
* AteDrum.h
*
* Created: 05/04/2017 15:12:34
* Author: paulsoulsby
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
	const AteDrumEngine::Ctrl CV_INPUT_TO_CTRL[AteOscHardware::CV_INPUTS] = {AteDrumEngine::CTRL_SAMPLE,AteDrumEngine::CTRL_PITCH,AteDrumEngine::CTRL_SAMPLEFREQ, AteDrumEngine::CTRL_DELAY,AteDrumEngine::CTRL_SAMPLE,AteDrumEngine::CTRL_VOLUME,AteDrumEngine::CTRL_LENGTH,AteDrumEngine::CTRL_SAMPLE};
//functions
public:
	AteDrum();
	~AteDrum();
	void initialize();
	void poll(unsigned char ticksPassed);
	void refreshLeds(unsigned char instr, unsigned char step);
	void hardwareSwitchChanged(unsigned char sw, unsigned char newValue);
	void hardwareSwitchHeld(unsigned char sw);
	void hardwareCvInputChanged(unsigned char input, unsigned int newValue);
	void hardwareGateInputChanged(unsigned char input, bool newValue);
	void hardwareRotaryEncoderChanged(unsigned char rotary, unsigned char newValue, bool clockwise);
	void hardwareAudioBufferStatusChanged(unsigned char newStatus);
	void engineInstrChanged(unsigned char instr);
	void engineStepChanged(unsigned char step);
	void enginePatternChanged(unsigned char step, bool newValue);
	void enginePatchChanged(unsigned char patchNum);
protected:
private:
	AteDrum( const AteDrum &c );
	AteDrum& operator=( const AteDrum &c );
	void readPatch(unsigned char patchNum);
	void stopFunctionFlash();
}; //AteDrum

#endif //__ATEDRUM_H__
