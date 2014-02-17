#pragma once
#include <new>
#include <windows.h>
#include <shobjidl.h> 
#include <shlwapi.h>
#include <assert.h>
#include <strsafe.h>

#include "sfmf.h"
#include "sf_memory.h"
#include <boost/msm/back/state_machine.hpp>
#include <boost/msm/front/state_machine_def.hpp>

namespace sf{
  namespace media {
    namespace msmf = boost::msm::front;

    enum struct State
    {
      closed = 0,     // No session.
      ready,          // Session was created, ready to open a file. 
      open_pending,    // Session is opening a file.
      started,        // Session is playing a file.
      paused,         // Session is paused.
      stopped,        // Session is stopped (ready to play). 
      closing         // Application has closed the session, but is waiting for MESessionClosed.
    };

    // ��ԃN���X��`
    struct closed : msmf::state<> {}; // �Z�b�V�����Ȃ�
    struct ready : msmf::state<> {}; // �Z�b�V�������쐬����A�t�@�C�����J���������ł��Ă���B
    struct open_pending :msmf::state<> {};// �Z�b�V�����̓t�@�C�����I�[�v�����Ă���
    struct started : msmf::state<> {};// �Z�b�V�����͉��t���Ă���B
    struct paused : msmf::state<> {};// �Z�b�V�����͈ꎞ��~���Ă���B
    struct stopped : msmf::state<> {};// �Z�b�V�����͒�~���Ă���i���t�\��Ԃł���j�B
    struct closing : msmf::state<> {};// �A�v���P�[�V�����̓Z�b�V������������AMESessionClosed��Ԃ�҂��Ă���B

    namespace ev 
    {
      struct init {}; // �������C�x���g
      struct open_url  // �t�@�C�����J���C�x���g
      {
        open_url() {};
        open_url(const open_url& s) : url_(s.url()) {}
        explicit open_url(const std::wstring& u) : url_(u) {}
        const std::wstring& url() const {return url_;}
      private:
        std::wstring url_;
      };
      struct open_complete {};// �t�@�C���I�[�v������
      struct start {};// ���t�J�n
      struct end {};// �I��
      struct pause {};// �ꎞ��~
      struct stop {};// ��~
      struct close {};// ����
    }

    // Media Foundation�̃X�^�[�g�A�b�v�ƃV���b�g�_�E��
    struct start_up
    {
      start_up() 
      {
        // �X�^�[�g�A�b�v
        res_ = MFStartup(MF_VERSION);
        THROW_IF_ERR(res_);
      }

      ~start_up()
      {
        // �V���b�g�_�E��
        if(SUCCEEDED(res_)){
          THROW_IF_ERR(MFShutdown());
        }
        
      }
    private:
      HRESULT res_;
    };

    IMFTopologyPtr create_topology();
    IMFTopologyPtr create_topology(std::wstring& url);
    

    // Media Session ���b�p�[
    struct session_ 
      : public Microsoft::WRL::RuntimeClass<Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>,IMFAsyncCallback>,
      public boost::msm::front::state_machine_def<session_>
    {
      typedef boost::msm::back::state_machine< session_ > this_type;
      typedef Microsoft::WRL::ComPtr<this_type> ptr_type;
      friend struct transition_table;
      typedef boost::signals2::signal<void( this_type &)> signal_t;

      session_();
      virtual ~session_();

      // IMFAsyncCallback methods
      STDMETHODIMP  GetParameters(DWORD*, DWORD*)
      {
        // Implementation of this method is optional.
        return E_NOTIMPL;
      }
      STDMETHODIMP  Invoke(IMFAsyncResult* pAsyncResult);
//      void HandleEvent(UINT_PTR pUnkPtr);
      
      signal_t& on_ready(){return on_ready_;}
      signal_t& on_open_url() {return on_open_url_;}
      signal_t& on_start(){return on_start_;}
      signal_t& on_end(){return on_end_;}
      signal_t& on_pause(){return on_pause_;}
      signal_t& on_stop(){return on_stop_;}
      signal_t& on_open_complete(){return on_open_complete_;}
      void open_complete()
      {
        on_open_complete_(static_cast<this_type&>(*this));
      }

    protected:

      void CreateSession();
      void CloseSession();
      void StartPlayback();

      // Playback
      void initialize( ev::init const& ev);
      void open_url( ev::open_url const& openurl);
      void start( ev::start const& ev);
      void resume( ev::pause const& ev){start(ev::start());};
      void pause( ev::pause const& ev);
      void stop( ev::stop const& ev);
      void shutdown( ev::close const& ev);
      void open_complete(ev::open_complete const&)
      {
        on_open_complete()(static_cast<this_type&>(*this));
      }

      // Media event handlers
      virtual void OnTopologyStatus(IMFMediaEventPtr pEvent){};
      virtual void OnPresentation_ended(IMFMediaEventPtr pEvent){};
      virtual void OnNewPresentation(IMFMediaEventPtr pEvent){};

      // Override to handle additional session events.
      virtual void OnSessionEvent(IMFMediaEventPtr, MediaEventType) 
      { 
        //return S_OK; 
      }
      void HandleEvent(UINT_PTR pEventPtr);
    private:

      // �O���ɃC�x���g���΂����߂̃V�O�i��
      signal_t on_ready_;
      signal_t on_open_url_;
      signal_t on_open_complete_;
      signal_t on_start_;
      signal_t on_end_;
      signal_t on_pause_;
      signal_t on_stop_;

    public:
      // ��ԑJ�ڃe�[�u��
      struct transition_table : boost::mpl::vector
        //            ���ݏ��     ,�C�x���g           , ���̏��      , �A�N�V����               , �K�[�h 
        < a_row     <closed        ,ev::init          ,ready          ,&session_::initialize      >,
        a_row       <ready         ,ev::open_url      ,open_pending    ,&session_::open_url         >,
        a_row       <open_pending  ,ev::open_complete ,stopped        ,&session_::open_complete>,
        a_row       <started       ,ev::pause         ,paused         ,&session_::pause           >,
        a_row       <started       ,ev::stop          ,stopped        ,&session_::stop            >,
        _row        <started       ,ev::end           ,stopped        >,
        a_row       <paused        ,ev::pause         ,started        ,&session_::resume            >,
        a_row       <paused        ,ev::stop          ,stopped        ,&session_::stop            >,
        a_row       <stopped       ,ev::start         ,started        ,&session_::start            >,
        a_row       <stopped       ,ev::open_url     ,open_pending    ,&session_::open_url        >//,
        // a_row       <msmf::interrupt_state    ,ev::Close         ,Closed         ,&Player_::shutdown>
        >
      {};
      typedef closed initial_state;
      IMFMediaSessionPtr media_session()
      {
        assert(media_session_);
        return media_session_;
      };
    private:
//      void create_media_source();
      void create_session();
      void close_session();
//      void configure_output(IMFStreamDescriptorPtr stream_dec,IMFTopologyPtr& topology);
      IMFMediaSessionPtr media_session_;
//      IMFMediaSourcePtr media_source_;
      handle_holder close_event_;
    };

    typedef boost::msm::back::state_machine< session_ > session;
    typedef Microsoft::WRL::ComPtr<session> session_ptr;

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

