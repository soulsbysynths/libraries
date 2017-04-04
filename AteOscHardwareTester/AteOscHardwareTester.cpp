//AteOscHardwareTester.cpp  Atmegatron Hardware Tester. Used by assembly factory.
//Copyright (C) 2015  Paul Soulsby info@soulsbysynths.com
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

#include "AteOscHardwareTester.h"

extern void writeFram(const void* data, unsigned int startAddr, size_t size);
extern void readFram(void* data, unsigned int startAddr, size_t size);
extern void writeMemory(const void* data, void* startAddr, size_t size);
extern void readMemory(void* data, const void* startAddr, size_t size);

// default constructor
AteOscHardwareTester::AteOscHardwareTester() : hardware_(AteOscHardware::getInstance())
{
	hardware_.construct(this);
	audio_ = new AtmAudio(WAVE_LEN);
} //AteOscHardwareTester

// default destructor
AteOscHardwareTester::~AteOscHardwareTester()
{
} //~AteOscHardwareTester

#ifdef _DEBUG==1

void AteOscHardwareTester::init()
{
	audio_->initialize();
	audio_->setSampleFreq((unsigned long)WAVE_LEN * 440);
	refreshSineWave();
	//Serial.begin(9600);
}
void AteOscHardwareTester::initMemory()
{
	
	unsigned int addr = 0;
	unsigned char framBuffer[OSC_WAVELEN] = {0};
	unsigned char eepromBuffer[PATCH_SIZE] = {0};
	unsigned char i,j,k;

	clearLeds();

	//init fram
	hardware_.refreshLeds();
	for(i=0;i<OSC_BANKS;++i)
	{
		hardware_.getLedCircular(AteOscHardware::FUNCTION).select(i);
		for (j=0;j<OSC_TABLES;++j)
		{
			hardware_.getLedCircular(AteOscHardware::VALUE).select(j);
			hardware_.refreshLeds();
			for(k=0;k<OSC_WAVELEN;++k)
			{
				framBuffer[k] = pgm_read_byte(&(OSC_WAVETABLE[i][j][k]));
			}
			writeFram((const void*)framBuffer, addr, sizeof(framBuffer));
			readFram((void*)framBuffer, addr, sizeof(framBuffer));
			for(k=0;k<OSC_WAVELEN;++k)
			{
				if(framBuffer[k]!=pgm_read_byte(&(OSC_WAVETABLE[i][j][k])))
				{
					hardware_.refreshLeds();
					for(;;){}  //STOP
				}
			}
			addr += OSC_WAVELEN;
		}
	}

	//init eeprom
	hardware_.refreshLeds();
	addr = 0;
	hardware_.getLedCircular(AteOscHardware::FUNCTION).select(0);
	for(i=0;i<PATCHES;++i)
	{
		hardware_.getLedCircular(AteOscHardware::VALUE).select(i);
		hardware_.refreshLeds();
		for(j=0;j<PATCH_SIZE;++j)
		{
			eepromBuffer[j] = pgm_read_byte(&(PATCH_DATA[i][j]));
		}
		writeMemory((const void*)eepromBuffer, (void*)addr, sizeof(eepromBuffer));
		readMemory((void*)eepromBuffer,(const void*)addr, sizeof(eepromBuffer));
		for(j=0;j<PATCH_SIZE;++j)
		{
			if(eepromBuffer[j]!=pgm_read_byte(&(PATCH_DATA[i][j])))
			{
				hardware_.refreshLeds();
				for(;;){}  //STOP
			}
		}
		addr += PATCH_SIZE;
	}

	hardware_.getLedCircular(AteOscHardware::FUNCTION).select(1);
	hardware_.refreshLeds();

	//global settings
	hardware_.writeEepromByte(AteOscHardware::EEPROM_CURRENT_PATCH, 0);
	hardware_.writeEepromByte(AteOscHardware::EEPROM_CTRL_MODE, 0);
	hardware_.writeEepromByte(AteOscHardware::EEPROM_CLOCK_MODE, 0);
	hardware_.writeEepromByte(AteOscHardware::EEPROM_QUANT_KEY, 0);
	hardware_.writeEepromByte(AteOscHardware::EEPROM_PITCH_LOW, 0x04);    //1024 1.25V 77.8Hz msb
	hardware_.writeEepromByte(AteOscHardware::EEPROM_PITCH_LOW+1, 0x00);  //1024 1.25V 77.8Hz lsb
	hardware_.writeEepromByte(AteOscHardware::EEPROM_PITCH_HIGH, 0x0C);   //3072 3.75V 440Hz msb
	hardware_.writeEepromByte(AteOscHardware::EEPROM_PITCH_HIGH+1, 0x00); //3072 3.75V 440Hz lsb
	hardware_.writeEepromByte(AteOscHardware::EEPROM_FILT_LOW, 0x04);    //1024 1.25V 77.8Hz msb
	hardware_.writeEepromByte(AteOscHardware::EEPROM_FILT_LOW+1, 0x00);  //1024 1.25V 77.8Hz lsb
	hardware_.writeEepromByte(AteOscHardware::EEPROM_FILT_HIGH, 0x0C);   //3072 3.75V 440Hz msb
	hardware_.writeEepromByte(AteOscHardware::EEPROM_FILT_HIGH+1, 0x00); //3072 3.75V 440Hz lsb

	hardware_.readCvCalib(AteOscHardware::EEPROM_PITCH_LOW);
	hardware_.readCvCalib(AteOscHardware::EEPROM_PITCH_HIGH);
	hardware_.readCvCalib(AteOscHardware::EEPROM_FILT_LOW);
	hardware_.readCvCalib(AteOscHardware::EEPROM_FILT_HIGH);

	clearLeds();

}
void AteOscHardwareTester::clearLeds()
{
	for(unsigned char i=0;i<2;++i)
	{
		hardware_.getLedCircular(i).setState(0);
	}
	hardware_.refreshLeds();
}
void AteOscHardwareTester::refreshSineWave()
{
	Wavetable wavetable_;
	wavetable_ = Wavetable(WAVE_LEN);
	for(unsigned char i=0;i<WAVE_LEN;++i)
	{
		wavetable_.setSample(i,sine_wave[i]);
	}
	audio_->pasteWavetable(wavetable_);
}

