//Envelope.cpp  Progmem for ADSR Envelope class
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

#ifndef ENVELOPEPROGMEM_H_
#define ENVELOPEPROGMEM_H_

#include <avr/pgmspace.h>

static const char expConvert[128] PROGMEM = {0,1,1,2,2,3,4,4,5,5,6,7,7,8,9,9,10,11,11,12,13,13,14,15,15,16,17,18,18,19,20,20,21,22,23,23,24,25,26,27,27,28,29,30,31,31,32,33,34,35,36,37,37,38,39,40,41,42,43,44,45,46,47,47,48,49,50,51,52,53,54,55,56,57,58,59,61,62,63,64,65,66,67,68,69,70,72,73,74,75,76,77,79,80,81,82,83,85,86,87,89,90,91,92,94,95,96,98,99,100,102,103,105,106,107,109,110,112,113,115,116,118,119,121,122,124,125,127};

#endif /* ENVELOPEPROGMEM_H_ */