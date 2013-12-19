﻿/*
==============================================================================

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
/* ToDo

TODO: リサイズに対応する

*/

#include "stdafx.h"
#include "resource.h"
#define BOOST_ASSIGN_MAX_PARAMS 7
#include <boost/assign.hpp>
#include <boost/assign/ptr_list_of.hpp>
#include <boost/assign/ptr_list_inserter.hpp>
#include <boost/foreach.hpp>

#include "sf_windows.h"
#include "dcomposition_window.h"
#include "CommDlg.h"
#include "icon.h"
#include "timer.h"
#include "exception.h"
#include "application.h"
#include "config_tab_dialog.h"
#include "info_tab_dialog.h"
#include "midi_config_tab_dialog.h"

#if _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

namespace sf 
{


  struct simple_vertex
  {
    DirectX::XMFLOAT3 pos;
    DirectX::XMFLOAT3 norm;
    DirectX::XMFLOAT2 tex;
  };

  struct cb_never_changes
  {
    DirectX::XMMATRIX mView;
    DirectX::XMFLOAT4 vLightDir;
  };

  struct cb_change_on_resize
  {
    DirectX::XMMATRIX mProjection;
  };

  struct cb_changes_every_frame
  {
    DirectX::XMMATRIX mWorld;
    DirectX::XMFLOAT4 vLightColor;

    //    DirectX::XMFLOAT4 vMeshColor;
  };

  struct dcomposition_window::impl : public base_win32_window2_t
  {

    impl(const std::wstring& menu_name,const std::wstring& name,bool fit_to_display,float width = 160,float height = 100) 
      : base_win32_window2_t(menu_name,name,fit_to_display,width,height) 
      , timer_(*this,100)
      , icon_(IDI_ICON1)
      /*,mesh_color_(0.7f, 0.7f, 0.7f, 1.0f)*/
      ,init_(false)
      ,thumb_start_(false)
    {
      fullscreen_ = false;
      //on_render.connect(boost::bind(&impl::render,this));
    };

    ~impl(){
      //safe_release(dxgi_factory_);
    };


    // -------------------------------------------------------------------
    // ウィンドウプロシージャ
    // -------------------------------------------------------------------

    virtual void create(){
      //create_device_independent_resources();
      //    icon_ = ::LoadIconW(HINST_THISCOMPONENT,MAKEINTRESOURCE(IDI_ICON1));
      register_class(this->name_.c_str(),CS_HREDRAW | CS_VREDRAW ,0,0,icon_.get());
      create_window();
      text(name_);

      // 半透明ウィンドウを有効にする。
      //BOOL dwmEnable;
      //DwmIsCompositionEnabled (&dwmEnable); 
      //if (dwmEnable) EnableBlurBehind(*this);

    }

    void calc_client_size()
    {
      //クライアント領域の現在の幅、高さを求める
      RECT rc;
      GetClientRect( hwnd_, &rc );
      client_width_ = rc.right - rc.left;
      client_height_ = rc.bottom - rc.top;
    }

    LRESULT other_window_proc(HWND hwnd,uint32_t message, WPARAM wParam, LPARAM lParam)
    {
      return DefWindowProcW(hwnd,message,wParam,lParam);
    };

    virtual void create_device() 
    {
     // renderer_->create_device();
    }

    virtual void discard_device()
    {
      renderer_->discard_device();
      discard_dcomp_device();
    }

    void discard_dcomp_device()
    {
      //dcomp_device_.Reset();
    }

    void render()
    {

      renderer_->render();
    }


    void on_player_event(HWND wnd,UINT_PTR wParam)
    {
      //    application::instance()->Player()->HandleEvent(wParam);
    }

    //sf::base_win32_window_t::result_t on_create(CREATESTRUCT *p)
    //{
    //  //create_device();
    //  //timer_.start();
    //  return 0;
    //}


    LRESULT on_size(uint32_t flag,uint32_t width,uint32_t height)
    {
      // バックバッファなどに関係するインターフェースを解放する
      // バックバッファを解放する
      if(init_)
      {
        int height = client_height_;
        int width = client_width_;

        calc_client_size();

      }
      // バックバッファなどに関係するインターフェースを再作成する

      //if (render_target_)
      //{
      //	D2D1_SIZE_U size;
      //	size.width = lParam & 0xFFFF;
      //	size.height = (lParam >> 16) & 0xFFFF; ;

      //	// Note: This method can fail, but it's okay to ignore the
      //	// error here -- it will be repeated on the next call to
      //	// EndDraw.
      //	render_target_->Resize(size);
      //}
      return TRUE;
    }

    LRESULT on_display_change(uint32_t bpp,uint32_t h_resolution,uint32_t v_resolution)
    {
      invalidate_rect();
      return TRUE;
    }

    LRESULT on_erase_backgroud(HDC dc)
    {
      return FALSE;
    }

    LRESULT on_hscroll(uint32_t state,uint32_t position,HWND ctrl_hwnd)
    {
      return FALSE;
    }

    LRESULT on_destroy()
    {
      ::PostQuitMessage(0);
      return FALSE;
    }