void AteOscHardwareTester::poll(unsigned char ticksPassed)
{
	static unsigned char waveIndex = 0;
	static unsigned char waveTick = 0;
	unsigned char waveLed = 0;

	if(allIpLedsZero() && audio_->getSampleFreq()!= TESTFREQ_AUDIO)
	{
		audio_->setSampleFreq(TESTFREQ_AUDIO);
		hardware_.setAudioBufferStatus(AteOscHardware::BUFFER_WAITZCROSS);
		clearLeds();
	}
	if(!allIpLedsZero() && audio_->getSampleFreq()!= WAVE_LEN)
	{
		audio_->setSampleFreq(WAVE_LEN);
		clearLeds();
	}

	hardware_.pollCvInputs(ticksPassed);
	hardware_.pollAudioBufferStatus();
	
	if (allIpLedsZero())
	{
		waveTick += ticksPassed;
		if(waveTick>31)
		{
			waveTick -= 32;
			waveLed = (hardware_.getAudioBuffer(waveIndex) + 128) >> 4;
			hardware_.getLedCircular(AteOscHardware::FUNCTION).select(((unsigned int)waveIndex<<3) / (hardware_.getAudioBufferLength()+1));
			hardware_.getLedCircular(AteOscHardware::VALUE).select(waveLed);
			waveIndex++;
			if(waveIndex>hardware_.getAudioBufferLength())
			{
				waveIndex = 0;
				hardware_.setAudioBufferStatus(AteOscHardware::BUFFER_WAITZCROSS);
			}
		}
	}
	else
	{
		hardware_.setAudioBufferStatus(AteOscHardware::BUFFER_WAITZCROSS);
	}


	hardware_.refreshFlash(ticksPassed);
	hardware_.refreshLeds();
}
bool AteOscHardwareTester::allIpLedsZero()
{
	for(unsigned char i=0;i<AteOscHardware::CV_INPUTS;++i)
	{
		if(ipLed_[i]>0)
		{
			return false;
		}
	}
	return true;
}
//***************** Hardware Events *******************************************
void AteOscHardwareTester::hardwareCvInputChanged(unsigned char control, unsigned int newValue)
{
	unsigned char newLed = newValue >> 8;
	if(newLed!=ipLed_[control])
	{
		ipLed_[control] = newLed;
		hardware_.getLedCircular(AteOscHardware::VALUE).fill(newLed);
		if(newLed>0)
		{
			hardware_.getLedCircular(AteOscHardware::FUNCTION).setSegment(control,true);
		}
		else
		{
			hardware_.getLedCircular(AteOscHardware::FUNCTION).setSegment(control,false);
		}
	}
}

void AteOscHardwareTester::hardwareSwitchChanged(unsigned char sw, unsigned char newValue)
{

}

