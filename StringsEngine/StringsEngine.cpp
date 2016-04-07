//StringsEngine.cpp  Audio engine for Strings
//Copyright (C) 2016  Paul Soulsby info@soulsbysynths.com
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

#include "StringsEngine.h"

extern void writeMemory(const void* data, void* startAddr, size_t size);
extern void readMemory(void* data, const void* startAddr, size_t size);
#define BASE_OCR1 758
//Many thanks to Jan Ostman for this faster multiplier and NCO code:  https://janostman.wordpress.com/
#define M(MX, MX1, MX2) \
asm volatile ( \
"clr r26 \n\t" \
"mulsu %B1, %A2 \n\t" \
"movw %A0, r0 \n\t" \
"mul %A1, %A2 \n\t" \
"add %A0, r1 \n\t" \
"adc %B0, r26 \n\t" \
"clr r1 \n\t" \
: \
"=&r" (MX) \
: \
"a" (MX1), \
"a" (MX2) \
: \
"r26" \
)

volatile unsigned int freq[OSCS][2]={0};
volatile unsigned int lfoPhaserVal;
volatile unsigned char phaserMix;
volatile char volume[OSCS] = {0};
volatile unsigned char wave[OSCS] = {0};
volatile bool gateMode = false;
volatile unsigned char integratorInc[OSCS] = {0};
volatile char ocr1aOffset = 0;

ISR(TIMER1_COMPA_vect) {
	static unsigned char integrators[OSCS][2] = {0};
	static unsigned int dcoPh[OSCS][2] = {0};
	static unsigned char writepointer = 0;
	static char delayline[256] = {0};
	unsigned int last;
	int ph;

	//only way to make this fast enough is to manually unroll it.  failed to get VMicro to take unroll compile setting
	last = dcoPh[0][0];
	dcoPh[0][0] += freq[0][0];
	if(dcoPh[0][0]<last)
	{
		integrators[0][0] = 128;
	}
	if(integrators[0][0])
	{
		integrators[0][0] -= integratorInc[0];
	}
	
	last = dcoPh[0][1];
	dcoPh[0][1] += freq[0][1];
	if(dcoPh[0][1]<last)
	{
		integrators[0][1] = 128;
	}
	if(integrators[0][1])
	{
		integrators[0][1] -= integratorInc[0];
	}
	
	last = dcoPh[1][0];
	dcoPh[1][0] += freq[1][0];
	if(dcoPh[1][0]<last)
	{
		integrators[1][0] = 128;
	}
	if(integrators[1][0])
	{
		integrators[1][0] -= integratorInc[1];
	}
	
	last = dcoPh[1][1];
	dcoPh[1][1] += freq[1][1];
	if(dcoPh[1][1]<last)
	{
		integrators[1][1] = 128;
	}
	if(integrators[1][1])
	{
		integrators[1][1] -= integratorInc[1];
	}
	
	last = dcoPh[2][0];
	dcoPh[2][0] += freq[2][0];
	if(dcoPh[2][0]<last)
	{
		integrators[2][0] = 128;
	}
	if(integrators[2][0])
	{
		integrators[2][0] -= integratorInc[2];
	}
	
	last = dcoPh[2][1];
	dcoPh[2][1] += freq[2][1];
	if(dcoPh[2][1]<last)
	{
		integrators[2][1] = 128;
	}
	if(integrators[2][1])
	{
		integrators[2][1] -= integratorInc[2];
	}
	
	last = dcoPh[3][0];
	dcoPh[3][0] += freq[3][0];
	if(dcoPh[3][0]<last)
	{
		integrators[3][0] = 128;
	}
	if(integrators[3][0])
	{
		integrators[3][0] -= integratorInc[3];
	}
	
	last = dcoPh[3][1];
	dcoPh[3][1] += freq[3][1];
	if(dcoPh[3][1]<last)
	{
		integrators[3][1] = 128;
	}
	if(integrators[3][1])
	{
		integrators[3][1] -= integratorInc[3];
	}

	int sum = ((int)volume[0] * ((char)pgm_read_byte(&(WAVE_TABLE[wave[0]][integrators[0][0]])) + (char)pgm_read_byte(&(WAVE_TABLE[wave[0]][integrators[0][1]])))) +
	((int)volume[1] * ((char)pgm_read_byte(&(WAVE_TABLE[wave[1]][integrators[1][0]])) + (char)pgm_read_byte(&(WAVE_TABLE[wave[1]][integrators[1][1]])))) +
	((int)volume[2] * ((char)pgm_read_byte(&(WAVE_TABLE[wave[2]][integrators[2][0]])) + (char)pgm_read_byte(&(WAVE_TABLE[wave[2]][integrators[2][1]])))) +
	((int)volume[3] * ((char)pgm_read_byte(&(WAVE_TABLE[wave[3]][integrators[3][0]])) + (char)pgm_read_byte(&(WAVE_TABLE[wave[3]][integrators[3][1]]))));

	writepointer++;
	delayline[writepointer] = (char)(sum>>7);
	M(ph,(int)delayline[(writepointer-lfoPhaserVal)&255],phaserMix);
	ph += delayline[writepointer];
	if(ph>127)
	{
		OCR2B = 255;
	}
	else if(ph<-128)
	{
		OCR2B = 0;
	}
	else
	{
		OCR2B = (unsigned char)(ph+128);
	}

	OCR1A = BASE_OCR1 + ocr1aOffset;
}

