/*
//ChordGen.h  Chord generator from 2 CV inputs - written for AT-ARP
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


#ifndef __CHORDGEN_H__
#define __CHORDGEN_H__

#include <string.h>
#include "ChordGenProgmem.h"

class ChordGen
{
	//variables
	public:
	enum ChordType : unsigned char
	{
		CT_ROOT = 0,
		CT_MINOR_THIRD,
		CT_MAJOR_THIRD,
		CT_FIFTH,
		CT_MINOR,
		CT_MAJOR,
		CT_ADD_FOURTH,
		CT_AUGMENTED,
		CT_DIMINISHED,
		CT_MIN_MIN_SEV,
		CT_MAJ_MIN_SEV,
		CT_MAJ_MAJ_SEV
	};
	protected:
	private:
	unsigned char noteNumbers_[44] = {0};
	unsigned char chordSize_ = 0;
	unsigned char rootNote_ = 0;
	unsigned char chordNote_ = 0;
	ChordType chordType_ = 0;
	unsigned char chordOctaves_ = 0;
	//functions
	public:
	ChordGen();
	~ChordGen();
	unsigned char getNoteNumbers(unsigned char index){return noteNumbers_[index];}
	unsigned char getChordSize(){return chordSize_;}
	ChordType getChordType(){return chordType_;}
	void setRootNote(unsigned char newNote);
	unsigned char getRootNote(){return rootNote_;}
	void setChordNote(unsigned char newNote);
	unsigned char getChordNote(){return chordNote_;}
	protected:
	private:
	ChordGen( const ChordGen &c );
	ChordGen& operator=( const ChordGen &c );
	void refreshNoteNumbers();
}; //ChordGen

#endif //__CHORDGEN_H__
