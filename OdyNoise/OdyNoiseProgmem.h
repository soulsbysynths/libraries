//OdyNoiseProgmem.h  Odytron white noise simulation. pink filter not used.  creates white-noise-esque noise super quick.
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

#ifndef ODYNOISEPROGMEM_H_
#define ODYNOISEPROGMEM_H_

#include <avr/pgmspace.h>

static const char NOISE_MAX[8] PROGMEM = {0,21,34,48,65,83,105,127};
static const char NOISE_MIN[8] PROGMEM = {0,-21,-34,-48,-65,-83,-105,-127};
static const unsigned char PRIME_SIZE = 50;
static const unsigned char PRIME_NUMBERS[PRIME_SIZE] PROGMEM = {11,13,17,19,23,29,31,37,41,43,47,53,59,61,67,71,73,79,83,89,97,101,103,107,109,113,127,131,137,139,149,151,157,163,167,173,179,181,191,193,197,199,211,223,227,229,233,239,241,251};

#endif /* ODYNOISEPROGMEM_H_ */