// default constructor
StringsEngine::StringsEngine(StringsEngineBase* base)
{
	construct(base);
} //AtmEngine

// default destructor
StringsEngine::~StringsEngine()
{
	if(patch_!=NULL)
	{
		delete patch_;
	}
	if(midi_!=NULL)
	{
		delete midi_;
	}
	if(arpeggiator_!=NULL)
	{
		delete arpeggiator_;
	}
} //~AtmEngine
void StringsEngine::construct(StringsEngineBase* base)
{
	base_ = base;
	patch_ = new AtmPatch(this);
	midi_ = new Midi(this,SYSEX_PROD_ID);
	masterClock_.setTicksPerCycle(BPM_TICKS);
	arpeggiator_ = new Arpeggiator(this,midi_->getNoteOnPtr());
}

void StringsEngine::initialize()
{
	unsigned char i,j;
	
	// Set up Timer 1 to send a sample every interrupt.
	cli();
	// Set CTC mode
	// Have to set OCR1A *after*, otherwise it gets reset to 0!
	TCCR1B = (TCCR1B & ~_BV(WGM13)) | _BV(WGM12);
	TCCR1A = TCCR1A & ~(_BV(WGM11) | _BV(WGM10));
	// No prescaler
	TCCR1B = (TCCR1B & ~(_BV(CS12) | _BV(CS11))) | _BV(CS10);
	// Set the compare register (OCR1A).
	// OCR1A is a 16-bit register, so we have to do this with
	// interrupts disabled to be safe.
	OCR1A = BASE_OCR1;//F_CPU / SAMPLE_RATE;
	// Enable interrupt when TCNT1 == OCR1A
	TIMSK1 |= _BV(OCIE1A);
	sei();
	// Set up Timer 2 for PWM output
	//use internal clock
	bitClear(ASSR,EXCLK);    //disable external clock
	bitClear(ASSR,AS2);      //clock timers from internal clock

	//Fast PWM modAmt_e
	bitSet(TCCR2A,WGM20);  //TOV flag set on max
	bitSet(TCCR2A,WGM21);  //Update OCRx at bottom
	bitClear(TCCR2B,WGM22);  //Top = 0xFF
	
	//Set non-inverting PWM on OC2B pin
	bitClear(TCCR2A,COM2B0);
	bitSet(TCCR2A,COM2B1);
	//set normal operation on OC2A pin
	bitClear(TCCR2A,COM2A0);
	bitClear(TCCR2A,COM2A1);
	
	//no prescaler
	bitSet(TCCR2B,CS20);
	bitClear(TCCR2B,CS21);
	bitClear(TCCR2B,CS22);
	OCR2B = 128;

	//load vanilla
	for(i=0;i<4;++i)
	{
		patch_->setFunctionValue(i,4);
	}
	patch_->setFunctionValue(FUNC_CYCLER,0);
	patch_->setFunctionValue(FUNC_ATTACK,6);
	patch_->setFunctionValue(FUNC_DECAY,4);
	patch_->setFunctionValue(FUNC_SUSTAIN,13);
	patch_->setFunctionValue(FUNC_RELEASE,8);
	patch_->setFunctionValue(FUNC_PITCHLFOWAVE,0);
	patch_->setFunctionValue(FUNC_PITCHLFOSPEED,8);
	patch_->setFunctionValue(FUNC_ARPPATTERN,0);
	patch_->setFunctionValue(FUNC_ARPSPEED,8);
	patch_->setFunctionValue(FUNC_PHASELFOWAVE,0);
	patch_->setFunctionValue(FUNC_PHASELFOSPEED,5);

	patch_->setOptionValue(FUNC_WAVEA,true);
	patch_->setOptionValue(FUNC_CYCLER,false);
	patch_->setOptionValue(FUNC_ATTACK,false);
	patch_->setOptionValue(FUNC_PITCHLFOWAVE,false);
	patch_->setOptionValue(FUNC_ARPPATTERN,false);
	patch_->setOptionValue(FUNC_PHASELFOWAVE,false);

	patch_->setCtrlValue(LOW,CTRL_DETUNE,128);
	patch_->setCtrlValue(LOW,CTRL_PITCHLFODEPTH,32);
	patch_->setCtrlValue(LOW,CTRL_PITCHENVDEPTH,0);
	patch_->setCtrlValue(LOW,CTRL_PHASESPEED,64);
	patch_->setCtrlValue(LOW,CTRL_PHASERDEPTH,128);
	patch_->setCtrlValue(LOW,CTRL_PHASERSOURCE,0);

}
void StringsEngine::poll(unsigned char ticksPassed)
{
	static unsigned char lfoCnt = 0;
	static unsigned char lfoTicks = 0;;
	for(unsigned char i=0;i<OSCS;++i)
	{
		env_[i].refresh(ticksPassed);
		if(gateMode==true)
		{
			if(oscNotes_[i]>0)
			{
				volume[i] = 127;
			}
			else
			{
				volume[i] = 0;
			}
		}
		else
		{
			volume[i] = env_[i].getExpOutput();
		}
	}
	if(midi_->getClockRunning()==true)
	{
		masterClock_.refresh(midi_->getClockTicksPassed());
	}
	else
	{
		masterClock_.refresh(ticksPassed);
	}
	arpeggiator_->refresh(masterClock_.getOutput());
	lfoPitch_.refresh(masterClock_.getOutput());
	lfoPhase_.refresh(masterClock_.getOutput());
	unsigned char lastCycleIndex = lfoCycle_.getIndex();
	lfoCycle_.refresh(masterClock_.getOutput());
	if(patch_->getOptionValue(FUNC_CYCLER)==true && lfoCycle_.getIndex()<lastCycleIndex)
	{
		cycleWaves();
	}
	lfoTicks += ticksPassed;
	if(lfoTicks>lfoSpeed_)
	{
		lfoCnt++;
		lfoTicks -= lfoSpeed_;
	}

	unsigned long f = 10540;  //equiv of 758
	if(patch_->getCtrlValue(0,CTRL_PITCHLFODEPTH)>0)
	{
		f = f * ((unsigned int)shapeExponential(lfoPitch_.getOutput(),pitchLfoAmt_,MULT_PITCH) + 1) / MULT_PITCH;  //>> BS_PITCH;
	}
	if(patch_->getCtrlValue(0,CTRL_PITCHENVDEPTH)>0)
	{
		f = f * ((unsigned int)shapeExponential(env_[curOsc_].getOutput(),pitchEnvAmt_,MULT_PITCH) + 1) / MULT_PITCH;  //>> BS_PITCH;
	}
	if(pitchBend_!=0)
	{
		f = f * ((unsigned int)shapeExponential(pitchBend_,MAX_PITCH,MULT_PITCH) + 1) / MULT_PITCH;
	}
	int newOff = (F_CPU / (2 * f)) - 1 - BASE_OCR1;
	ocr1aOffset = constrainChar(newOff);
	
	int p = (((int)lfoPhase_.getOutput()) * ((int)phaserSource_+1)) >> 8;
	p += ((int)pgm_read_byte(&(SINE_TABLE[lfoCnt])) * (256-phaserSource_)) >> 8;
	p += 127;
	lfoPhaserVal = constrainUChar(p);

}
unsigned char StringsEngine::calcIntegratorInc(unsigned char note)
{
	if(note>84)
	{
		return 4;
	}
	else if(note>60)
	{
		return 2;
	}
	else
	{
		return 1;
	}
}
void StringsEngine::tieOptions(Func minFunc, Func maxFunc, bool newOpt)
{
	for(unsigned char i=minFunc;i<=maxFunc;++i)
	{
		if(patch_->getOptionValue(i)!=newOpt)
		{
			patch_->setOptionValue(i,newOpt);
		}
	}
}
void StringsEngine::tieValues(Func minFunc, Func maxFunc, unsigned char newVal)
{
	for(unsigned char i=minFunc;i<=maxFunc;++i)
	{
		if(patch_->getFunctionValue(i)!=newVal)
		{
			patch_->setFunctionValue(i,newVal);
		}
	}
}
void StringsEngine::setMono(bool newValue)
{
	unsigned char i;
	mono_ = newValue;
	for(i=0;i<4;++i)
	{
		oscNotes_[i] = 0;
		oscNoteOrder_[i]= 0;
		freqBase_[i] = 0;
	}
	if(newValue==true)
	{
		refreshFreq(0);
	}
	else
	{
		for(i=0;i<OSCS;++i)
		{
			refreshFreq(i);
		}
	}
}
void StringsEngine::cycleWaves()
{
	unsigned char tmpWave = wave[0];
	for(unsigned char i=0;i<OSCS-1;++i)
	{
		patch_->setFunctionValue(i,wave[i+1]);
	}
	patch_->setFunctionValue(3,tmpWave);
}
void StringsEngine::setFunction(StringsEngine::Func newFunc)
{
	function_ = newFunc;
	base_->engineFunctionChanged((unsigned char)function_,patch_->getFunctionValue(function_));
	base_->engineOptionChanged((unsigned char)function_,patch_->getOptionValue(function_));
}
void StringsEngine::refreshFreq(unsigned char index)
{
	unsigned int f = freqBase_[index];
	unsigned int off = 50;
	if(mono_==true)
	{
		freq[0][0] = f;
		freq[0][1] = f;
		for(unsigned char i=1;i<OSCS;++i)
		{
			freq[i][0] = f+((f/off)*detuneAmt_>>7);
			freq[i][1] = f-((f/off)*detuneAmt_>>7);
			off -= 10;
		}
	}
	else
	{
		freq[index][0]=f-((f/off)*detuneAmt_>>7);
		freq[index][1]=f+((f/off)*detuneAmt_>>7);
	}
	
}
void StringsEngine::triggerNote(unsigned char note)
{
	unsigned char i;
	if(mono_==true)
	{
		for(i=0;i<OSCS;++i)
		{
			oscNotes_[i] = note;
			oscNoteOrder_[i] = 0;
			freqBase_[i] = pgm_read_word(&(FREQ_INC[note]));
			integratorInc[i]= calcIntegratorInc(note);
			env_[i].trigger();
		}
		refreshFreq(0);
	}
	else
	{
		//try and find an empty osc
		
		if(midi_->getTotNotesOn()<=OSCS || arpeggiator_->getType()>0)
		{
			curOsc_++;
			curOsc_ &= 0x03;
			for(i=0;i<OSCS;++i)
			{
				if(oscNoteOrder_[curOsc_]==0)
				{
					break;
				}
				else
				{
					curOsc_++;
					curOsc_ &= 0x03;
				}
			}
		}
		else
		{
			unsigned char fst = 128;
			for(i=0;i<OSCS;++i)
			{
				if(oscNoteOrder_[i]<fst && oscNoteOrder_[i]>0)
				{
					fst = oscNoteOrder_[i];
					curOsc_ = i;
				}
			}
		}

		//but still trigger even if you can't
		oscNotes_[curOsc_] = note;
		oscNoteOrder_[curOsc_] = midi_->getTotNotesOn();
		freqBase_[curOsc_] = pgm_read_word(&(FREQ_INC[note]));
		refreshFreq(curOsc_);
		integratorInc[curOsc_]= calcIntegratorInc(note);
		env_[curOsc_].trigger();
		//ignore mode
		//if(midi_->getTotNotesOn()<=OSCS)
		//{
		//for(i=0;i<OSCS;++i)
		//{
		//if(oscNotes_[curOsc_]==0)
		//{
		//break;
		//}
		//else
		//{
		//curOsc_++;
		//curOsc_ &= 0x03;
		//}
		//}
		//oscNotes_[curOsc_] = note;
		//freqBase_[curOsc_] = pgm_read_word(&(FREQ_INC[note]));
		//refreshFreq(curOsc_);
		//integratorInc[curOsc_]= calcIntegratorInc(note);
		//env_[curOsc_].trigger();
		//}
	}
}
void StringsEngine::releaseNote(unsigned char note)
{
	unsigned char i,j;
	if(mono_==true)
	{
		if(midi_->getTotNotesOn()<=0)
		{
			for(i=0;i<OSCS;++i)
			{
				oscNotes_[i] = 0;
				oscNoteOrder_[i] = 0;
				env_[i].release();
			}
		}
	}
	else
	{
		for (i=0; i<OSCS; ++i)
		{
			if (oscNotes_[i]==note)
			{
				oscNotes_[i] = 0;
				oscNoteOrder_[i] = 0;
				if(midi_->getTotNotesOn()<OSCS || arpeggiator_->getType()>0)
				{
					env_[i].release();
				}
				else
				{
					for(j=1;j<128;++j)
					{
						if(midi_->getNoteOn(j)==true && j!=oscNotes_[0] && j!=oscNotes_[1] && j!=oscNotes_[2] && j!=oscNotes_[3])
						{
							oscNotes_[i] = j;
							oscNoteOrder_[i] = 1;
							freqBase_[i] = pgm_read_word(&(FREQ_INC[oscNotes_[i]]));
							refreshFreq(i);
							integratorInc[i]= calcIntegratorInc(oscNotes_[i]);
							env_[i].setState(Envelope::SUSTAINING);
							break;
						}
					}
				}
				break;
			}
		}
	}
}
//***********************MIDI events********************************************
void StringsEngine::midiNoteOnReceived(unsigned char note, unsigned char velocity)
{
	if(arpeggiator_->getType()==0)
	{
		triggerNote(note);
	}
	else
	{
		arpeggiator_->buildNoteOrder();
	}
	if (arpeggiator_->getType()>0 && totNotesOnLast_==0 && midi_->getTotNotesOn()==1 && midi_->getClockRunning()==false)
	{
		masterClock_.reset();
		arpeggiator_->reset();
	}
	totNotesOnLast_ = midi_->getTotNotesOn();
}

