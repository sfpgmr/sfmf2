#include "stdafx.h"

#include "sf_windows_base.h"
#include "sf_base_win32_window2.h"
#include "exception.h"

#if _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif
using namespace std;
using namespace DirectX;

namespace sf 
{
  template <typename ProcType, typename RendererType>
  base_win32_window2<ProcType, RendererType>::~base_win32_window2()
  {

  }

  template <typename ProcType, typename RendererType> 
  base_win32_window2<ProcType, RendererType>::base_win32_window2(
    const std::wstring& title,const std::wstring& name,bool fit_to_display,float width,float height
    )
    : title_(title),name_(name),
    fit_to_display_(fit_to_display),
    width_(width),height_(height),
    thunk_(this,reinterpret_cast<ProcType::proc_type>(base_win32_window2::WndProc)),
    hwnd_(0),timer_(*this,10),
    fullscreen_(true)
  {
    width_ = dpi_.scale_x(width_);
    height_ = dpi_.scale_y(height_);
    memset(&wp_,0,sizeof(wp_));
    wp_.length = sizeof(WINDOWPLACEMENT);
    thunk_proc_ = (WNDPROC)thunk_.getCode();
  }

  template <typename ProcType, typename RendererType> 
  typename base_win32_window2<ProcType, RendererType>::result_t base_win32_window2<ProcType, RendererType>::window_proc(HWND hwnd,uint32_t message, WPARAM wParam, LPARAM lParam)
  {

    switch (message)
    {
    case WM_NCCREATE:
      return on_nccreate(reinterpret_cast<CREATESTRUCT*>(lParam));
    case WM_CREATE:
      return on_create(reinterpret_cast<CREATESTRUCT*>(lParam));
    case WM_INITDIALOG:
      return on_init_dialog(reinterpret_cast<HWND>(wParam),lParam);
    case WM_SIZE:
      return on_size(wParam,LOWORD(lParam),HIWORD(lParam)) ;
    case WM_PAINT:
      return on_paint();
    case WM_DISPLAYCHANGE:
      return on_display_change(wParam,LOWORD(lParam),HIWORD(lParam));
    case WM_ERASEBKGND:
      return on_erase_backgroud(reinterpret_cast<HDC>(wParam));
    case WM_HSCROLL:
      return on_hscroll(LOWORD(wParam),HIWORD(wParam),reinterpret_cast<HWND>(lParam));
    case WM_VSCROLL:
      return on_vscroll(LOWORD(wParam),HIWORD(wParam),reinterpret_cast<HWND>(lParam));
    case WM_LBUTTONDOWN:
      return on_left_mouse_button_down(
        wParam,dpi_.scale_x(
        GET_X_LPARAM(lParam)),dpi_.scale_y(GET_Y_LPARAM(lParam)))
        ;
      ;
    case WM_LBUTTONUP:
      return on_left_mouse_button_up(
        wParam,dpi_.scale_x(
        GET_X_LPARAM(lParam)),dpi_.scale_y(GET_Y_LPARAM(lParam)))
        ;
    case WM_LBUTTONDBLCLK:
      return on_left_mouse_button_double_click(wParam,
        dpi_.scale_x(
        GET_X_LPARAM(lParam)),dpi_.scale_y(GET_Y_LPARAM(lParam)))
        ;
    case WM_ACTIVATE:
      return on_activate(LOWORD(wParam),HIWORD(wParam) != 0);
    case WM_MOUSEMOVE:
      {
        return on_mouse_move(wParam,
          dpi_.scale_x(
          GET_X_LPARAM(lParam)),dpi_.scale_y(GET_Y_LPARAM(lParam)))
          ;
        //					on_mouse_move(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam),wParam); 
      }
    case WM_MOUSEWHEEL:
      return on_mouse_wheel(GET_KEYSTATE_WPARAM(wParam),GET_WHEEL_DELTA_WPARAM(wParam),
        dpi_.scale_x(
        GET_X_LPARAM(lParam)),dpi_.scale_y(GET_Y_LPARAM(lParam)))
        ;
    case WM_MOUSELEAVE:
      return on_mouse_leave() ;
    case WM_KEYDOWN:
      return on_key_down(wParam,lParam & 0xffff0000,LOWORD(lParam)) ;
    case WM_KEYUP:
      return on_key_up(wParam,lParam & 0xffff0000,LOWORD(lParam)) ;
    case WM_APPCOMMAND:
      return on_app_command(GET_APPCOMMAND_LPARAM(lParam),GET_DEVICE_LPARAM(lParam),GET_KEYSTATE_LPARAM(lParam));
    case WM_COMMAND:
      return on_command(wParam,lParam);
    case WM_DESTROY:
      return on_destroy();
    case WM_CLOSE:
      return on_close();
    case WM_TIMER:
      return on_timer(wParam);
    case WM_NOTIFY:
      return on_notify(reinterpret_cast<NMHDR*>(lParam));
    case WM_DWMCOMPOSITIONCHANGED:
      return on_dwm_composition_changed();
      //case WM_DWMCOLORIZATIONCOLORCHANGED:
      //  return on_dwm_colorlizationcolor_changed
    }

    // 他のWindowメッセージを派生クラス側でフックできるようにする
    return other_window_proc(hwnd,message,wParam,lParam);

  };

