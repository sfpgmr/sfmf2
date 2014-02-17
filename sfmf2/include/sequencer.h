#pragma once
/*
  ==============================================================================

   This file is part of the async
   Copyright 2005-11 by Satoshi Fujiwara.

   async can be redistributed and/or modified under the terms of the
   GNU General Public License, as published by the Free Software Foundation;
   either version 2 of the License, or (at your option) any later version.

   async is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with async; if not, visit www.gnu.org/licenses or write to the
   Free Software Foundation, Inc., 59 Temple Place, Suite 330, 
   Boston, MA 02111-1307 USA

  ==============================================================================
*/
#include <audioclient.h>
#include "exception.h"
#include "audio_source.h"
#include "sf_memory.h"
#include "audio_processor.h"

namespace sf {

//#pragma pack(push,8)

  struct sequencer;

  struct command_base 
  {
    //virtual void process(sequencer& s);
  };

  struct seq_event
  {

    uint64_t step() {return step_;}
    std::unique_ptr<command_base>& command() {return command_;}

    void step(uint64_t v) {step_ = v;}
  private:
    uint64_t step_;
    std::unique_ptr<command_base> command_;
  };

  struct track_base 
  {
    const std::wstring& name() {return name_;}
    const std::wstring& comment() {return comment_;}

    void name(const std::wstring& v) {name_ = v;}
    void comment(const std::wstring& v) {comment_ = v;}

  private:
    std::wstring name_;
    std::wstring comment_;
  };

  struct track : public track_base
  {
    uint64_t step() {return step_;}// �X�e�b�v�I�t�Z�b�g
    int32_t key() {return key_;}// �L�[�I�t�Z�b�g
    uint32_t midi_channel() {return midi_channel_;}// MIDI�`�����l��
    audio_processor* processor() {return processor_;}// �I�[�f�B�I�G���W���ւ̎Q��
    std::vector<seq_event>& events() {return events_;}// �C�x���g

    void step(uint64_t v) {step_ = v;}// �X�e�b�v�I�t�Z�b�g
    void key(int32_t v) {key_ = v;}// �L�[�I�t�Z�b�g
    void midi_channel(uint32_t v) {midi_channel_ = v;}// MIDI�`�����l��
    void processor(audio_processor* v) {processor_ = v;}// �I�[�f�B�I�G���W���ւ̎Q��

  private:
    uint64_t step_;// �X�e�b�v�I�t�Z�b�g
    int32_t key_;// �L�[�I�t�Z�b�g
    uint32_t midi_channel_;// MIDI�`�����l��
    audio_processor* processor_;// �I�[�f�B�I�G���W���ւ̎Q��
    std::vector<seq_event> events_;// �C�x���g
  };

  struct pattern : public command_base
  {

    const std::wstring& name() {return name_;}
    const std::wstring& comment() {return comment_;}
    int32_t key() {return key_;}// �L�[�I�t�Z�b�g
    uint32_t step() {return step_;}// �X�e�b�v�I�t�Z�b�g
    std::vector<track_base>& tracks() {return tracks_;}// 

    void name(const std::wstring v) {name_ = v;}
    void comment(const std::wstring v) {comment_ = v;}
    void key(int32_t v) {key_ = v;}// �L�[�I�t�Z�b�g
    void step(uint32_t v) {step_ = v;}// �X�e�b�v�I�t�Z�b�g
  private:
    std::wstring name_;
    std::wstring comment_;
    int32_t key_;// �L�[�I�t�Z�b�g
    uint32_t step_;// �X�e�b�v�I�t�Z�b�g
    std::vector<track_base> tracks_;// 
  };

  struct song_t 
  {
    song_t();
    
    std::wstring& name() {return name_;}
    std::wstring& comment() {return comment_;}
    uint32_t time_base(){return time_base_;}// �^�C���x�[�X(�l�������̕���\)
    uint32_t tempo(){return tempo_;}// �e���|
    uint32_t denominator(){return denominator_;}// ����
    uint32_t numerator(){return numerator_;}// ���q
    int32_t key(){return key_;}// ��
    std::vector<pattern>& patterns(){return patterns_;}// �p�^�[���z��

    void name(std::wstring& v) {name_ = v;}
    void comment(std::wstring& v) {comment_ = v;}
    void time_base(uint32_t v) {time_base_ = v;}// �^�C���x�[�X(�l�������̕���\)
    void tempo(uint32_t v) {tempo_ = v;}// �e���|
    void denominator(uint32_t v) {denominator_ = v;}// ����
    void numerator(uint32_t v) {numerator_ = v;}// ���q
    void key(int32_t v) {key_ = v;}// ��

  private:
    std::wstring name_;
    std::wstring comment_;
    uint32_t time_base_;// �^�C���x�[�X(�l�������̕���\)
    uint32_t tempo_;// �e���|
    uint32_t denominator_;// ����
    uint32_t numerator_;// ���q
    int32_t key_;// ��
    std::vector<pattern> patterns_;// �p�^�[���z��
  };

  struct note_command : public command_base
  {
    note_command(uint32_t n,uint32_t g,uint32_t v) 
      : note_(n),gate_time_(g),velocity_(v) {};

    uint32_t note() {return note_;};
    uint32_t gate_time() {return gate_time_;};
    uint32_t velocity() {return velocity_;};

    void note(uint32_t v) {note_ = v;}
    void gate_time(uint32_t v) {gate_time_ = v;}
    void velocity(uint32_t v) {velocity_ = v;}

  private:
    uint32_t note_;
    uint32_t gate_time_;
    uint32_t velocity_;
  };

//#pragma pack(pop)

  struct sequencer : public audio_source
  {
  public:

    sequencer();
    ~sequencer(){};
    virtual bool seekable(){return true;};
    virtual bool stream_status(){return true;};
    virtual WAVEFORMATEXTENSIBLE &get_wave_format() {return WAVEFORMATEXTENSIBLE();};
    virtual bool more_data_available(){return false;};
    virtual void read_data(BYTE *buffer, uint64_t numbytes){};
    virtual void reset_data_position(){};
    virtual uint64_t total_data_bytes(){return 0;};
    virtual void seek(uint64_t pos){};
    virtual uint64_t data_bytes_remaining(){return 0;};
    virtual HANDLE raw_handle(){return 0;};
    virtual void wait(int timer = -1){};
    song_t& song() {return song_;};

  private:
    
    uint32_t current_tempo_;
    uint32_t current_denominator_;// ����
    uint32_t current_numerator_;// ���q
    uint64_t current_pos_;// �Đ��|�W�V����
    uint32_t current_key_;// �L�[�I�t�Z�b�g
    uint32_t current_meas_;
    song_t song_;
  };
}

 

