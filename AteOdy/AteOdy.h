/* 
* AteOdy.h
*
* Created: 27/09/2016 14:42:01
* Author: paulsoulsby
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
	AteOscHardware& hardware_;
	AteOdyEngine& engine_;
	const unsigned char LED_FLASH_TICKS = 4;
	const AteOdyEngine::Ctrl CV_INPUT_TO_CTRL[AteOscHardware::CV_INPUTS] = {AteOdyEngine::CTRL_NULL,AteOdyEngine::CTRL_MODA,AteOdyEngine::CTRL_VCO1, AteOdyEngine::CTRL_VCO2,AteOdyEngine::CTRL_NULL,AteOdyEngine::CTRL_Q,AteOdyEngine::CTRL_MODB,AteOdyEngine::CTRL_NULL};
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
}; //AteOdy

#endif //__ATEODY_H__
