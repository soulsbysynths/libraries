/*
* ChordGen.cpp
*
* Created: 14/04/2017 11:58:37
* Author: paulsoulsby
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