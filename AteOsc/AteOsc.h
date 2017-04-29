/* 
* AteOsc.h
*
* Created: 08/01/2016 15:43:50
* Author: Paul Soulsby
*/


#ifndef __ATEOSC_H__
#define __ATEOSC_H__

#include "AteOscEngine/AteOscEngineBase.h"
#include "AteOscEngine/AteOscEngine.h"
#include "AteOscHardware.h"
#include "AteOscHardwareBase.h"
#include "SsHelpers.h"
//#include "Arduino.h"

class AteOsc : public AteOscHardwareBase, AteOscEngineBase
{
	//variables
	public:
	protected:
	private:
	enum ClockMode : unsigned char
	{
		CM_CAPTURE = 0,
		CM_WAVE_INC,
		CM_WAVELEN_INC,
		CM_PORT_INC,
		CM_FILT_INC,
		CM_BITCRUSH_INC,
		CM_PATCH_INC,
		CM_WAVE_CV,
		CM_RES_CV,
		CM_PITCHCOARSE_CV,
		CM_PORT_CV,
		CM_FILT_CV,
		CM_BITCRUSH_CV,
		CM_QUANTKEY_CV,
		CM_PITCHATT_CV,
		CM_FILTATT_CV
	};
	AteOscHardware& hardware_;
	AteOscEngine& engine_;
	const unsigned char IS_SHIFT_HOLD = 0x8E;//0x8E;  //0xA6 proto
	const unsigned char LED_FLASH_TICKS = 4;
	const AteOscEngine::Ctrl CV_INPUT_TO_CTRL[AteOscHardware::CV_INPUTS] = {AteOscEngine::CTRL_SAMPLE,AteOscEngine::CTRL_Q,AteOscEngine::CTRL_PITCHFINE, AteOscEngine::CTRL_FILTOFF,AteOscEngine::CTRL_SAMPLE,AteOscEngine::CTRL_PWM,AteOscEngine::CTRL_FX,AteOscEngine::CTRL_SAMPLE};
	const AteOscEngine::Func CLOCK_MODE_TO_FUNC[6] = {AteOscEngine::FUNC_WAVE, AteOscEngine::FUNC_WAVELEN, AteOscEngine::FUNC_PITCHCOARSE, AteOscEngine::FUNC_PORTA, AteOscEngine::FUNC_FILT, AteOscEngine::FUNC_BITCRUSH};
	unsigned char testNote_ = 60;
	bool valueSecondaryMode_ = false;
	bool heldFuncAndValue_ = false;
	ClockMode clockMode_ = CM_CAPTURE;
	//functions
	public:
	AteOsc();
	~AteOsc();
	void initialize();
	void poll(unsigned char ticksPassed);
	void hardwareSwitchChanged(unsigned char sw, unsigned char newValue);
	void hardwareSwitchHeld(unsigned char sw);
	void hardwareCvInputChanged(unsigned char input, unsigned int newValue);
	void hardwareGateInputChanged(unsigned char input, bool newValue);
	void hardwareRotaryEncoderChanged(unsigned char rotary, unsigned char newValue, bool clockwise);
	void hardwareAudioBufferStatusChanged(unsigned char newStatus);
	void engineFunctionChanged(unsigned char func, unsigned char val);
	void engineOptionChanged(unsigned char func, bool opt);
	void engineMinLengthChanged(unsigned char newLength);
	void engineDoEvents();
	void engineStartCapture(bool way);
	protected:
	private:
	AteOsc( const AteOsc &c );
	AteOsc& operator=( const AteOsc &c );
	void pollPitch(unsigned char ticksPassed);
	void toggleOption();
	void setClockMode(ClockMode newMode);
}; //AteOsc

#endif //__ATEOSC_H__
