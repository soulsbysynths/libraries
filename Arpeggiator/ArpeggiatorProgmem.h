//ArpeggiatorProgmem.cpp  Progemem for Arpeggiator Class
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

#ifndef ARPEGGIATORPROGMEM_H_
#define ARPEGGIATORPROGMEM_H_

#include <avr/pgmspace.h>

static const unsigned int ARP_MULT[16] PROGMEM = {1,1,1,85,3,1,85,3,1,341,1,341,1,1365,1,1};
static const unsigned char ARP_BS[16] PROGMEM = {12,11,10,16,11,9,15,10,8,16,7,15,6,16,5,4};
static const unsigned char ARP_BIT_MASK = 15;
static const unsigned char ARP_LENGTH = 16;

static const char ARP_TYPES[16][16] PROGMEM =
{
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},                          //THIS ONE SHOULD ALWAYS BE BLANK
	{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16},                   //up
	{-1,-2,-3,-4,-5,-6,-7,-8,-9,-10,-11,-12,-13,-14,-15,-16},   //down
	{1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8},                          //octave
	{1,1,2,3,4,4,5,6,7,7,8,9,10,10,11,12},                      //5ths
	{1,3,2,4,3,5,4,6,5,7,6,8,7,9,8,10},                         //skip 1 up
	{-1,-3,-2,-4,-3,-5,-4,-6,-5,-7,-6,-8,-7,-9,-8,-10},         //skip 1 down
	{-1,-1,-1,-1,-3,-3,-2,-2,-1,-1,-1,-1,-3,-3,-2,-2},          //I feel love
	{2,1,3,2,4,5,6,1,2,1,3,2,4,5,6,1},                          //Horton
	{-1,-2,-1,-4,-2,-1,-4,-1,-4,-2,-3,-2,-1,-3,-2,-1},          //Etheridge
	{1,2,3,2,4,3,2,3,1,3,2,4,1,3,2,1},                          //Meads
	{1,2,3,2,-1,6,5,4,1,1,-1,-2,1,1,4,1},                       //Palmer
	{4,1,4,1,3,1,4,1,2,1,3,1,4,1,4,1},                          //gwEm 1
	{1,1,4,4,1,1,4,4,3,3,6,6,2,2,5,5},                          //gwEm 2
	{1,1,3,2,2,4,3,3,5,4,4,6,5,5,7,6},                          //Triplet up
	{1,2,3,2,1,2,3,2,1,2,3,2,1,2,3,2},                          //You Will Be Robyn
};



#endif /* ARPEGGIATORPROGMEM_H_ */