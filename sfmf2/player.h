

#ifndef PLAYER_H
#define PLAYER_H

#include <boost/msm/back/state_machine.hpp>
#include <boost/msm/front/state_machine_def.hpp>

namespace sf {
  namespace player {
    namespace msmf = boost::msm::front;

    // const UINT WM_APP_PLAYER_EVENT = WM_APP + 1;   
    // WPARAM = IMFMediaEvent*, WPARAM = MediaEventType

    enum struct PlayerState
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

    namespace ev 
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

    struct Player_ : public IMFAsyncCallback,public boost::msm::front::state_machine_def<sf::player::Player_>
    {
      typedef boost::msm::back::state_machine< Player_ > this_type;
      typedef Microsoft::WRL::ComPtr<this_type> ptr_type;
      friend ptr_type CreatePlayer(HWND hVideo, HWND hEvent);
      friend struct transition_table;
      typedef boost::signals2::signal<void( this_type &)> signal_t;


      // IUnknown methods
      STDMETHODIMP QueryInterface(REFIID iid, void** ppv);
      STDMETHODIMP_(ULONG) AddRef();
      STDMETHODIMP_(ULONG) Release();

      // IMFAsyncCallback methods
      STDMETHODIMP  GetParameters(DWORD*, DWORD*)
      {
        // Implementation of this method is optional.
        return E_NOTIMPL;
      }
      STDMETHODIMP  Invoke(IMFAsyncResult* pAsyncResult);

      void       HandleEvent(UINT_PTR pUnkPtr);
 //     PlayerState   GetState() const { return m_state; }

      // Video functionality
      void       Repaint();
      void       ResizeVideo(WORD width, WORD height);

      BOOL          HasVideo() const { return (m_pVideoDisplay != NULL);  }

      signal_t& OnReady(){return OnReady_;}
      signal_t& OnOpenURL() {return OnOpenURL_;}
      signal_t& OnStart(){return OnStart_;}
      signal_t& OnEnd(){return OnEnd_;}
      signal_t& OnPause(){return OnPause_;}
      signal_t& OnStop(){return OnStop_;}
      signal_t& OnOpenComplete(){return OnOpenComplete_;}

      void OpenComplete()
      {
        OnOpenComplete_(static_cast<this_type&>(*this));
      }


    protected:


      // Constructor is private. Use static CreateInstance method to instantiate.
      Player_(HWND hVideo, HWND hEvent);

      // Destructor is private. Caller should call Release.
      virtual ~Player_(); 

      void CreateSession();
      void CloseSession();
      void StartPlayback();

      // Playback
      void initialize( ev::Init const& ev);
      void open_url( ev::OpenURL const& openurl);
      void play( ev::Play const& ev);
      void resume( ev::Pause const& ev){play(ev::Play());};
      void pause( ev::Pause const& ev);
      void stop( ev::Stop const& ev);
      void shutdown( ev::Close const& ev);
      void open_complete(ev::OpenComplete const&)
      {
        OnOpenComplete()(static_cast<this_type&>(*this));
      }

      // Media event handlers
      virtual void OnTopologyStatus(IMFMediaEventPtr pEvent);
      virtual void OnPresentationEnded(IMFMediaEventPtr pEvent);
      virtual void OnNewPresentation(IMFMediaEventPtr pEvent);

      // Override to handle additional session events.
      virtual void OnSessionEvent(IMFMediaEventPtr, MediaEventType) 
      { 
        //return S_OK; 
      }


      long                    m_nRefCount;        // Reference count.

      IMFMediaSessionPtr         m_pSession;
      IMFMediaSourcePtr          m_pSource;
      IMFVideoDisplayControlPtr  m_pVideoDisplay;

      HWND                    m_hwndVideo;        // Video window.
      HWND                    m_hwndEvent;        // App window to receive events.
     // PlayerState             m_state;            // Current state of the media session.
      HANDLE                  m_hCloseEvent;      // Event to wait on while closing.

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
  < a_row       <Closed        ,ev::Init          ,Ready          ,&sf::player::Player_::initialize      >,
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
      typedef Closed initial_state;


      template <class FSM,class Event>
      void no_transition(Event const& e, FSM&,int state)
      {
        throw exception(L"No Transition");
      }

      template <class FSM>
      void no_transition(ev::Close const& e, FSM&,int state)
      {
        shutdown(ev::Close());
      }

    };

    typedef boost::msm::back::state_machine< Player_ > Player;

    typedef Microsoft::WRL::ComPtr<Player> PlayerPtr;
    PlayerPtr CreatePlayer(HWND hVideo, HWND hEvent);

  }
}
#endif PLAYER_H
