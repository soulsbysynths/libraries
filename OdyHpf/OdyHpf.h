//OdyHpf.h  Odytron high pass filter.  Now just holds cutoff.  Not really needed.
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


#ifndef __ODYHPF_H__
#define __ODYHPF_H__


class OdyHpf
{
//variables
public:
protected:
private:
	unsigned char fc_ = 0;
//functions
public:
	OdyHpf();
	~OdyHpf();
	void setFc(unsigned char newFc){fc_ = newFc;}
	unsigned char getFc(){return fc_;}	
protected:
private:
	OdyHpf( const OdyHpf &c );
	OdyHpf& operator=( const OdyHpf &c );

}; //OdyHpf

#endif //__ODYHPF_H__
