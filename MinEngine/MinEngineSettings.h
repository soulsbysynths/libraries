//MinEngineSettings.h  Audio engine settings for miniAtmegatron
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


#ifndef MINENGINESETTINGS_H_
#define MINENGINESETTINGS_H_

#define EXP_ENVELOPES 1
#define WAVE_LENGTH 32  //don't go higher than 32, without altering Oscillator PROGMEM
#define NOTE_PRIORITY 0  //0 = classic atmegatron (only current note), 1 = low note, 2 = high note, 3 = last note
#define LEGATO 0
#define MIDI_CHANNEL 0
#define PATCH_NUM 0

#endif /* MINENGINESETTINGS_H_ */