void StringsEngine::midiNoteOffReceived(unsigned char note)
{
	releaseNote(note);
	if(arpeggiator_->getType()==0)
	{
		
	}
	else
	{
		arpeggiator_->buildNoteOrder();
	}
	totNotesOnLast_ = midi_->getTotNotesOn();
}

void StringsEngine::midiControlChangeReceived(unsigned char cc, unsigned char val)
{
	switch ((Midi::MidiCC) cc)
	{
		case Midi::CC_MODWHEEL:
		patch_->setCtrlValue(LOW,CTRL_PITCHLFODEPTH,val<<1);
		break;
		case Midi::CC_DETUNE:
		patch_->setCtrlValue(LOW,CTRL_DETUNE,val<<1);
		break;
		case Midi::CC_PHASER:
		patch_->setCtrlValue(LOW,CTRL_PHASERDEPTH,val<<1);
		break;
		case Midi::CC_GP1:
		patch_->setFunctionValue(FUNC_WAVEA,val>>3);
		break;
		case Midi::CC_GP2:
		patch_->setFunctionValue(FUNC_WAVEB,val>>3);
		break;
		case Midi::CC_GP3:
		patch_->setFunctionValue(FUNC_WAVEC,val>>3);
		break;
		case Midi::CC_GP4:
		patch_->setFunctionValue(FUNC_WAVED,val>>3);
		break;
		case Midi::CC_ATTACK:
		patch_->setFunctionValue(FUNC_ATTACK,val>>3);
		break;
		case Midi::CC_DECAY:
		patch_->setFunctionValue(FUNC_DECAY,val>>3);
		break;
		case Midi::CC_SUSTAIN:
		patch_->setFunctionValue(FUNC_SUSTAIN,val>>3);
		break;
		case Midi::CC_RELEASE:
		patch_->setFunctionValue(FUNC_RELEASE,val>>3);
		break;
		case 79:
		patch_->setFunctionValue(FUNC_PITCHLFOSPEED,val>>3);
		break;
		case Midi::CC_GP5:
		patch_->setFunctionValue(FUNC_CYCLER,val>>3);
		break;
		case Midi::CC_GP6:
		patch_->setFunctionValue(FUNC_PHASELFOSPEED,val>>3);
		break;
		case Midi::CC_GP7:
		patch_->setCtrlValue(LOW,CTRL_PHASESPEED,val<<1);
		break;
		case Midi::CC_ALLNOTESOFF:
		midi_->reset();
		break;
	}
}

