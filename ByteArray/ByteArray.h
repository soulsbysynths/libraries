/*
//Byte Array.h  Helper functions to convert ints into big endian byte array (where I should've just used little endian!!)
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
*/


#ifndef BYTEARRAY_H_
#define BYTEARRAY_H_

inline
void convIntToByteArray(int i, unsigned char * uc)
{
    uc[0] = (unsigned int)i >> 8;
    uc[1] = (unsigned int)i & 0xFF;	
}

inline 
int convByteArrayToInt(unsigned char * uc)
{
    unsigned int i = uc[0] << 8;
    i |= uc[1];
	return (int)i;
}

inline
void convUintToByteArray(unsigned int ui, unsigned char * uc)
{
    uc[0] = ui >> 8;
    uc[1] = ui & 0xFF;
}

inline
unsigned int convByteArrayToUint(unsigned char * uc)
{
    unsigned int ui = uc[0] << 8;
    ui |= uc[1];
    return ui;
}

#endif /* BYTEARRAY_H_ */