void AteOscHardwareTester::hardwareSwitchHeld(unsigned char sw)
{
	
}
void AteOscHardwareTester::hardwareRotaryEncoderChanged(unsigned char rotary, unsigned char newValue, bool clockwise)
{

}
void AteOscHardwareTester::hardwareAudioBufferStatusChanged(unsigned char newStatus)
{


}

#else

//**** NORMAL HARDWARE TESTER**********************************************************************************************

void AteOscHardwareTester::init()
{
	wavetable_ = Wavetable(WAVE_LEN);
	audio_->initialize();
	refreshSineWave();
	//Serial.begin(9600);
}
void AteOscHardwareTester::initMemory()
{
	
	unsigned int addr = 0;
	unsigned char framBuffer[OSC_WAVELEN] = {0};
	unsigned char eepromBuffer[PATCH_SIZE] = {0};
	unsigned char i,j,k;

	for(i=0;i<2;++i)
	{
		hardware_.getLedCircular(i).setState(0);
		hardware_.getLedSwitch(i).setColour(LedRgb::OFF);
	}
	hardware_.refreshLeds();

	//init fram
	hardware_.getLedSwitch(AteOscHardware::FUNCTION).setColour(LedRgb::YELLOW);
	hardware_.refreshLeds();
	for(i=0;i<OSC_BANKS;++i)
	{
		hardware_.getLedCircular(AteOscHardware::FUNCTION).select(i);
		for (j=0;j<OSC_TABLES;++j)
		{
			hardware_.getLedCircular(AteOscHardware::VALUE).select(j);
			hardware_.refreshLeds();
			for(k=0;k<OSC_WAVELEN;++k)
			{
				framBuffer[k] = pgm_read_byte(&(OSC_WAVETABLE[i][j][k]));
			}
			writeFram((const void*)framBuffer, addr, sizeof(framBuffer));
			readFram((void*)framBuffer, addr, sizeof(framBuffer));
			for(k=0;k<OSC_WAVELEN;++k)
			{
				if(framBuffer[k]!=pgm_read_byte(&(OSC_WAVETABLE[i][j][k])))
				{
					hardware_.getLedSwitch(AteOscHardware::FUNCTION).setColour(LedRgb::RED);
					hardware_.refreshLeds();
					for(;;){}  //STOP
				}
			}
			addr += OSC_WAVELEN;
		}
	}

	hardware_.getLedSwitch(AteOscHardware::FUNCTION).setColour(LedRgb::GREEN);
	//init eeprom
	hardware_.getLedSwitch(AteOscHardware::VALUE).setColour(LedRgb::YELLOW);
	hardware_.refreshLeds();
	addr = 0;
	hardware_.getLedCircular(AteOscHardware::FUNCTION).select(0);
	for(i=0;i<PATCHES;++i)
	{
		hardware_.getLedCircular(AteOscHardware::VALUE).select(i);
		hardware_.refreshLeds();
		for(j=0;j<PATCH_SIZE;++j)
		{
			eepromBuffer[j] = pgm_read_byte(&(PATCH_DATA[i][j]));
		}
		writeMemory((const void*)eepromBuffer, (void*)addr, sizeof(eepromBuffer));
		readMemory((void*)eepromBuffer,(const void*)addr, sizeof(eepromBuffer));
		for(j=0;j<PATCH_SIZE;++j)
		{
			if(eepromBuffer[j]!=pgm_read_byte(&(PATCH_DATA[i][j])))
			{
				hardware_.getLedSwitch(AteOscHardware::FUNCTION).setColour(LedRgb::RED);
				hardware_.refreshLeds();
				for(;;){}  //STOP
			}
		}
		addr += PATCH_SIZE;
	}
	hardware_.getLedSwitch(AteOscHardware::VALUE).setColour(LedRgb::GREEN);
	hardware_.getLedCircular(AteOscHardware::FUNCTION).select(1);
	hardware_.refreshLeds();

	//global settings
	hardware_.writeEepromByte(AteOscHardware::EEPROM_CURRENT_PATCH, 0);
	hardware_.writeEepromByte(AteOscHardware::EEPROM_CTRL_MODE, 0);
	hardware_.writeEepromByte(AteOscHardware::EEPROM_CLOCK_MODE, 0);
	hardware_.writeEepromByte(AteOscHardware::EEPROM_QUANT_KEY, 0);
	//stick zeros in for all of this so don't think it's calibrated when it isn't
	hardware_.writeEepromByte(AteOscHardware::EEPROM_PITCH_LOW, 0x00);    //1024 1.25V 77.8Hz msb
	hardware_.writeEepromByte(AteOscHardware::EEPROM_PITCH_LOW+1, 0x00);  //1024 1.25V 77.8Hz lsb
	hardware_.writeEepromByte(AteOscHardware::EEPROM_PITCH_HIGH, 0x00);   //3072 3.75V 440Hz msb
	hardware_.writeEepromByte(AteOscHardware::EEPROM_PITCH_HIGH+1, 0x00); //3072 3.75V 440Hz lsb
	hardware_.writeEepromByte(AteOscHardware::EEPROM_FILT_LOW, 0x00);    //1024 1.25V 77.8Hz msb
	hardware_.writeEepromByte(AteOscHardware::EEPROM_FILT_LOW+1, 0x00);  //1024 1.25V 77.8Hz lsb
	hardware_.writeEepromByte(AteOscHardware::EEPROM_FILT_HIGH, 0x00);   //3072 3.75V 440Hz msb
	hardware_.writeEepromByte(AteOscHardware::EEPROM_FILT_HIGH+1, 0x00); //3072 3.75V 440Hz lsb
	//
	for(i=0;i<2;++i)
	{
		hardware_.getLedCircular(i).setState(0);
		hardware_.getLedSwitch(i).setColour(LedRgb::OFF);
	}
	hardware_.refreshLeds();

}

