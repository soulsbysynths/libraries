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

class AteOsc : public AteOscHardwareBase, AteOscEngineBase
{
	//variables
	public:
	protected:
	private:
	AteOscHardware& hardware_;
	AteOscEngine& engine_;
	const unsigned char IS_FUNC_FILL[2] = {0xFC,0x21};
	const unsigned char LED_FLASH_TICKS = 4;
	unsigned char testNote_ = 60;
	//functions
	public:
	AteOsc();
	~AteOsc();
	void initialize();
	void poll(unsigned char ticksPassed);
	void hardwareSwitchChanged(unsigned char sw, unsigned char newValue);
	void hardwareSwitchHeld(unsigned char sw);
	void hardwareCvInputChanged(unsigned char control, unsigned int newValue);
	void hardwareRotaryEncoderChanged(unsigned char rotary, unsigned char newValue, bool clockwise);
	void hardwareAudioBufferStatusChanged(unsigned char newStatus);
	void engineFunctionChanged(unsigned char func, unsigned char val);
	void engineOptionChanged(unsigned char func, bool opt);
	void engineMinLengthChanged(unsigned char newLength);
	protected:
	private:
	AteOscEngine::Ctrl cHardToEngFunc(AteOscHardware::CvInputName cvInput);
	AteOsc( const AteOsc &c );
	AteOsc& operator=( const AteOsc &c );
	bool isShiftHold(AteOscEngine::Func func);
}; //AteOsc

#endif //__ATEOSC_H__
