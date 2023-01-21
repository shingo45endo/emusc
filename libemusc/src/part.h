/*  
 *  This file is part of libEmuSC, a Sound Canvas emulator library
 *  Copyright (C) 2022-2023  Håkon Skjelten
 *
 *  libEmuSC is free software: you can redistribute it and/or modify it
 *  under the terms of the GNU Lesser General Public License as published
 *  by the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  libEmuSC is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with libEmuSC. If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef __PART_H__
#define __PART_H__


#include "control_rom.h"
#include "pcm_rom.h"
#include "note.h"
#include "settings.h"

#include <stdint.h>

#include <array>
#include <list>
#include <vector>


namespace EmuSC {

class Part
{
public:
  Part(uint8_t id, uint8_t mode, uint8_t type, Settings *settings,
       ControlRom &cRom, PcmRom &pRom);
  ~Part();

  int get_next_sample(float *sampleOut);
  float get_last_peak_sample(void);
  int get_num_partials(void);

  // MIDI Channel Voice Messages
  int set_program(uint8_t index);
  int add_note(uint8_t key, uint8_t velocity);
  int stop_note(uint8_t key);
  int control_change(uint8_t msgId, uint8_t value);
  int channel_pressure(uint8_t value);
  int poly_key_pressure(uint8_t key, uint8_t value);
  int pitch_bend_change(uint8_t lsb, uint8_t msb);

  // MIDI Channel Mode Messages
  int clear_all_notes(void);

  void reset(void);

  uint8_t id(void) { return _id; }

  bool mute() { return(_mute); }
  void set_mute(bool mute) { _mute = mute; }

  uint8_t midi_channel(void) { return _settings->get_param(PatchParam::RxChannel, _id); }

private:
  const uint8_t _id;          // Part id: [0-15] on SC-55, [0-31] on SC-88

  Settings *_settings;

  uint16_t _instrument;       // [0-127] -> variation table
  int8_t _drumSet;            // [0-13] drumSet (SC-55)

  uint8_t _partialReserve;    // [0-24] Default 2

  bool _mute;                 // Part muted

  std::vector<uint8_t> _holdPedalKeys;

  const double _7bScale;      // Constant: 1 / 127

  float _lastPeakSample;

  enum Mode {
    mode_Norm  = 0,
    mode_Drum1 = 1,
    mode_Drum2 = 2
  };

  struct std::list<Note*> _notes;

  ControlRom &_ctrlRom;
  PcmRom &_pcmRom;

  inline bool _le_native(void) { uint16_t n = 1; return (*(uint8_t *) & n); } 
  uint16_t _native_endian_uint16(uint8_t *ptr);

};

}

#endif  // __PART_H__
