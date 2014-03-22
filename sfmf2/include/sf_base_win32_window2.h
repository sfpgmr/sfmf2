#pragma once
/*
*/
// Windows Header Files:
#define XBYAK64
#include "exception.h"
#include "sf_windows_base.h"

#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

namespace sf{
  /** window ベースクラス */
  template <typename DerivedType, template <typename> class RendererType,typename WindowProcType = wndproc>
  struct base_win32_window2 : public base_window 
  {
	typedef typename WindowProcType proc_t;
	typedef typename proc_t::return_type result_t;
    typedef RendererType<DerivedType> renderer_t;

    HWND hwnd() const override {return hwnd_;};

    virtual float width() const override { return width_; }
    virtual float height() const override { return height_; }
    virtual sf::dpi& dpi() override { return dpi_; };
    virtual bool is_fullscreen() override { return fullscreen_; }
	virtual MARGINS& margins() override { return margins_; }
	virtual on_closed_t& on_closed() override { return on_closed_; }

  protected:

	  base_win32_window2(
      const std::wstring& title,
      const std::wstring& name,bool fit_to_display,
      float width,float height) :
	  title_(title), name_(name),
	  fit_to_display_(fit_to_display),
	  thunk_(this, reinterpret_cast<proc_t::proc_type>(base_win32_window2::WndProc)),
	  hwnd_(0), timer_(*this, 10),
	  fullscreen_(true)
	  {
		  width_ = dpi_.scale_x(width);
		  height_ = dpi_.scale_y(height);
		  memset(&wp_, 0, sizeof(wp_));
		  wp_.length = sizeof(WINDOWPLACEMENT);
		  thunk_proc_ = (WNDPROC) thunk_.getCode();
	  }

	  ~base_win32_window2(){};

	  void register_class(
		  const wchar_t* menu_name,
		  uint32_t style,
		  int32_t     cbClsExtra = 0,
		  int32_t     cbWndExtra = sizeof(LONG_PTR),
		  HICON       hIcon = ::LoadIcon(NULL, IDI_APPLICATION),
		  HCURSOR     hCursor = ::LoadCursor(NULL, IDC_ARROW),
		  HBRUSH      hbrBackground = ::CreateSolidBrush(0xff000000),
		  HICON       hIconSm = NULL
		  )
	  {
		  wnd_class_.reset(new sf::window_class_ex(menu_name, name_, HINST_THISCOMPONENT, &start_wnd_proc, style, cbClsExtra, cbWndExtra, hIcon, hCursor, hbrBackground, hIconSm));
	  }

    /** デフォルト設定 */
	void register_class()
	{
		wnd_class_.reset(new sf::window_class_ex(0, name_, HINST_THISCOMPONENT, &start_wnd_proc));
	}

	void create_window(HWND parent = NULL)
	{
		{
			// Create the application window.
			//
			// Because the CreateWindow function takes its size in pixels, we
			// obtain the system DPI and use it to scale the window size.
			//FLOAT dpiX, dpiY;
			//d2d_factory_->GetDesktopDpi(&dpiX, &dpiY);


			// Windowを作成する
			// Windowを作成する
			CreateWindowEx(
				WS_EX_OVERLAPPEDWINDOW,
				name_.c_str(),
				title_.c_str(),
				WS_OVERLAPPED | WS_SYSMENU | WS_THICKFRAME,
				CW_USEDEFAULT,
				CW_USEDEFAULT,
				static_cast<uint32_t>(width_),
				static_cast<uint32_t>(height_),
				parent,
				NULL,
				HINST_THISCOMPONENT,
				this
				);
			::GetWindowPlacement(hwnd_, &wp_);
		}
	}

  public:

	  //template <typename C>
	  //inline auto window_proc(C& c, HWND hwnd, uint32_t message, WPARAM wParam, LPARAM lParam) -> LRESULT
	  //{
		 // return proc_t::def_wnd_proc(hwnd, message, wParam, lParam);
	  //}

