/*
//ChordGen.cpp  Chord generator from 2 CV inputs - written for AT-ARP
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



#include "ChordGen.h"

// default constructor
ChordGen::ChordGen()
{
} //ChordGen

// default destructor
ChordGen::~ChordGen()
{
} //~ChordGen

void ChordGen::setRootNote(unsigned char newNote)
{
	rootNote_ = newNote;
	refreshNoteNumbers();
}

void ChordGen::setChordNote(unsigned char newNote)
{
	chordNote_ = newNote;
	chordType_ = newNote % 12;
	chordOctaves_ = (newNote / 12) + 1;
	refreshNoteNumbers();
}

void ChordGen::refreshNoteNumbers()
{
	unsigned int note = 0;
	memset(noteNumbers_,0,sizeof(noteNumbers_));
	chordSize_ = 0;
	for(unsigned char i=0;i<chordOctaves_;++i)
	{
		for(unsigned char j=0;j<4;++j)
		{
			note = (unsigned int)pgm_read_byte(&(CG_CHORD_DATA[chordType_][j]));
			if(note!=CG_X)
			{
				note += rootNote_;
				note += (i * 12);
				if(note<=127)
				{
					noteNumbers_[chordSize_] = note;
					chordSize_++;
				}
			}
		}
	}
}