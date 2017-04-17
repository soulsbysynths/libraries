/*
* ChordGen.h
*
* Created: 14/04/2017 11:58:37
* Author: paulsoulsby
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
