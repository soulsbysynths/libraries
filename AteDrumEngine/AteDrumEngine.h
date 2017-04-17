/* 
* AteDrumEngine.h
*
* Created: 05/04/2017 15:28:57
* Author: paulsoulsby
*/


#ifndef __ATEDRUMENGINE_H__
#define __ATEDRUMENGINE_H__

#include "AtmAudio/AtmAudio.h"
#include "AteDrumPatch.h"
#include "AteDrumPatchBase.h"
#include "AteDrumEngineProgmem.h"
#include "AteDrumEngineBase.h"
#include "DelayLine.h"
#include "AteOscPitch.h"

class AteDrumEngine : public AteDrumPatchBase
{
//variables
public:
	static AteDrumEngine& getInstance()
	{
		static AteDrumEngine instance; // Guaranteed to be destroyed.
		// Instantiated on first use.
		return instance;
	}
	enum Ctrl : unsigned char
	{
		CTRL_SAMPLEFREQ = 0,
		CTRL_VOLUME,
		CTRL_DELAY,
		CTRL_PITCH,
		CTRL_LENGTH,
		CTRL_SAMPLE  //this is a dummy to fill CV_INPUT_TO_CTRL
	};
	static const unsigned int BASE_SF = 11025;
protected:
private:
	static const unsigned char CTRLS = 6;
	static const unsigned int RUNNING_TIMEOUT = 2000;
	AteDrumPatch* patch_;
	unsigned char instr_ = 0;
	unsigned char step_ = 0;
	unsigned char bufferPatchRead_ = 255;
	AteDrumEngineBase* base_ = NULL;
	unsigned char length_[AteDrumPatch::INSTRUMENTS] = {0};
	DelayLine* delayLine_[AteDrumPatch::INSTRUMENTS];
	unsigned char volume_[AteDrumPatch::INSTRUMENTS] = {0};
	unsigned long sampleFreq_ = 0;
	unsigned int linearFreq_[2] = {0};
	unsigned int runningTick_ = 0;
//functions
public:
	void construct(AteDrumEngineBase* base);
	const AteDrumPatch* getPatchPtr() const { return  patch_; }
	AteDrumPatch* getPatchPtr() { return patch_; }
	void initialize();
	void pollRunning(unsigned char ticksPassed);
	void setInstr(unsigned char newValue);
	unsigned char getInstr(){return instr_;}
	void setStep(unsigned char newValue);
	unsigned char getStep(){return step_;}
	void setPlayStep(unsigned char newValue);
	unsigned char getPlayStep();
	void resetPlayStep();
	void setLinearFreq(unsigned char source, unsigned int newValue);
	void setBufferPatchRead(unsigned char newValue);
	void patchPatternChanged(unsigned char instr, unsigned char step, bool newValue);
	void patchCtrlChanged(unsigned char instr, unsigned char ctrl, unsigned char newValue);
	bool getRunning();
protected:
private:
	AteDrumEngine(AteDrumEngineBase* base);
	AteDrumEngine() {}
	AteDrumEngine( const AteDrumEngine &c );
	~AteDrumEngine();
	AteDrumEngine& operator=( const AteDrumEngine &c );
	void setSampleFreq(unsigned long newSf);
	void calcEndAddress(unsigned char instr, unsigned char len, unsigned char inc);
}; //AteDrumEngine

#endif //__ATEDRUMENGINE_H__
