//SevenSegLedProgmem.h  7 segment display, using minimum progmem and max speed
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


#ifndef SEVENSEGLEDPROGMEM_H_
#define SEVENSEGLEDPROGMEM_H_

#include <avr/pgmspace.h>

const unsigned int LED_CHARS[64] PROGMEM =
{
	0xF01E, //space
	0xB01E, //tape 1
	0x701E, //tape 2
	0xE01E, //tape 3
	0xF00E, //tape 4
	0xF016, //tape 5
	0xF01A, //tape 6
	0xE01E, //tape 7
	0xD01E, //tape 8
	0x3012, //close bracket
	0x1E, //asterix
	0xC00E, //plus
	0xF012, //comma
	0xE01E, //minus
	0xF01C, //full stop
	0x600E, //forward slash
	0x1002, //0
	0x701A, //1
	0x2006, //2
	0x2012, //3
	0x401A, //4
	0x8012, //5
	0x8002, //6
	0x301A, //7
	0x2, //8
	0x12, //9
	0xE016, //colon
	0xE012, //semi colon
	0xE006, //less than
	0xE016, //equals
	0xE012, //greater than
	0x200E, //quest mark
	0x2002, //at
	0xA, //A
	0xC002, //B
	0x9006, //C
	0x6002, //D
	0x8006, //E
	0x800E, //F
	0x9002, //G
	0xC00A, //H
	0xD00E, //I
	0x7002, //J
	0x800A, //K
	0xD006, //L
	0xB00A, //M
	0x100A, //N
	0x1002, //O
	0xE, //P
	0x1A, //Q
	0x100E, //R
	0x8012, //S
	0xC006, //T
	0x5002, //U
	0x5012, //V
	0x5016, //W
	0x400A, //X
	0x4012, //Y
	0x2006, //Z
	0x9006, //open square bracket
	0xC01A, //back slash
	0x3012, //close square bracket
	0x101E, //hat
	0xF016  //underscore
};


#endif /* SEVENSEGLEDPROGMEM_H_ */