void StringsEngine::midiPitchBendReceived(char bend)
{
	pitchBend_ = bend;
}

//****************************************patch events********************************************
void StringsEngine::patchValueChanged(unsigned char func, unsigned char newValue)
{
	unsigned char i;
	switch (func)
	{
		case FUNC_WAVEA:
		case FUNC_WAVEB:
		case FUNC_WAVEC:
		case FUNC_WAVED:
		if(wave[func]!=newValue)
		{
			wave[func] = newValue;
			if(patch_->getOptionValue(func)==true)
			{
				tieValues(FUNC_WAVEA,FUNC_WAVED,newValue);
			}
		}
		break;
		case FUNC_ATTACK:
		for(i=0;i<OSCS;++i)
		{
			env_[i].setAttack(pgm_read_word(&(ENV_ADR_INC[newValue])));
		}
		break;
		case FUNC_DECAY:
		for(i=0;i<OSCS;++i)
		{
			env_[i].setDecay(pgm_read_word(&(ENV_ADR_INC[newValue])));
		}
		break;
		case FUNC_SUSTAIN:
		for(i=0;i<OSCS;++i)
		{
			env_[i].setSustain(pgm_read_word(&(ENV_S_LEVEL[newValue])));
		}
		break;
		case FUNC_RELEASE:
		for(i=0;i<OSCS;++i)
		{
			env_[i].setRelease(pgm_read_word(&(ENV_ADR_INC[newValue])));
		}
		break;
		case FUNC_PITCHLFOWAVE:
		lfoPitch_.setTable(newValue);
		break;
		case FUNC_PITCHLFOSPEED:
		lfoPitch_.setDivision(newValue);
		break;
		case FUNC_PHASELFOWAVE:
		lfoPhase_.setTable(newValue);
		break;
		case FUNC_PHASELFOSPEED:
		lfoPhase_.setDivision(newValue);
		break;
		case FUNC_ARPPATTERN:
		arpeggiator_->setType(newValue);
		break;
		case FUNC_ARPSPEED:
		arpeggiator_->setDivision(newValue);
		break;
		case FUNC_CYCLER:
		lfoCycle_.setDivision(newValue);
		break;
	}
	if(func==function_)
	{
		base_->engineFunctionChanged(func,newValue);
	}
}