	  //template <typename C>
	  //inline auto window_proc(C& c, HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) -> decltype(c.window_proc(HWND, UINT, WPARAM, LPARAM))
	  //{
		 // c.window_proc(hwnd,message,wParam,lParam);
	  //}


	  //virtual result_t on_close()
   // {
   //   renderer_->discard();
   //   sf::graphics::instance()->discard_device();
   //   // Windowの破棄
   //   BOOL ret(::DestroyWindow(hwnd_));
   //   BOOST_ASSERT(ret != 0);
   //   return std::is_same<proc_t,wndproc>::value?1:TRUE;
   // }

  protected:

    // Window生成後呼ばれる関数
    // WM_NCCREATEメッセージの時にthunkに切り替える
    static result_t CALLBACK start_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
      if(message == WM_NCCREATE)
      {
        LPCREATESTRUCT param = reinterpret_cast<LPCREATESTRUCT>(lParam);
        base_win32_window2* ptr = reinterpret_cast<base_win32_window2*>(param->lpCreateParams);
        ptr->hwnd_ = hwnd;
        // ウィンドウプロシージャをインスタンスと結び付けるthunkプロシージャに入れ替える
        LONG_PTR r = SetWindowLongPtr(hwnd,GWLP_WNDPROC,reinterpret_cast<LONG_PTR>(ptr->thunk_proc_));
        assert(r == reinterpret_cast<LONG_PTR>(&start_wnd_proc));
		return static_cast<DerivedType&>(*ptr).window_proc(hwnd, message, wParam, lParam);
      }
      return ::DefWindowProcW(hwnd,message,wParam,lParam);
    };

    static result_t CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
      base_win32_window2* ptr = reinterpret_cast<base_win32_window2*>(hwnd);
      return static_cast<DerivedType&>(*ptr).window_proc(ptr->hwnd_,message,wParam,lParam);
    };

    // thisとhwndをつなぐthunkクラス
    struct hwnd_this_thunk : public Xbyak::CodeGenerator {
      hwnd_this_thunk(base_win32_window2* impl,typename proc_t::proc_type proc)
      {
        // rcxにhwndが格納されているので、それをimpl->hwndに保存
	    	mov(rax,(size_t)&impl->hwnd_);	
        mov(ptr[rax],rcx);				
        // 代わりにthisのアドレスをrcxに格納
        mov(rcx,(LONG_PTR)impl);
        // r10にproc(Window プロシージャ)へのアドレスを格納
        mov(r10,(LONG_PTR)proc);
        // Window プロシージャへへジャンプ
        jmp(r10);
      }
    };

    void update_window_size()
    {
      RECT r;
      GetWindowRect(hwnd_,&r);
      width_ = r.right - r.left;
      height_ = r.bottom - r.top;
	  calc_client_size();
	}

	void calc_client_size()
	{
		//クライアント領域の現在の幅、高さを求める
		RECT rc;
		GetClientRect(hwnd_, &rc);
		client_width_ = rc.right - rc.left;
		client_height_ = rc.bottom - rc.top;
	}

    HWND hwnd_;
    hwnd_this_thunk thunk_;
    std::wstring title_;
    std::wstring name_;
    float width_,height_;
	bool fit_to_display_;
    std::shared_ptr<sf::window_class_ex> wnd_class_;
    typename proc_t::proc_type thunk_proc_;
    sf::dpi dpi_;
    WINDOWPLACEMENT wp_;
    bool init_;
    std::unique_ptr<renderer_t> renderer_;
	MARGINS margins_;
	on_closed_t on_closed_;

    float client_width_,client_height_;
	// 

    bool activate_;
    bool fullscreen_;
    timer timer_;

  };


  //typedef base_win32_window2<> base_win32_window2_t;
  //typedef base_win32_window2<dlgproc> base_win32_dialog2_t;

}