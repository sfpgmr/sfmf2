#pragma once
/*
*/
// Windows Header Files:
#define XBYAK64
#include "exception.h"
#include "sf_windows_base.h"

namespace sf{

  /** window ベースクラス */
  template <typename ProcType = wndproc>
  struct base_win32_window : public base_window 
  {
    typedef ProcType proc_t;
    typedef typename proc_t::return_type result_t;
//    typedef proc_t::return_type result_t;

    operator HWND() const {return hwnd_;};

    virtual void * raw_handle() const {return hwnd_;};
    //    virtual void show(uint32_t show_flag);

    virtual void show() {
      ::ShowWindow(hwnd_,SW_SHOW);
      ::GetWindowPlacement(hwnd_,&wp_);
    }

    // Window を画面から隠す
    virtual bool is_show() {
      return ( wp_.showCmd == SW_SHOWMAXIMIZED 
        || wp_.showCmd == SW_SHOWMINIMIZED
        || wp_.showCmd == SW_SHOWNORMAL );
    };

    //
    virtual void hide()
    {
      ::ShowWindow(hwnd_,SW_HIDE);
      ::GetWindowPlacement(hwnd_,&wp_);
    };

    virtual bool is_activate() { return activate_; };
    virtual float width() { return width_; }
    virtual float height() { return height_; }
    virtual sf::dpi& dpi() { return dpi_; };
    virtual bool is_fullscreen() { return fullscreen_; }

    ;


    virtual void text(std::wstring& text)
    {
      ::SetWindowTextW(*this,text.c_str());
    };


    virtual void update();

    virtual void create_device_independent_resources();
    virtual void create_device();

    virtual void create_swapchain_dependent_resources();
    virtual void create_d2d_render_target();

    virtual void discard_swapchain_dependent_resources();
    virtual void restore_swapchain_and_dependent_resources();
    virtual void discard_device();
    virtual void discard_device_independant_resources();
    virtual void discard_d2d_render_target();

  protected:
    base_win32_window(
      const std::wstring& title,
      const std::wstring& name,bool fit_to_display,
      float width,float height);


    ~base_win32_window();

    void register_class (
      const wchar_t* menu_name,
      uint32_t style, 
      int32_t     cbClsExtra  = 0,
      int32_t     cbWndExtra  = sizeof(LONG_PTR),
      HICON       hIcon = ::LoadIcon(NULL,IDI_APPLICATION),
      HCURSOR     hCursor = ::LoadCursor(NULL, IDC_ARROW),
      HBRUSH      hbrBackground = ::CreateSolidBrush(0xff000000),
      HICON       hIconSm = NULL
      );		

    /** デフォルト設定 */
    void register_class();
    void create_window(HWND parent = NULL);

    void calc_client_size()
    {
      //クライアント領域の現在の幅、高さを求める
      RECT rc;
      GetClientRect( hwnd_, &rc );
      client_width_ = rc.right - rc.left;
      client_height_ = rc.bottom - rc.top;
    }
  public:
    // SetWindowLong API
    void set_long(int index,long data)
    {
      SetLastError(0);
      if(::SetWindowLongW(hwnd_,index,data) == 0)
      {
        long err = 0;
        if( (err = GetLastError()) != 0){
          SetLastError(err);
          throw sf::win32_error_exception();
        }
      };
    }

    void set_pos(
      HWND hwnd_insert_after,  // 配置順序のhandle
      int x,                 // 横方向の位置
      int y,                 // 縦方向の位置
      int cx,                // 幅
      int cy,                // 高さ
      UINT flags            // ウィンドウ位置のオプション
      )
    {
      BOOL res = SetWindowPos(hwnd_,hwnd_insert_after,x,y,cx,cy,flags);
      if(!res)
      {
        throw win32_error_exception();
      }
    }

    bool invalidate_rect(bool erase = false,const RECT * rect_ptr = 0)
    {
      return ::InvalidateRect(*this,rect_ptr,erase) == TRUE;
    }

    void enable_control(uint32_t id,bool enable)
    {
      ::EnableWindow(GetDlgItem(hwnd_,id),enable?TRUE:FALSE);
    }

    void enable_control(HWND hwnd,uint32_t id,bool enable)
    {
      ::EnableWindow(GetDlgItem(hwnd,id),enable?TRUE:FALSE);
    };

    virtual result_t window_proc(HWND hwnd,uint32_t message, WPARAM wParam, LPARAM lParam);
    virtual result_t other_window_proc(HWND hwnd,uint32_t message, WPARAM wParam, LPARAM lParam)
    {
      return proc_t::def_wnd_proc(hwnd,message,wParam,lParam);
    };