void StringsEngine::patchOptionChanged(unsigned char func, bool newOpt)
{
	unsigned char i;
	switch (func)
	{
		case FUNC_WAVEA:
		case FUNC_WAVEB:
		case FUNC_WAVEC:
		case FUNC_WAVED:
		if(newOpt==true)
		{
			tieValues(FUNC_WAVEA,FUNC_WAVED,patch_->getFunctionValue(FUNC_WAVEA));
		}
		tieOptions(FUNC_WAVEA,FUNC_WAVED,newOpt);
		break;
		case FUNC_ATTACK:
		case FUNC_DECAY:
		case FUNC_SUSTAIN:
		case FUNC_RELEASE:
		gateMode = newOpt;
		tieOptions(FUNC_ATTACK,FUNC_RELEASE,newOpt);
		break;
		case FUNC_PITCHLFOWAVE:
		case FUNC_PITCHLFOSPEED:
		lfoPitch_.setInvert(newOpt);
		tieOptions(FUNC_PITCHLFOWAVE,FUNC_PITCHLFOSPEED,newOpt);
		break;
		case FUNC_PHASELFOWAVE:
		case FUNC_PHASELFOSPEED:
		lfoPhase_.setInvert(newOpt);
		tieOptions(FUNC_PHASELFOWAVE,FUNC_PHASELFOSPEED,newOpt);
		break;
	}
	if(func==function_)
	{
		base_->engineOptionChanged(func,newOpt);
	}
}

