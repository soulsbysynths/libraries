/*
* AteOscHardwareCalibrate.cpp
*
* Created: 13/03/2017 19:13:12
* Author: paulsoulsby
*/

#include "AteOscHardwareCalibrate.h"

extern void writeFram(const void* data, unsigned int startAddr, size_t size);
extern void readFram(void* data, unsigned int startAddr, size_t size);
extern void writeMemory(const void* data, void* startAddr, size_t size);
extern void readMemory(void* data, const void* startAddr, size_t size);

// default constructor
AteOscHardwareCalibrate::AteOscHardwareCalibrate() : hardware_(AteOscHardware::getInstance())
{
	hardware_.construct(this);
} //AteOscHardwareCalibrate

// default destructor
AteOscHardwareCalibrate::~AteOscHardwareCalibrate()
{
} //~AteOscHardwareCalibrate

bool AteOscHardwareCalibrate::initEeprom()
{
	unsigned int addr = 0;
	unsigned char eepromBuffer[PATCH_SIZE] = {0};
	unsigned char i,j,k;

	//init eeprom

	addr = 0;
	for(i=0;i<PATCHES;++i)
	{
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
				return false;
			}
		}
		addr += PATCH_SIZE;
	}

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

	return true;
}

bool AteOscHardwareCalibrate::initFram()
{
	unsigned int addr = 0;
	unsigned char framBuffer[OSC_WAVELEN] = {0};
	unsigned char i,j,k;

	//init fram
	for(i=0;i<OSC_BANKS;++i)
	{
		for (j=0;j<OSC_TABLES;++j)
		{
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
					return false;
				}
			}
			addr += OSC_WAVELEN;
		}
	}
	return true;
}

void AteOscHardwareCalibrate::initTest(char test)
{
	switch(test)
	{
		case 'a':
		for(unsigned char i=0;i<8;++i)
		{
			hardware_.getLedCircular(AteOscHardware::FUNCTION).fill(i);
			hardware_.refreshLeds();
			delay(250);
		}
		break;
	}
}

void AteOscHardwareCalibrate::hardwareAudioBufferStatusChanged(unsigned char newStatus)
{

}

void AteOscHardwareCalibrate::hardwareCvInputChanged(unsigned char control, unsigned int newValue)
{

}

void AteOscHardwareCalibrate::hardwareGateInputChanged(unsigned char input, bool newValue)
{

}

void AteOscHardwareCalibrate::hardwareRotaryEncoderChanged(unsigned char rotary, unsigned char newValue, bool clockwise)
{

}

void AteOscHardwareCalibrate::hardwareSwitchChanged(unsigned char sw, unsigned char newValue)
{

}

void AteOscHardwareCalibrate::hardwareSwitchHeld(unsigned char sw)
{

}