void AteOscHardwareTester::refreshAudioTest()
{
	//hardware_.getLedCircular(AteOscHardware::VALUE).select(hardware_.getAudioCurrent()>>4);  //no longer exists, would involve a volatile global variable
	if(hardware_.getAudioBufferStatus()==AteOscHardware::BUFFER_CAPTURED)
	{
		//see excel sheet for proof of this interpolation
		unsigned int pos = 0;
		unsigned int jump = (unsigned int)hardware_.getAudioBufferLength() << 1;  //0-255
		for(unsigned char i=0;i<WAVE_LEN;++i)
		{
			wavetable_.setSample(i,hardware_.getAudioBuffer(pos >> 8));
			pos += jump;
		}
		audio_->pasteWavetable(wavetable_);
		hardware_.setAudioBufferStatus(AteOscHardware::BUFFER_IDLE);
	}
}
void AteOscHardwareTester::refreshSineWave()
{
	for(unsigned char i=0;i<WAVE_LEN;++i)
	{
		wavetable_.setSample(i,saw_wave[i]);
	}
	audio_->pasteWavetable(wavetable_);
}
void AteOscHardwareTester::tick()
{
	unsigned char i;
	if(test_==TEST_TICK)
	{
		hardware_.getLedCircular(0).select(tick_%8);
		hardware_.getLedCircular(1).select(tick_);

		for(i=0;i<2;++i)
		{

			if(tick_%2==0)
			{
				hardware_.getLedSwitch(i).setColour(LedRgb::RED);
			}
			else
			{
				hardware_.getLedSwitch(i).setColour(LedRgb::GREEN);
			}

		}
		tick_++;
		if(tick_>15)
		{
			tick_ = 0;
		}
	}
}

void AteOscHardwareTester::poll(unsigned char ticksPassed)
{
	static unsigned char waveIndex = 0;
	static unsigned char waveTick = 0;
	unsigned char waveLed = 0;
	hardware_.pollSwitches(ticksPassed);
	hardware_.pollRotEncoders(ticksPassed);
	hardware_.pollCvInputs(ticksPassed);
	hardware_.pollAudioBufferStatus();
	
	if(test_==TEST_POTS)
	{
		for(unsigned char i=0;i<8;++i)
		{
			if(hardware_.getCvInput(i).getMoving()!=lastMoving_[i])
			{
				lastMoving_[i] = hardware_.getCvInput(i).getMoving();
				if(lastMoving_[i]==true)
				{
					hardware_.getLedCircular(0).setSegment(i,true);
				}
				else
				{
					hardware_.getLedCircular(0).setSegment(i,false);
				}
			}
			//Serial.print(hardware_.getCvInput(i).getValue(),DEC);
			//Serial.print("\t");
		}
		//Serial.println("");
	}
	if(test_==TEST_AUDIO)
	{
		refreshAudioTest();
		waveTick += ticksPassed;
		if(waveTick>7)
		{
			waveTick -= 8;
			waveLed = (hardware_.getAudioBuffer(waveIndex) + 128) >> 4;
			hardware_.getLedCircular(AteOscHardware::FUNCTION).select(waveIndex>>5);
			hardware_.getLedCircular(AteOscHardware::VALUE).select(waveLed);
			waveIndex++;
			if(waveIndex>hardware_.getAudioBufferLength())
			{
				waveIndex = 0;
			}
		}
	}
	audio_->setSampleFreq((unsigned long)pitch_.calcFrequency(pitch_.getOutput()) * (WAVE_LEN>>2));
	hardware_.refreshFlash(ticksPassed);
	hardware_.refreshLeds();
}