void StringsEngine::patchCtrlChanged(unsigned char bank, unsigned char ctrl, unsigned char newValue)
{
	unsigned char nD,i;
	if(bank!=LOW) return;
	switch (ctrl)
	{
		case CTRL_DETUNE:
		nD = newValue >> 1;
		if(nD!=detuneAmt_)
		{
			detuneAmt_ = nD;
			if(mono_==true)
			{
				refreshFreq(0);
			}
			else
			{
				for (i=0; i<OSCS; i++)
				{
					refreshFreq(i);
				}
			}

		}
		break;
		case CTRL_PITCHLFODEPTH:
		pitchLfoAmt_ = ((float)newValue * MAX_PITCH) / 255;
		break;
		case CTRL_PITCHENVDEPTH:
		pitchEnvAmt_ = ((float)newValue * MAX_PITCH) / 255;
		break;
		case CTRL_PHASESPEED:
		lfoSpeed_ = newValue >> 1;
		break;
		case CTRL_PHASERDEPTH:
		phaserMix = newValue;
		break;
		case CTRL_PHASERSOURCE:
		phaserSource_ = newValue;
		break;
	}
}
//*********************Arpeggiator events***************************
void StringsEngine::arpeggiatorNoteEvent(unsigned char lastNote, unsigned char newNote)
{
	if(lastNote>0)
	{
		releaseNote(lastNote);
	}
	if(newNote>0)
	{
		triggerNote(newNote);
	}
}