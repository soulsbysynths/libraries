//OdyEngineProgmem.h  Odytron audio engine singleton class.  Odytron Progmem.  THIS IS WHERE MIDI FREQS NOW LIVE
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


#ifndef ODYENGINEPROGMEM_H_
#define ODYENGINEPROGMEM_H_

#include <avr/pgmspace.h>

static const unsigned int ENV_A_INC[16] PROGMEM = {6552,3649,2034,1134,634,355,200,113,65,39,24,15,11,8,7,6};
static const unsigned int ENV_D_INC[16] PROGMEM = {3276,1911,1115,651, 381,223,131,78, 47,28,18,12,8, 6,5,4};
static const unsigned int ENV_R_INC[16] PROGMEM = {2183,1308,784, 471, 283,170,103,62, 38,24,15,10,7, 5,4,3};

static const int ENV_S_LEVEL[16] PROGMEM = {0,2048,4352,6400,8704,10752,13056,15104,17408,19456,21760,23808,26112,28160,30464,32767};

static const unsigned int PORTA_SPEED[16] PROGMEM = {0,5,14,27,46,77,123,195,306,477,740,1145,1768,2728,4207,6484};

static const unsigned char PITCH_OFFSET[256] PROGMEM = {64,64,65,65,65,66,66,66,67,67,68,68,68,69,69,69,70,70,71,71,71,72,72,72,73,73,74,74,74,75,75,76,76,77,77,77,78,78,79,79,79,80,80,81,81,82,82,83,83,83,84,84,85,85,86,86,87,87,88,88,89,89,90,90,91,91,91,92,92,93,93,94,95,95,96,96,97,97,98,98,99,99,100,100,101,101,102,103,103,104,104,105,105,106,106,107,108,108,109,109,110,111,111,112,112,113,114,114,115,115,116,117,117,118,119,119,120,121,121,122,123,123,124,125,125,126,127,127,128,129,129,130,131,132,132,133,134,134,135,136,137,137,138,139,140,140,141,142,143,143,144,145,146,147,147,148,149,150,151,151,152,153,154,155,156,156,157,158,159,160,161,162,162,163,164,165,166,167,168,169,170,171,171,172,173,174,175,176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,192,193,194,195,196,197,198,200,201,202,203,204,205,206,207,208,210,211,212,213,214,215,216,218,219,220,221,222,224,225,226,227,228,230,231,232,233,235,236,237,239,240,241,243,244,245,246,248,249,251,252,253,255};

static const unsigned char LFO_EXP_KNOB[128] PROGMEM = {2,2,2,2,2,2,2,2,2,2,2,3,3,3,3,3,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,5,5,5,5,5,5,6,6,6,6,6,7,7,7,7,7,8,8,8,9,9,9,10,10,10,11,11,11,12,12,13,13,14,14,15,15,16,17,17,18,19,19,20,21,22,22,23,24,25,26,27,28,29,30,32,33,34,36,37,38,40,42,43,45,47,49,51,53,55,57,59,62,64,67,69,72,75,78,81,84,88,91,95,99,103,107,111,116,120,125,130,136,141,147,153,159};

static const unsigned int MIDI_FREQS[128] PROGMEM =  //all scaled by x4 to get better resolution
{
	33,35,37,39,41,44,46,49,52,55,58,62,65,69,73,78,82,87,92,98,104,110,117,123,131,139,147,156,165,175,185,196,208,220,233,247,262,277,294,311,330,349,370,392,415,440,466,494,523,554,587,622,659,698,740,784,831,880,932,988,1047,1109,1175,1245,1319,1397,1480,1568,1661,1760,1865,1976,2093,2217,2349,2489,2637,2794,2960,3136,3322,3520,3729,3951,4186,4435,4699,4978,5274,5588,5920,6272,6645,7040,7459,7902,8372,8870,9397,9956,10548,11175,11840,12544,13290,14080,14917,15804,16744,17740,18795,19912,21096,22351,23680,25088,26580,28160,29834,31609,33488,35479,37589,39824,42192,44701,47359,50175
};

#endif /* ODYENGINEPROGMEM_H_ */