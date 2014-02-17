#pragma once
/*
*/
// Windows Header Files:
#include "sf_windows.h"
#include "icon.h"
#include "base_window.h"
#include "taskbar.h"

#define WM_PLAY_STOP WM_USER + 1
#define WM_PLAY_PAUSE WM_USER + 2
#define WM_PLAY_PLAY WM_USER + 3

namespace sf
{

  struct fluidcs11_window;
  typedef std::shared_ptr<fluidcs11_window> fluidcs11_window_ptr;

  /** fluidcs11_window を生成する関数 */
  fluidcs11_window_ptr create_fluidcs11_window (
    const std::wstring& menu_name,
    const std::wstring& name,
    const uint32_t show_flag = SW_SHOW,
    bool fit_to_display = false,
    float width = 640,
    float height = 480
    );
  /** fluidcs11_window を生成する関数 */
  void dialogbox (
    const std::wstring& menu_name,
    const std::wstring& name
    );

  /** toplevel ウィンドウクラス */
  /* このクラスは、create_dcltoplevel_window 関数からのみ生成可能 */
  struct fluidcs11_window : public base_window
  {

    friend   fluidcs11_window_ptr create_fluidcs11_window
      (
      const std::wstring& menu_name,
      const std::wstring& name,
      const uint32_t show_flag,
      bool fit_to_display,
      float width ,
      float height
      );

    friend void dialogbox (
      const std::wstring& menu_name,
      const std::wstring& name
    );

    ~fluidcs11_window(){};
 
    void * raw_handle() const;
    void create();
    void fluidcs11_window::show();
    bool fluidcs11_window::is_show();
    void fluidcs11_window::hide();
    virtual bool is_activate() override;
    virtual float width() override;
    virtual float height() override;
    virtual sf::dpi& dpi() override;
    virtual bool is_fullscreen() override;
    //void message_box(const std::wstring& text,const std::wstring& caption,uint32_t type = MB_OK);
    void text(std::wstring& text);
    //std::wstring text()
    void update();
    void render();
    void video_bitmap(ID2D1Bitmap1Ptr & bitmap);
    virtual base_window::closed_t& closed() override;
  private:
    struct impl;
    fluidcs11_window(const std::wstring& menu_name,const std::wstring& name,bool fit_to_display,float width = 800 ,float height = 600);
    // 実装部
    std::shared_ptr<impl> impl_;
  };
}