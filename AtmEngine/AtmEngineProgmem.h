//AtmEngineProgmem.h  Progmem for Audio engine for Atmegatron
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

#ifndef ATMENGINEPROGMEM_H_
#define ATMENGINEPROGMEM_H_

#include <avr/pgmspace.h>

static const unsigned int ENV_ADR_INC[16] PROGMEM = {32767,4095,1424,682,372,217,133,86,57,38,26,19,13,9,7,5};
static const int ENV_S_LEVEL[16] PROGMEM = {0,2048,4352,6400,8704,10752,13056,15104,17408,19456,21760,23808,26112,28160,30464,32767};                              

static const unsigned int PORTA_SPEED[16] PROGMEM = {0,5,14,27,46,77,123,195,306,477,740,1145,1768,2728,4207,6484};

//static const unsigned int MIDI_FREQS[128] PROGMEM =
//{
	//8,9,9,10,10,11,12,12,13,14,15,15,16,17,18,19,21,22,23,24,26,28,29,31,33,35,37,39,41,44,46,49,52,55,58,62,65,69,73,78,82,87,92,98,104,110,117,123,131,139,147,156,165,175,185,
	//196,208,220,233,247,262,277,294,311,330,349,370,392,415,440,466,494,523,554,587,622,659,698,740,784,831,880,932,988,1047,1109,1175,1245,1319,1397,1480,1568,1661,1760,1865,1976,2093,2217,2349,2489,2637,2794,
	//2960,3136,3322,3520,3729,3951,4186,4435,4699,4978,5274,5588,5920,6272,6645,7040,7459,7902,8372,8870,9397,9956,10548,11175,11840,12544
//};

static const unsigned int MIDI_FREQS[128] PROGMEM =  //all scaled by x4 to get better resolution
{
	33,35,37,39,41,44,46,49,52,55,58,62,65,69,73,78,82,87,92,98,104,110,117,123,131,139,147,156,165,175,185,196,208,220,233,247,262,277,294,311,330,349,370,392,415,440,466,494,523,554,587,622,659,698,740,784,831,880,932,988,1047,1109,1175,1245,1319,1397,1480,1568,1661,1760,1865,1976,2093,2217,2349,2489,2637,2794,2960,3136,3322,3520,3729,3951,4186,4435,4699,4978,5274,5588,5920,6272,6645,7040,7459,7902,8372,8870,9397,9956,10548,11175,11840,12544,13290,14080,14917,15804,16744,17740,18795,19912,21096,22351,23680,25088,26580,28160,29834,31609,33488,35479,37589,39824,42192,44701,47359,50175
};

#endif /* ATMENGINEPROGMEM_H_ */