    virtual result_t on_activate(int active,bool minimized);
    // デフォルトウィンドウメッセージハンドラ
    virtual result_t on_nccreate(CREATESTRUCT *p) ;//{ return std::is_same<proc_t,wndproc>::value?1:FALSE;}
    virtual result_t on_create(CREATESTRUCT *p); //{ return std::is_same<proc_t,wndproc>::value?0:FALSE;}
    virtual result_t on_init_dialog(HWND default_focus_ctrl,LPARAM data) {return TRUE;}
    virtual result_t on_size(uint32_t flag,uint32_t width,uint32_t height);//{return std::is_same<proc_t,wndproc>::value?0:FALSE;    }
    //virtual LRESULT 
    virtual result_t on_paint();
    virtual result_t on_display_change(uint32_t bpp,uint32_t h_resolution,uint32_t v_resolution);// {         invalidate_rect();return std::is_same<proc_t,wndproc>::value?0:FALSE;}
    virtual result_t on_erase_backgroud(HDC dc) {return std::is_same<proc_t,wndproc>::value?1:TRUE;}
    virtual result_t on_hscroll(uint32_t state,uint32_t position,HWND ctrl_hwnd) {return std::is_same<proc_t,wndproc>::value?0:FALSE;}
    virtual result_t on_vscroll(uint32_t state,uint32_t position,HWND ctrl_hwnd) {return std::is_same<proc_t,wndproc>::value?0:FALSE;}
    virtual result_t on_left_mouse_button_down(uint32_t mouse_key,int x,int y ) { return std::is_same<proc_t,wndproc>::value?0:FALSE; }
    virtual result_t on_left_mouse_button_up(uint32_t mouse_key,int x,int y) { return std::is_same<proc_t,wndproc>::value?0:FALSE; }
    virtual result_t on_left_mouse_button_double_click(uint32_t mouse_key,int x,int y) { return std::is_same<proc_t,wndproc>::value?0:FALSE; }
    virtual result_t on_mouse_move(uint32_t mouse_key,int x,int y) {return std::is_same<proc_t,wndproc>::value?0:FALSE; }
    virtual result_t on_mouse_wheel(uint32_t mouse_key,int delta,int x,int y) {  return std::is_same<proc_t,wndproc>::value?0:FALSE; }
    //virtual bool on_mouse_enter(uint32_t mouse_key,int x,int y) {  return false; }
    virtual result_t on_mouse_leave() {  return std::is_same<proc_t,wndproc>::value?0:FALSE; }
    virtual result_t on_destroy(){   
      //::PostQuitMessage(0);
      return std::is_same<proc_t,wndproc>::value?0:FALSE;
    }

    virtual result_t on_close()
    {
      discard_device();
      // Windowの破棄
      BOOL ret(::DestroyWindow(hwnd_));
      BOOST_ASSERT(ret != 0);
//      return TRUE;
      return std::is_same<proc_t,wndproc>::value?1:TRUE;
    }

    virtual result_t on_set_cursor() { return std::is_same<proc_t,wndproc>::value?0:FALSE; }
    virtual result_t on_key_down(uint32_t vkey,uint32_t ext_key,uint32_t repeat);
    virtual result_t on_key_up(uint32_t vkey,uint32_t ext_key,uint32_t repeat) { return std::is_same<proc_t,wndproc>::value?0:FALSE; }
    virtual result_t on_app_command(uint32_t command,uint32_t device,uint32_t keystate) {return std::is_same<proc_t,wndproc>::value?0:FALSE;}
    virtual result_t on_command(uint32_t wparam, uint32_t lparam)  { return std::is_same<proc_t,wndproc>::value?0:FALSE; } 
    virtual result_t on_timer(uint32_t timer_id)  {
      //::InvalidateRect(hwnd_,NULL,FALSE);
      render();
      return std::is_same<proc_t,wndproc>::value?0:FALSE; 
    } 
    virtual result_t on_notify(NMHDR* nmhdr)  { return std::is_same<proc_t,wndproc>::value?0:FALSE; }
    virtual result_t on_dwm_composition_changed();
    virtual void render();
  protected:
    void get_dxgi_information();

    // Window生成後呼ばれる関数
    // WM_NCCREATEメッセージの時にthunkに切り替える
    static result_t CALLBACK start_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
      if(message == WM_NCCREATE)
      {
        LPCREATESTRUCT param = reinterpret_cast<LPCREATESTRUCT>(lParam);
        base_win32_window* ptr = reinterpret_cast<base_win32_window*>(param->lpCreateParams);
        ptr->hwnd_ = hwnd;
        // ウィンドウプロシージャをインスタンスと結び付けるthunkプロシージャに入れ替える
        LONG_PTR r = SetWindowLongPtr(hwnd,GWLP_WNDPROC,reinterpret_cast<LONG_PTR>(ptr->thunk_proc_));
        assert(r == reinterpret_cast<LONG_PTR>(&start_wnd_proc));
        return ptr->window_proc(hwnd,message,wParam,lParam);
      }
      return ::DefWindowProcW(hwnd,message,wParam,lParam);
    };