    LRESULT on_close()
    {
      //slider_.detatch();
      timer_.player_stop();
      // 後始末
      discard_device();
      // レンダーターゲットのリリース
      //safe_release(dcr_);
      //      safe_release(render_target_);
      // Windowの破棄
      renderer_->discard_device_independant_resources();
      BOOL ret(::DestroyWindow(hwnd_));
      BOOST_ASSERT(ret != 0);
      return TRUE;
    }

    LRESULT on_command(uint32_t wparam, uint32_t lparam)
    {
      return FALSE;
    }

    LRESULT on_timer(uint32_t timer_id)
    {
      // TODO:スレッドのエラーチェックも入れておく
      //update();
      //InvalidateRect(hwnd_,NULL,FALSE);
    /*  static float angle = 0.0f;
      rot_->SetAngle(angle);
      rot_child_->SetAngle(360.0f - angle);
      angle += 10.0f;

      if(angle > 360.0f) angle -= 360.0f*/;

      //static float scale = 1.0f;
      //static float scale_add = 0.1f;

      //scale += scale_add;

      //if(scale > 10.0f) {
      //  scale  = 10.0f;
      //  scale_add = -scale_add;
      //} 

      //if(scale < 0.1f)
      //{
      //  scale = 0.1f;
      //  scale_add = -scale_add;
      //}

      //scale_->SetScaleX(scale);
      //scale_->SetScaleY(scale);

      //dcomp_device_->Commit();

      invalidate_rect();
      return TRUE;
    }

  private:

    timer timer_;
    bool thumb_start_;
    //slider slider_;

    float client_width_,client_height_;

    //IDXGISwapChainPtr dxgi_swap_chain_;
    // std::wstring dxgi_info_;

    icon icon_;
    bool init_;

    D2D1_SIZE_U icon_size_;
    IDCompositionDevicePtr dcomp_device_;
    IDCompositionTargetPtr dcomp_target_;
    IDCompositionRotateTransformPtr rot_,rot_child_;
    IDCompositionScaleTransformPtr scale_;
    IDCompositionTranslateTransformPtr trans_;

    // thisとhwndをつなぐthunkクラス
    // メンバー関数を直接呼び出す。
    struct hwnd_this_thunk2 : public Xbyak::CodeGenerator {
      hwnd_this_thunk2(LONG_PTR this_addr,const void * proc)
      {
        // 引数の位置をひとつ後ろにずらす
        mov(r10,r9);
        mov(r9,r8);
        mov(r8,rdx);
        mov(rdx,rcx);
        // thisのアドレスをrcxに格納する
        mov(rcx,(LONG_PTR)this_addr);
        // 第5引数をスタックに格納
        push(r10);
        sub(rsp,32);
        mov(r10,(LONG_PTR)proc);
        // メンバ関数呼び出し
        call(r10);
        add(rsp,40);
        ret(0);
      }
    };

    //hwnd_this_thunk2 thunk_info_;
    //  hwnd_this_thunk2 thunk_config_;
    //proc_t proc_info_;
    //  proc_t proc_config_;

  };

  dcomposition_window::dcomposition_window(const std::wstring& menu_name,const std::wstring& name,bool fit_to_display,float width ,float height)
    : impl_(new impl(menu_name,name,fit_to_display,width,height))
  {

  };

  void * dcomposition_window::raw_handle() const {return impl_->raw_handle();};
  void dcomposition_window::create(){impl_->create();};
  void dcomposition_window::show(){impl_->show();};
  void dcomposition_window::hide(){impl_->hide();};
  bool dcomposition_window::is_activate() { return impl_->is_activate(); }
  float dcomposition_window::width() { return impl_->width(); }
  float dcomposition_window::height() { return impl_->height(); }
  sf::dpi& dcomposition_window::dpi() { return impl_->dpi(); }
  bool dcomposition_window::is_fullscreen() { return impl_->is_fullscreen(); }


  bool dcomposition_window::is_show(){return impl_->is_show();};
  void dcomposition_window::text(std::wstring& text){impl_->text(text);};
//  void dcomposition_window::message_box(const std::wstring& text,const std::wstring& caption,uint32_t type )
//  {
 //   message_box(text,caption,type);
 // };
  void dcomposition_window::update(){impl_->update();};
  void dcomposition_window::render(){impl_->render();};
  // void dcomposition_window::player_ready(){impl_->player_ready();};


  dcomposition_window_ptr create_dcomposition_window
    (
    const std::wstring& menu_name,
    const std::wstring& name,
    const uint32_t show_flag,
    bool fit_to_display,
    float width,
    float height
    )
  {
    // クライアント領域のサイズからウィンドウサイズを設定
    RECT    rect    = { 0, 0, width, height };
    //::AdjustWindowRectEx( &rect, WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME, FALSE, WS_EX_APPWINDOW | WS_EX_TOPMOST );
    dcomposition_window* p = new dcomposition_window(menu_name,name,fit_to_display,rect.right - rect.left,rect.bottom - rect.top);
    p->create();
    p->show();
    p->update();
    return dcomposition_window_ptr(p);
  }

}

