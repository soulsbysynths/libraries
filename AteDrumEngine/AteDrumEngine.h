/*
//AteDrumEngine.h  Oscidrum audio engine 
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
		CTRL_VOLUME = 0,
		CTRL_DELAY,
		CTRL_PITCH,
		CTRL_LENGTH,
		CTRL_DUMMY  //this is a dummy to fill CV_INPUT_TO_CTRL
	};
	static const unsigned int BASE_SF = 11025;
	static const unsigned int BASE_SF_LINEAR = 3377;
	static const unsigned char CLEAR_PATCH_READ = 0xFF;
protected:
private:
	static const unsigned char CTRLS = 6;
	AteDrumPatch* patch_;
	unsigned char instr_ = 0;
	unsigned char editStep_ = 0;
	unsigned char playStep_ = 0; 
	unsigned char bufferPatchRead_ = CLEAR_PATCH_READ;
	AteDrumEngineBase* base_ = NULL;
	unsigned char length_[AteDrumPatch::INSTRUMENTS] = {0};
	DelayLine* delayLine_[AteDrumPatch::INSTRUMENTS];
	unsigned char volume_[AteDrumPatch::INSTRUMENTS] = {0};
	unsigned long sampleFreq_ = 0;
	unsigned int linearFreq_[2] = {0};
	bool running_ = false;
//functions
public:
	void construct(AteDrumEngineBase* base);
	const AteDrumPatch* getPatchPtr() const { return  patch_; }
	AteDrumPatch* getPatchPtr() { return patch_; }
	void initialize();
	void setInstr(unsigned char newValue);
	unsigned char getInstr(){return instr_;}
	void setEditStep(unsigned char newValue);
	unsigned char getEditStep(){return editStep_;}
	void setPlayStep(unsigned char newValue);
	unsigned char getPlayStep(){return playStep_;}
	void setRunning(bool newValue){running_ = newValue;}
	bool getRunning(){return running_;}
	void playStep();
	void playInstr(unsigned char instr, unsigned char vol);
	void setLinearFreq(unsigned char source, unsigned int newValue);
	void setBufferPatchRead(unsigned char newValue);
	void patchPatternChanged(unsigned char instr, unsigned char step, bool newValue);
	void patchCtrlChanged(unsigned char instr, unsigned char ctrl, unsigned char newValue);
protected:
private:
	AteDrumEngine(AteDrumEngineBase* base);
	AteDrumEngine() {}
	AteDrumEngine( const AteDrumEngine &c );
	~AteDrumEngine();
	AteDrumEngine& operator=( const AteDrumEngine &c );
	void setSampleFreq(unsigned long newSf);
	void calcEndAddress(unsigned char instr, unsigned char len);
}; //AteDrumEngine

#endif //__ATEDRUMENGINE_H__
