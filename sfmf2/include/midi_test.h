#pragma once
/** @file
*  @brief MIDI Output�̃w�b�_�[�t�@�C��
*/
#include "boost/detail/lightweight_test.hpp"
#include "midi_input.h"
#include "midi_output.h"
#include "midi_message.h"
namespace sf {
class midi_test
{
public:
  midi_test()
  {
    // MIDI IN �f�o�C�X���������ăR���e�i�Ɋi�[����
    sf::midi����::�f�o�C�X�̗�();

    // MIDI OUT �f�o�C�X���������ăR���e�i�Ɋi�[����
    sf::midi�o��::�f�o�C�X�̗�();

    ����_.reset(new midi����(sf::midi����::�f�o�C�X���R���e�i().begin()->id()));
    �o��_.reset(new midi�o��(sf::midi����::�f�o�C�X���R���e�i().begin()->id()));
    
    ����_->MIDI���b�Z�[�W������.connect(boost::bind(&midi_test::MIDI���b�Z�[�W������,*this,_1,_2,_3));

  }

  ~midi_test()
  {
    ::midiDisconnect((HMIDI)����_->�n���h��(),�o��_->�n���h��(),NULL);
  };

  void run_test()
  {


    // �R���e�i�Ɋi�[�����f�o�C�X����\��
    std::for_each(sf::midi����::�f�o�C�X���R���e�i().begin(),sf::midi����::�f�o�C�X���R���e�i().end()
      ,[](const sf::midi����::caps& c)
    {
      sf::wdout <<  L"MIDI Input Device: " << c.name() << std::endl;
    }
    );

    // MIDI Input�̊ȒP�ȃe�X�g
    if(!sf::midi����::�f�o�C�X���R���e�i().empty())
    {
      try{
        ����_->��M�J�n();
        ����_->��M��~();
        ����_->���Z�b�g();
        ����_->�N���[�Y();
      } catch (sf::midi���̓G���[& e)
      {
        sf::wdout << boost::wformat(L"Error: %s") % e.what() << std::endl;
      }
    }

  
    // �R���e�i�Ɋi�[�����f�o�C�X����\��
    std::for_each(sf::midi�o��::�f�o�C�X���R���e�i().begin(),sf::midi�o��::�f�o�C�X���R���e�i().end()
      ,[](const sf::midi�o��::�f�o�C�X���& c)
    {
      sf::wdout << L"MIDI Output Device: " << c.���O() << std::endl;
    }
    );

    // MIDI Output�̊ȒP�ȃe�X�g
    if(!sf::midi�o��::�f�o�C�X���R���e�i().empty())
    {
      try{
        �o��_->���Z�b�g();
        �o��_->�N���[�Y();
      } catch (sf::midi�o�̓G���[& e)
      {
        sf::wdout << boost::wformat(L"Error: %s") % e.what() << std::endl;
      }
    }

    �o��_->�I�[�v��();
    ����_->�I�[�v��();

    // 
    ::midiConnect((HMIDI)����_->�n���h��(),�o��_->�n���h��(),NULL);
    ����_->��M�J�n();
  }

  void MIDI���b�Z�[�W������( boost::uint32_t ���b�Z�[�W, boost::uint32_t MIDI���b�Z�[�W, boost::uint32_t �^�C���X�^���v)
  {

    switch( ���b�Z�[�W )
    {
      // normal data message
    case MIM_DATA:
      {
        boost::uint32_t data2_ = ((MIDI���b�Z�[�W & 0xFF0000) >> 16);
        boost::uint32_t data1_ = ((MIDI���b�Z�[�W & 0xFF00) >> 8);
        boost::uint32_t status_ = ((MIDI���b�Z�[�W & 0xFF));

        SFTRACE((boost::wformat(_T("%2x %2x %2x \n")) % status_ % data1_ % data2_));
        switch (status_ & 0xf0)
        {
        case 0x90: // note on
          {
            SFTRACE((boost::wformat(_T("note on: %2x %2x \n")) % data1_ % data2_));
          }
          break;
        case 0x80: // note off
          {
          }
          break;
        case 0xa0: // poly after touch
          {
          }
          break;
        case 0xb0: // control change
          {
            SFTRACE((boost::wformat(_T("control change: %2x %2x \n")) % data1_ % data2_));
            //if(data1_ < 32 )
            //{
            //  m_control_change_buffer[status_ & 0xf][0] = data1_;
            //  m_control_change_buffer[status_ & 0xf][1] = data2_ << 7;

            //  input_messages::instance()->push_back
            //    (
            //    m_control_creators[data1_](current_step,cur_mac,(float)(data2_ << 7) / 16384.0f ,status_ & 0xf)
            //    );

            //} else {
            //  if(data1_ > 31 && data1_ < 64)
            //  {

            //    if(m_control_change_buffer[status_ & 0xf][0] == data1_)
            //    {
            //      data2_ += m_control_change_buffer[status_ & 0xf][1];
            //      input_messages::instance()->push_back(
            //        m_control_creators[data1_](current_step,(main_controller::instance()->current_machine()),(float)data2_ / 16384.0f,status_ & 0xf)
            //        );
            //    };
            //    m_control_change_buffer[status_ & 0xf][0] = 0;
            //    m_control_change_buffer[status_ & 0xf][1] = 0;
            //  } else {
            //    // 64 ... 119
            //    input_messages::instance()->push_back(
            //      m_control_creators[data1_](current_step,sf::model::main_controller::instance()->current_machine(),(float)(data2_) / 127.0f,status_ & 0xf)
            //      );
            //    m_control_change_buffer[status_ & 0xf][0] = 0;
            //    m_control_change_buffer[status_ & 0xf][1] = 0;
            //  }
            //}
          }
          break;
        case 0xc0: // program change
          {
          }
          break;
        case 0xd0: // channel after touch
          {
          }
          break;
        case 0xe0: // pitch bend
          {
            const boost::uint32_t value_ = data1_ << 7 | data2_;
            const float fvalue_ = value_ / 16384.0f; 
          }
          break;
        default:
          break;
        }
      }
      break;
    }
  }
private:
  boost::shared_ptr<midi����> ����_;
  boost::shared_ptr<midi�o��> �o��_;

};
}