    static result_t CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
      base_win32_window* ptr = reinterpret_cast<base_win32_window*>(hwnd);
      return ptr->window_proc(ptr->hwnd_,message,wParam,lParam);
    };

    // thisとhwndをつなぐthunkクラス
    struct hwnd_this_thunk : public Xbyak::CodeGenerator {
      hwnd_this_thunk(base_win32_window* impl,typename proc_t::proc_type proc)
      {
        // rcxにhwndが格納されているので、それをimpl->hwndに保存
//        mov(ptr[&(impl->hwnd_)],rcx); // <-- エラー発生部分
		mov(rax,(size_t)&impl->hwnd_);	// <-- 訂正
        mov(ptr[rax],rcx);				// <-- 訂正
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
    }

    void init_view_matrix();

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

    ID2D1Factory1Ptr d2d_factory_;
    ID2D1Bitmap1Ptr d2d1_target_bitmap_;
    //ID2D1HwndRenderTargetPtr render_target_;
    ID2D1DevicePtr d2d_device_;
    ID2D1DeviceContextPtr d2d_context_;

    IDWriteFactory1Ptr write_factory_;
    IWICImagingFactoryPtr wic_imaging_factory_;
    IDWriteTextFormatPtr write_text_format_;

    IDXGIFactory2Ptr dxgi_factory_;
    IDXGIAdapter2Ptr dxgi_adapter_;
    IDXGIOutput1Ptr dxgi_output_;
    IDXGIDevice2Ptr dxgi_device_;
    IDXGISurface2Ptr dxgi_back_buffer_;
    DXGI_MODE_DESC1 mode_desc_;
    DXGI_SWAP_CHAIN_DESC1 swap_chain_desc_;
    DXGI_SWAP_CHAIN_FULLSCREEN_DESC swap_chain_fullscreen_desc_;
    IDXGISwapChain1Ptr dxgi_swap_chain_;
    std::wstring dxgi_info_;

    ID3D11DevicePtr d3d_device_;
    ID3D11DeviceContextPtr d3d_context_;
    ID3D11Texture2DPtr back_buffer_;

    ID3D11RenderTargetViewPtr d3d_render_target_view_;
    ID3D11Texture2DPtr d3d_depth_texture_;
    ID3D11DepthStencilViewPtr depth_view_;
    ID3D11VertexShaderPtr v_shader_;
    ID3D11InputLayoutPtr input_layout_;
    ID3D11PixelShaderPtr p_shader_;
    ID3D11BufferPtr v_buffer_;
    ID3D11BufferPtr i_buffer_;
    ID3D11BufferPtr cb_never_changes_;
    ID3D11BufferPtr cb_change_on_resize_;
    ID3D11BufferPtr cb_changes_every_frame_;
    ID3D11ShaderResourceViewPtr shader_res_view_;
    ID3D11SamplerStatePtr sampler_state_;

    
 /*   ID3D11SamplerStatePtr cube_sampler_state_;
    ID3D11Texture2DPtr cube_texture_;
    ID3D11Texture2DPtr cube_depth_texture_;
    ID3D11ShaderResourceViewPtr cube_shader_res_view_;
    ID3D11RenderTargetViewPtr cube_view_;
    ID3D11DepthStencilViewPtr cube_depth_view_;*/


	
	
    DirectX::XMMATRIX                            mat_world_;
    DirectX::XMMATRIX                            mat_view_;
    DirectX::XMMATRIX                            mat_projection_;
    DirectX::XMMATRIX                            cube_mat_projection_;
	
    DirectX::XMFLOAT4                            mesh_color_;
    float client_width_,client_height_;

    bool activate_;
    bool fullscreen_;
    timer timer_;

    // __declspec ( thread ) static std::queue<proc_t::proc_type> ptrs_ ;// thread local storage

  };

  typedef base_win32_window<> base_win32_window_t;
  typedef base_win32_window<dlgproc> base_win32_dialog_t;

  /// サブクラスウィンドウ
  struct subclass_window : public base_win32_window_t
  {
    subclass_window(HWND hwnd);
    subclass_window();
    void attach(HWND hwnd);
    void detatch();
    ~subclass_window();
    virtual result_t window_proc(HWND hwnd,uint32_t message, WPARAM wParam, LPARAM lParam) 
    {
      return CallWindowProc(proc_backup_,hwnd,message,wParam,lParam);
    };
  protected:
    bool is_subclassed_;
    WNDPROC proc_backup_;
  };

}