//***************** Hardware Events *******************************************
void AteOscHardwareTester::hardwareCvInputChanged(unsigned char control, unsigned int newValue)
{
	if(test_==TEST_POTS && !(control==AteOscHardware::CV_PITCH || control==AteOscHardware::CV_FILT))
	{
		hardware_.getLedCircular(AteOscHardware::VALUE).fill(newValue>>8);
	}
	if(control==AteOscHardware::CV_PITCHPOT)  //2 = pitch knob
	{
		pitch_.setCoarseOffset(newValue<<1);
	}
	if(control==AteOscHardware::CV_PITCH)
	{
		pitch_.setInput((newValue * 15)>>4);
	}
}

void AteOscHardwareTester::hardwareSwitchChanged(unsigned char sw, unsigned char newValue)
{
	unsigned char i;
	static unsigned char lastLed = 255;
	unsigned char curLed = 0;
	if(newValue==HIGH)
	{
		for(i=0;i<2;++i)
		{
			hardware_.getLedCircular(i).setState(0);
			hardware_.getLedSwitch(i).setColour(LedRgb::OFF);
		}
		hardware_.getLedSwitch(sw).setColour(LedRgb::YELLOW);
		if(sw==AteOscHardware::FUNCTION)
		{
			if(test_==TEST_TICK)
			{
				test_ = TEST_POTS;
				refreshSineWave();
			}
			else
			{
				test_ = TEST_TICK;
			}
		}
		else
		{
			test_ = TEST_AUDIO;
			hardware_.setAudioBufferStatus(AteOscHardware::BUFFER_WAITZCROSS);
		}
	}
}

void AteOscHardwareTester::hardwareSwitchHeld(unsigned char sw)
{
	if(test_==TEST_POTS)
	{
		hardware_.getLedSwitch(sw).setColour(LedRgb::YELLOW);
		switch(hardware_.getRotEncoder(AteOscHardware::FUNCTION).getValue())
		{
			case 0:
			//hardware_.setCvCalib(AteOscHardware::EEPROM_PITCH_LOW);
			break;
			case 1:
			//hardware_.setCvCalib(AteOscHardware::EEPROM_PITCH_HIGH);
			break;
			case 2:
			//hardware_.setCvCalib(AteOscHardware::EEPROM_FILT_LOW);
			break;
			case 3:
			//hardware_.setCvCalib(AteOscHardware::EEPROM_FILT_HIGH);
			break;
		}
	}
	
}
void AteOscHardwareTester::hardwareRotaryEncoderChanged(unsigned char rotary, unsigned char newValue, bool clockwise)
{
	if(test_==TEST_POTS)
	{
		hardware_.getLedCircular(rotary).select(newValue);
	}
}
void AteOscHardwareTester::hardwareAudioBufferStatusChanged(unsigned char newStatus)
{
	if(test_==TEST_AUDIO)
	{
		switch (newStatus)
		{
			case AteOscHardware::BUFFER_IDLE:
			hardware_.getLedSwitch(AteOscHardware::VALUE).setColour(LedRgb::OFF);
			break;
			case AteOscHardware::BUFFER_WAITZCROSS:
			hardware_.getLedSwitch(AteOscHardware::VALUE).setColour(LedRgb::YELLOW);
			break;
			case AteOscHardware::BUFFER_CAPTURING:
			hardware_.getLedSwitch(AteOscHardware::VALUE).setColour(LedRgb::RED);
			break;
			case AteOscHardware::BUFFER_OVERFLOW:
			hardware_.getLedSwitch(AteOscHardware::FUNCTION).setColour(LedRgb::RED);
			hardware_.getLedSwitch(AteOscHardware::VALUE).setColour(LedRgb::OFF);
			break;
			case AteOscHardware::BUFFER_CAPTURED:
			hardware_.getLedSwitch(AteOscHardware::FUNCTION).setColour(LedRgb::GREEN);
			hardware_.getLedSwitch(AteOscHardware::VALUE).setColour(LedRgb::OFF);
			break;
		}
	}

}
#endif