  template <typename ProcType, typename RendererType> 
  void base_win32_window2<ProcType, RendererType>::register_class (
    const wchar_t * menu_name,
    uint32_t        style ,
    int32_t     cbClsExtra,
    int32_t   cbWndExtra,
    HICON       hIcon ,
    HCURSOR     hCursor,
    HBRUSH      hbrBackground ,
    HICON       hIconSm
    )		
  {
    wnd_class_.reset(new sf::window_class_ex(menu_name,name_,HINST_THISCOMPONENT,&start_wnd_proc,style,cbClsExtra,cbWndExtra,hIcon,hCursor,hbrBackground,hIconSm));
  }

  /** デフォルト設定 */
  template <typename ProcType, typename RendererType> 
  void base_win32_window2<ProcType, RendererType>::register_class()
  {
    //register_class(0,0);
    wnd_class_.reset(new sf::window_class_ex(0,name_,HINST_THISCOMPONENT,&start_wnd_proc));
  }

  template <typename ProcType, typename RendererType> 
  void base_win32_window2<ProcType, RendererType>::create_window(HWND parent)
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
    ::GetWindowPlacement(hwnd_,&wp_);
  }

  template <typename ProcType, typename RendererType>
  typename base_win32_window2<ProcType, RendererType>::result_t base_win32_window2<ProcType, RendererType>::on_key_down(uint32_t vkey,uint32_t ext_key,uint32_t repeat) 
  {
    if(vkey == VK_ESCAPE)
    {
      PostMessage( hwnd_, WM_CLOSE, 0, 0 );
    }
    return std::is_same<proc_t,wndproc>::value?0:FALSE; 
  }

  template <typename ProcType, typename RendererType> 
  void base_win32_window2<ProcType, RendererType>::update() {::UpdateWindow(hwnd_);}


  template <typename ProcType, typename RendererType> 
  typename base_win32_window2<ProcType, RendererType>::result_t base_win32_window2<ProcType, RendererType>::on_nccreate(CREATESTRUCT *p)
  {

    return std::is_same<proc_t,wndproc>::value?1:FALSE;
  }

  template <typename ProcType, typename RendererType>
  typename base_win32_window2<ProcType, RendererType>::result_t  base_win32_window2<ProcType, RendererType>::on_size(uint32_t flag,uint32_t width,uint32_t height)
  {
    if(init_ && !(width == 0 || height == 0))
    {
      update_window_size();
      calc_client_size();
    }
    return std::is_same<proc_t,wndproc>::value?0:FALSE;  
  }

  template <typename ProcType, typename RendererType>
  typename base_win32_window2<ProcType, RendererType>::result_t base_win32_window2<ProcType, RendererType>::on_create(CREATESTRUCT *p)
  {
    // ウィンドウ全体を半透明にする
    //SetLayeredWindowAttributes(
    //  hwnd_,
    //  RGB(0,0,0), // color key
    //  100, // alpha value
    //  LWA_ALPHA | LWA_COLORKEY);

    // ウィンドウの指定領域を半透明にする
    // リージョンの設定
    //HRGN rgn = CreateRectRgn(0, 0, width_, height_);
    //SetWindowRgn(hwnd_, rgn, FALSE);
    renderer_.reset(new renderer_t(*this));
    timer_.start();
    show();
    text(title_);
    return std::is_same<proc_t,wndproc>::value?0:FALSE;
  }

  template <typename ProcType, typename RendererType>
  typename base_win32_window2<ProcType, RendererType>::result_t base_win32_window2<ProcType, RendererType>::on_paint() 
  {
    sf::paint_struct p(hwnd_);
    //  gdi_object<HBRUSH> brush(::CreateSolidBrush(RGB(255,0,0)));
    //  {
    //    FillRect(p->hdc,&p->rcPaint,brush);
    //  }
    //}
    renderer_->render();
    return  std::is_same<proc_t,wndproc>::value?0:FALSE;
  }

  template <typename ProcType, typename RendererType>
  typename base_win32_window2<ProcType, RendererType>::result_t base_win32_window2<ProcType, RendererType>::on_dwm_composition_changed()
  {
    return  std::is_same<proc_t,wndproc>::value?0:FALSE;
  }

  template <typename ProcType, typename RendererType>
  typename base_win32_window2<ProcType, RendererType>::result_t base_win32_window2<ProcType, RendererType>::on_activate(int active,bool minimized)
  {
    activate_ = (active != 0);
    if(activate_ && init_)
    {
      renderer_->restore_swapchain_and_dependent_resources();
    }
    return std::is_same<proc_t,wndproc>::value?0:FALSE;
  }

  template <typename ProcType, typename RendererType>
  typename base_win32_window2<ProcType, RendererType>::result_t base_win32_window2<ProcType, RendererType>::on_display_change(uint32_t bpp,uint32_t h_resolution,uint32_t v_resolution) 
  {
    invalidate_rect();
    return std::is_same<proc_t,wndproc>::value?0:FALSE;
  }


  template struct base_win32_window2<wndproc>;
  template struct base_win32_window2<dlgproc>;

}

