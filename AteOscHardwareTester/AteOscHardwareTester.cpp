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

void AteOscHardwareTester::init()
{
	wavetable_ = Wavetable(WAVE_LEN);
	audio_->initialize();
	refreshSineWave();
	//Serial.begin(9600);
}
void AteOscHardwareTester::refreshAudioTest()
{
	//hardware_.getLedCircular(AteOscHardware::VALUE).select(hardware_.getAudioCurrent()>>4);
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
	if(hardware_.getAudioBufferStatus()!=AteOscHardware::BUFFER_CAPTURING && hardware_.getAudioBufferStatus()!=AteOscHardware::BUFFER_WAITZCROSS)
	{
		hardware_.setAudioBufferStatus(AteOscHardware::BUFFER_WAITZCROSS);
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
	
	hardware_.refreshFlash(ticksPassed);
	hardware_.refreshLeds();
	if(test_==TEST_AUDIO)
	{
		refreshAudioTest();
	}
}

void AteOscHardwareTester::writeCvCalib(unsigned char cvIp, unsigned int addr)
{
	const unsigned char CV_READS = 8;
	unsigned char calibVal[2] = {0};
	unsigned long calibRead = 0;
	for(unsigned char i=0;i<CV_READS;++i)
	{
		hardware_.pollCvInputs(0);
		calibRead += hardware_.getCvInput(cvIp).getValue();
	}
	calibRead = calibRead / CV_READS;
	calibVal[0] = (unsigned char)((calibRead >> 8) & 0xFF);  //msb
	calibVal[1] = (unsigned char)(calibRead & 0xFF);  //lsb
	writeMemory((const void*)calibVal,(void*)addr,sizeof(calibVal));
}

//***************** Hardware Events *******************************************
void AteOscHardwareTester::hardwareCvInputChanged(unsigned char control, unsigned int newValue)
{
	if(test_==TEST_POTS)
	{
		hardware_.getLedCircular(1).fill(newValue>>8);
	}
	if(control==2)  //2 = pitch knob
	{
		audio_->setSampleFreq((unsigned long)newValue*WAVE_LEN);
	}
	
}

void AteOscHardwareTester::hardwareSwitchChanged(unsigned char sw, unsigned char newValue)
{
	static const unsigned char MEMTEST_SIZE = 128;
	static const unsigned int MEMTEST_ADDR = 1024;
	unsigned char i;
	unsigned char writetest[MEMTEST_SIZE] = {0};
	unsigned char readtest[MEMTEST_SIZE] = {0};
	static unsigned char lastLed = 255;
	unsigned char curLed = 0;
	if(newValue==HIGH)
	{
		if(sw==0)
		{
			hardware_.getLedSwitch(sw).setColour(LedRgb::GREEN);
			if(test_!=TEST_POTS)
			{
				test_ = TEST_POTS;
				for(i=0;i<2;++i)
				{
					hardware_.getLedCircular(i).setState(0);
					hardware_.getLedSwitch(i).setColour(LedRgb::OFF);
				}
				refreshSineWave();
				//Serial.println("Writing");
				for (i=0;i<MEMTEST_SIZE;++i)
				{
					writetest[i]  = MEMTEST_SIZE-i;
					curLed = ((unsigned int)i<<3)/MEMTEST_SIZE;
					if(curLed!=lastLed)
					{
						lastLed = curLed;
						hardware_.getLedCircular(AteOscHardware::FUNCTION).setSegment(curLed,true);
						hardware_.refreshLeds();
					}

				}
				writeFram((const void*)writetest,MEMTEST_ADDR,MEMTEST_SIZE);
				//Serial.println("Reading");
				readFram((void*)readtest,MEMTEST_ADDR,MEMTEST_SIZE);
				lastLed = 255;
				for (i=0;i<MEMTEST_SIZE;++i)
				{
					//Serial.println(readtest[i],DEC);
					if(readtest[i]==writetest[i])
					{
						curLed = ((unsigned int)i<<4)/MEMTEST_SIZE;
						if(curLed!=lastLed)
						{
							lastLed = curLed;
							hardware_.getLedCircular(AteOscHardware::VALUE).setSegment(curLed,true);
							hardware_.refreshLeds();
						}
					}
					else
					{
						hardware_.getLedCircular(AteOscHardware::VALUE).setSegment(curLed,false);
						hardware_.refreshLeds();
					}
				}
			}

			//Serial.println("Finished");
		}
		else
		{
			test_ = TEST_AUDIO;
			hardware_.setAudioBufferStatus(AteOscHardware::BUFFER_WAITZCROSS);
		}
		
	}
	else
	{
		if(test_==TEST_POTS)
		{
			hardware_.getLedSwitch(sw).setColour(LedRgb::RED);
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
			writeCvCalib(AteOscHardware::CV_PITCH,1016);
			audio_->setSampleFreq(78L*WAVE_LEN);
			break;
			case 1:
			writeCvCalib(AteOscHardware::CV_PITCH,1018);
			audio_->setSampleFreq(440L*WAVE_LEN);
			break;
			case 2:
			writeCvCalib(AteOscHardware::CV_FILT,1020);
			audio_->setSampleFreq(78L*WAVE_LEN);
			break;
			case 3:
			writeCvCalib(AteOscHardware::CV_FILT,1022);
			audio_->setSampleFreq(440L*WAVE_LEN);
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