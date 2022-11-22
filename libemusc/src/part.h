/*  
 *  This file is part of libEmuSC, a Sound Canvas emulator library
 *  Copyright (C) 2022  Håkon Skjelten
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

#include <stdint.h>

#include <list>
#include <vector>


namespace EmuSC {

class Part
{
private:
  const uint8_t _id;          // Part id: [0-15] on SC-55, [0-31] on SC-88
  
  uint16_t _instrument;       // [0-127] -> variation table
  int8_t _drumSet;            // [0-13] drumSet (SC-55)
  uint8_t _volume;            // [0-127] 100 is factory preset
  int8_t _pan;                // [-63-63] 
  uint8_t _reverb;            // [0-127]
  uint8_t _chorus;            // [0-127]
  int8_t _keyShift;           // [-24-24]
  uint8_t _midiChannel;       // [0-15] MIDI channel

  uint8_t _mode;              // [0-2] 0=Norm, 1=Drum1, 2=Drum2
  uint8_t _bendRange;         // [0-24] Default 2
  uint8_t _modDepth;          // [0-127] Default 10
  uint8_t _keyRangeL;         // [c1-g9] Default  24 => C1
  uint8_t _keyRangeH;         // [c1-g9] Default 127 => G9
  uint8_t _velSensDepth;      // [0-127] Default 64
  uint8_t _velSensOffset;     // [0-127] Default 64
  uint8_t _partialReserve;    // [0-24] Default 2
  bool _polyMode;             // true = poly, false = mono

  int8_t _vibRate;            // [-50-50] Default 0
  int8_t _vibDepth;           // [-50-50] Default 0
  int8_t _vibDelay;           // [-50-50] Default 0
  int8_t _cutoffFreq;         // [-50-16] Default 0
  int8_t _resonance;          // [-50-50] Default 0
  int8_t _attackTime;         // [-50-50] Default 0
  int8_t _decayTime;          // [-50-50] Default 0
  int8_t _releaseTime;        // [-50-50] Default 0

  bool _mute;                 // Part muted
  int8_t &_synthKeyShift;    // Global key shift on top of part setting
  uint8_t _modulation;        // [0-127] MOD Wheel (CM 1) Default 0
  uint8_t _expression;        // [0-127] temporary volume modifier (CM 11)
  bool _portamento;           // Portamento pitch slide [on / off] Default off
  bool _holdPedal;            // Hold all notes [on / off] Default off
  uint8_t _portamentoTime;    // [0-127] Pitch slide, 0 is slowest

  uint8_t _programIndex;      // Current program index inside variation bank
  uint8_t _programBank;       // Current variation bank

  std::vector<uint8_t> _holdPedalKeys;
  //    uint8_t lever;          // [0-127]

  float _pitchBend;

  const double _7bScale;      // Constant: 1 / 127

  float _lastPeakSample;
  
  enum Mode {
    mode_Norm  = 0,
    mode_Drum1,
    mode_Drum2
  };
  
  enum AdsrState {
    adsr_Attack,
    adsr_Hold,
    adsr_Decay,
    adsr_Sustain,
    adsr_Release,
    adsr_Done
  };

  struct std::list<Note*> _notes;

  ControlRom &_ctrlRom;
  PcmRom &_pcmRom;

public:
  Part(uint8_t id, uint8_t mode, uint8_t type, int8_t &keyShift,
       ControlRom &cRom, PcmRom &pRom);
  ~Part();

  enum ControlMsg {
    cmsg_Unknown,
    cmsg_ModWheel,
    cmsg_PortamentoTime,
    cmsg_Volume,
    cmsg_Pan,
    cmsg_Expression,
    cmsg_HoldPedal,
    cmsg_Portamento,
    cmsg_Reverb,
    cmsg_Chorus
  };

  int get_next_sample(float *sampleOut);
  float get_last_peak_sample(void);
  int get_num_partials(void);

  int add_note(uint8_t key, uint8_t velocity, uint32_t sr);
  int stop_note(uint8_t key);
  int clear_all_notes(void);

  int set_control(enum ControlMsg m, uint8_t value);
  int set_pitchBend(int16_t pitchbend);

  uint8_t id(void) { return _id; }

  bool mute() { return(_mute); }
  void set_mute(bool mute) { _mute = mute; }

  uint8_t program(uint8_t &b) { b = _programBank; return _programIndex; }
  int set_program(uint8_t index, uint8_t bank);
  
  uint8_t level(void) { return _volume; }
  void set_level(uint8_t level) { _volume = level; }

  int8_t pan(void) { return _pan; }
  void set_pan(int8_t pan) { _pan = pan; }

  uint8_t reverb(void) { return _reverb; }
  void set_reverb(uint8_t reverb) { _reverb = reverb; }

  uint8_t chorus(void) { return _chorus; }
  void set_chorus(uint8_t chorus) { _chorus = chorus; }

  int8_t key_shift(void) { return _keyShift; }
  void set_key_shift(int8_t keyShift) { _keyShift = keyShift; }

  uint8_t midi_channel(void) { return _midiChannel; }
  void set_midi_channel(uint8_t midiChannel) { _midiChannel = midiChannel; }

  uint8_t mode(void) { return _mode; }
  void set_mode(uint8_t mode) { _mode = mode; }
};

}

#endif  // __PART_H__