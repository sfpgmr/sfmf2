#pragma once
//class encoder
//{
//public:
//  encoder(void);
//  virtual ~encoder(void);
//};
#include "sfmf.h"
#include "sf_memory.h"
#include <boost/msm/back/state_machine.hpp>
#include <boost/msm/front/state_machine_def.hpp>

namespace sf{
  namespace encoder {
    namespace msmf = boost::msm::front;

    enum struct State
    {
        Closed = 0,     // No session.
        Ready,          // Session was created, ready to open a file. 
        OpenPending,    // Session is opening a file.
        Started,        // Session is playing a file.
        Paused,         // Session is paused.
        Stopped,        // Session is stopped (ready to play). 
        Closing         // Application has closed the session, but is waiting for MESessionClosed.
    };

    // ��ԃN���X��`
    struct Closed : msmf::state<> {}; // �Z�b�V�����Ȃ�
    struct Ready : msmf::state<> {}; // �Z�b�V�������쐬����A�t�@�C�����J���������ł��Ă���B
    struct OpenPending :msmf::state<> {};// �Z�b�V�����̓t�@�C�����I�[�v�����Ă���
    struct Started : msmf::state<> {};// �Z�b�V�����͉��t���Ă���B
    struct Paused : msmf::state<> {};// �Z�b�V�����͈ꎞ��~���Ă���B
    struct Stopped : msmf::state<> {};// �Z�b�V�����͒�~���Ă���i���t�\��Ԃł���j�B
    struct Closing : msmf::state<> {};// �A�v���P�[�V�����̓Z�b�V������������AMESessionClosed��Ԃ�҂��Ă���B

    namespace event 
    {
      struct Init {}; // �������C�x���g
      struct OpenURL  // �t�@�C�����J���C�x���g
      {
        OpenURL() {};
        OpenURL(const OpenURL& s) : url_(s.url()) {}
        explicit OpenURL(const std::wstring& u) : url_(u) {}
        const std::wstring& url() const {return url_;}
      private:
        std::wstring url_;
      };
      struct OpenComplete {};// �t�@�C���I�[�v������
      struct Play {};// ���t�J�n
      struct End {};// �I��
      struct Pause {};// �ꎞ��~
      struct Stop {};// ��~
      struct Close {};// ����
    }

    struct encoder_ 
    : public Microsoft::WRL::RuntimeClass<Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>,IMFAsyncCallback>,
      public boost::msm::front::state_machine_def<sf::encoder::encoder_>
  {
    encoder_();
    typedef boost::msm::back::state_machine< encoder_ > this_type;
    typedef Microsoft::WRL::ComPtr<this_type> ptr_type;
    friend struct transition_table;
    typedef boost::signals2::signal<void( this_type &)> signal_t;

    // IMFAsyncCallback methods
    STDMETHODIMP  GetParameters(DWORD*, DWORD*)
    {
      // Implementation of this method is optional.
      return E_NOTIMPL;
    }
    STDMETHODIMP  Invoke(IMFAsyncResult* pAsyncResult);
    void       HandleEvent(UINT_PTR pUnkPtr);
    void start();
    void stop();
    void pause();
    private:

      // �O���ɃC�x���g���΂����߂̃V�O�i��
      signal_t OnReady_;
      signal_t OnOpenURL_;
      signal_t OnOpenComplete_;
      signal_t OnStart_;
      signal_t OnEnd_;
      signal_t OnPause_;
      signal_t OnStop_;

    public:
// ��ԑJ�ڃe�[�u��
struct transition_table : boost::mpl::vector
  //            ���ݏ��      ,�C�x���g           , ���̏��      , �A�N�V����               , �K�[�h 
  < a_row       <Closed        ,ev::Init          ,Ready          ,&:encoder_::Player_::initialize      >,
    a_row       <Ready         ,ev::OpenURL       ,OpenPending    ,&sf::player::Player_::open_url         >,
    a_row        <OpenPending   ,ev::OpenComplete  ,Stopped        ,&sf::player::Player_::open_complete>,
    a_row       <Started       ,ev::Pause         ,Paused         ,&sf::player::Player_::pause           >,
    a_row       <Started       ,ev::Stop          ,Stopped        ,&sf::player::Player_::stop            >,
    _row        <Started       ,ev::End           ,Stopped        >,
    a_row       <Paused        ,ev::Pause         ,Started        ,&sf::player::Player_::resume            >,
    a_row       <Paused        ,ev::Stop          ,Stopped        ,&sf::player::Player_::stop            >,
    a_row       <Stopped       ,ev::Play          ,Started        ,&sf::player::Player_::play            >,
    a_row       <Stopped       ,ev::OpenURL       ,OpenPending    ,&sf::player::Player_::open_url        >//,
   // a_row       <msmf::interrupt_state    ,ev::Close         ,Closed         ,&Player_::shutdown>
  >
{};
  private:
    void create_media_source();
    void configure_output(IMFStreamDescriptorPtr stream_dec,IMFTopologyPtr& topology);
    IMFMediaSessionPtr session_;
    IMFMediaSourcePtr source_;
    handle_holder close_event_;
  };

  typedef boost::msm::back::state_machine< encoder_ > encoder;
  typedef Microsoft::WRL::ComPtr<encoder> PlayerPtr;

  //
  HRESULT InitializeSinkWriter(IMFSinkWriter **ppWriter, DWORD *pStreamIndex);
  HRESULT WriteFrame(
    IMFSinkWriter *pWriter, 
    DWORD streamIndex, 
    const LONGLONG& rtStart,        // Time stamp.
    const LONGLONG& rtDuration      // Frame duration.
    );
  }
}

