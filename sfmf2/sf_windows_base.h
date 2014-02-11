#pragma once
/*
*/
// Windows Header Files:
#define XBYAK64
#include "exception.h"
#include "base_window.h"
#include "dpi.h"
#include "xbyak.h"
#include "windows.h"
#include "windowsx.h"
#include "CommCtrl.h"
#include <type_traits>
#include "timer.h"
//#include <boost/type_traits/is_same.hpp>

//#include "input.h"


// Direct Write

_WRL_PTR_TYPEDEF(IDWriteFactory);
_WRL_PTR_TYPEDEF(IDWriteFactory1);
_WRL_PTR_TYPEDEF(IDWriteFactory2);
_WRL_PTR_TYPEDEF(IDWriteGdiInterop);
_WRL_PTR_TYPEDEF(IDWriteFontFace1);
_WRL_PTR_TYPEDEF(IDWriteFontFace);
_WRL_PTR_TYPEDEF(IDWriteFont1);
_WRL_PTR_TYPEDEF(IDWriteFont);
_WRL_PTR_TYPEDEF(IDWriteFontFamily);
_WRL_PTR_TYPEDEF(IDWriteFontCollection);
_WRL_PTR_TYPEDEF(IDWriteLocalizedStrings);
_WRL_PTR_TYPEDEF(IDWriteTextFormat);
_WRL_PTR_TYPEDEF(IDWriteTextAnalyzer1);
_WRL_PTR_TYPEDEF(IDWriteTextLayout);
_WRL_PTR_TYPEDEF(IDWriteTextLayout1);
_WRL_PTR_TYPEDEF(IDWriteFontFile);
//_WRL_PTR_TYPEDEF(IDWriteFontFile);

// Direct2D

_WRL_PTR_TYPEDEF(ID2D1AnalysisTransform);
_WRL_PTR_TYPEDEF(ID2D1Bitmap);
_WRL_PTR_TYPEDEF(ID2D1Bitmap1);
_WRL_PTR_TYPEDEF(ID2D1BitmapBrush);
_WRL_PTR_TYPEDEF(ID2D1BitmapBrush1);
_WRL_PTR_TYPEDEF(ID2D1BitmapRenderTarget);
_WRL_PTR_TYPEDEF(ID2D1BlendTransform);
_WRL_PTR_TYPEDEF(ID2D1BorderTransform);
_WRL_PTR_TYPEDEF(ID2D1BoundsAdjustmentTransform);
_WRL_PTR_TYPEDEF(ID2D1Brush);
_WRL_PTR_TYPEDEF(ID2D1ColorContext);
_WRL_PTR_TYPEDEF(ID2D1CommandList);
_WRL_PTR_TYPEDEF(ID2D1CommandSink);
_WRL_PTR_TYPEDEF(ID2D1ComputeInfo);
_WRL_PTR_TYPEDEF(ID2D1ComputeTransform);
_WRL_PTR_TYPEDEF(ID2D1ConcreteTransform);
_WRL_PTR_TYPEDEF(ID2D1DCRenderTarget);
_WRL_PTR_TYPEDEF(ID2D1Device);
_WRL_PTR_TYPEDEF(ID2D1Device1);
_WRL_PTR_TYPEDEF(ID2D1DeviceContext);
_WRL_PTR_TYPEDEF(ID2D1DeviceContext1);
_WRL_PTR_TYPEDEF(ID2D1DrawInfo);
_WRL_PTR_TYPEDEF(ID2D1DrawingStateBlock);
_WRL_PTR_TYPEDEF(ID2D1DrawingStateBlock1);
_WRL_PTR_TYPEDEF(ID2D1DrawTransform);
_WRL_PTR_TYPEDEF(ID2D1Effect);
_WRL_PTR_TYPEDEF(ID2D1EffectContext);
_WRL_PTR_TYPEDEF(ID2D1EffectImpl);
_WRL_PTR_TYPEDEF(ID2D1EllipseGeometry);
_WRL_PTR_TYPEDEF(ID2D1Factory);
_WRL_PTR_TYPEDEF(ID2D1Factory1);
_WRL_PTR_TYPEDEF(ID2D1Factory2);
_WRL_PTR_TYPEDEF(ID2D1GdiInteropRenderTarget);
_WRL_PTR_TYPEDEF(ID2D1GdiMetafile);
_WRL_PTR_TYPEDEF(ID2D1GdiMetafileSink);
_WRL_PTR_TYPEDEF(ID2D1Geometry);
_WRL_PTR_TYPEDEF(ID2D1GeometryGroup);
_WRL_PTR_TYPEDEF(ID2D1GeometrySink);
_WRL_PTR_TYPEDEF(ID2D1GradientStopCollection);
_WRL_PTR_TYPEDEF(ID2D1GradientStopCollection1);
_WRL_PTR_TYPEDEF(ID2D1HwndRenderTarget);
_WRL_PTR_TYPEDEF(ID2D1Image);
_WRL_PTR_TYPEDEF(ID2D1ImageBrush);
_WRL_PTR_TYPEDEF(ID2D1Layer);
_WRL_PTR_TYPEDEF(ID2D1LinearGradientBrush);
_WRL_PTR_TYPEDEF(ID2D1Mesh);
//_WRL_PTR_TYPEDEF(ID2D1MultiThread);
_WRL_PTR_TYPEDEF(ID2D1OffsetTransform);
_WRL_PTR_TYPEDEF(ID2D1PathGeometry);
_WRL_PTR_TYPEDEF(ID2D1PathGeometry1);
_WRL_PTR_TYPEDEF(ID2D1PrintControl);
_WRL_PTR_TYPEDEF(ID2D1Properties);
_WRL_PTR_TYPEDEF(ID2D1RadialGradientBrush);
_WRL_PTR_TYPEDEF(ID2D1RectangleGeometry);
_WRL_PTR_TYPEDEF(ID2D1RenderInfo);
_WRL_PTR_TYPEDEF(ID2D1RenderTarget);
_WRL_PTR_TYPEDEF(ID2D1Resource);
_WRL_PTR_TYPEDEF(ID2D1ResourceTexture);
_WRL_PTR_TYPEDEF(ID2D1RoundedRectangleGeometry);
_WRL_PTR_TYPEDEF(ID2D1SimplifiedGeometrySink);
_WRL_PTR_TYPEDEF(ID2D1SolidColorBrush);
_WRL_PTR_TYPEDEF(ID2D1SourceTransform);
_WRL_PTR_TYPEDEF(ID2D1StrokeStyle);
_WRL_PTR_TYPEDEF(ID2D1StrokeStyle1);
_WRL_PTR_TYPEDEF(ID2D1TessellationSink);
_WRL_PTR_TYPEDEF(ID2D1Transform);
_WRL_PTR_TYPEDEF(ID2D1TransformedGeometry);
_WRL_PTR_TYPEDEF(ID2D1TransformGraph);
_WRL_PTR_TYPEDEF(ID2D1TransformNode);
_WRL_PTR_TYPEDEF(ID2D1VertexBuffer);

// WIC

_WRL_PTR_TYPEDEF(IWICImagingFactory);
_WRL_PTR_TYPEDEF(IWICImagingFactory2);
_WRL_PTR_TYPEDEF(IWICBitmapDecoder);
_WRL_PTR_TYPEDEF(IWICBitmapFrameDecode);
_WRL_PTR_TYPEDEF(IWICStream);
_WRL_PTR_TYPEDEF(IWICFormatConverter);
_WRL_PTR_TYPEDEF(IWICBitmapScaler);
_WRL_PTR_TYPEDEF(ITaskbarList3);

// DXGI 

_WRL_PTR_TYPEDEF(IDXGIAdapter);
_WRL_PTR_TYPEDEF(IDXGIAdapter1);
_WRL_PTR_TYPEDEF(IDXGIAdapter2);
_WRL_PTR_TYPEDEF(IDXGIDebug);
_WRL_PTR_TYPEDEF(IDXGIDevice);
_WRL_PTR_TYPEDEF(IDXGIDevice1);
_WRL_PTR_TYPEDEF(IDXGIDevice2);
_WRL_PTR_TYPEDEF(IDXGIDevice3);
_WRL_PTR_TYPEDEF(IDXGIDeviceSubObject);
_WRL_PTR_TYPEDEF(IDXGIDisplayControl);
_WRL_PTR_TYPEDEF(IDXGIFactory);
_WRL_PTR_TYPEDEF(IDXGIFactory1);
_WRL_PTR_TYPEDEF(IDXGIFactory2);
_WRL_PTR_TYPEDEF(IDXGIFactory3);
_WRL_PTR_TYPEDEF(IDXGIInfoQueue);
_WRL_PTR_TYPEDEF(IDXGIKeyedMutex);
_WRL_PTR_TYPEDEF(IDXGIObject);
_WRL_PTR_TYPEDEF(IDXGIOutput);
_WRL_PTR_TYPEDEF(IDXGIOutput1);
_WRL_PTR_TYPEDEF(IDXGIOutput2);
_WRL_PTR_TYPEDEF(IDXGIOutputDuplication);
_WRL_PTR_TYPEDEF(IDXGIResource);
_WRL_PTR_TYPEDEF(IDXGIResource1);
_WRL_PTR_TYPEDEF(IDXGISurface);
_WRL_PTR_TYPEDEF(IDXGISurface1);
_WRL_PTR_TYPEDEF(IDXGISurface2);
_WRL_PTR_TYPEDEF(IDXGISwapChain);
_WRL_PTR_TYPEDEF(IDXGISwapChain1);

// Direct3D

_WRL_PTR_TYPEDEF(ID3D11Asynchronous);
_WRL_PTR_TYPEDEF(ID3D11BlendState);
_WRL_PTR_TYPEDEF(ID3D11BlendState1);
_WRL_PTR_TYPEDEF(ID3D11CommandList);
_WRL_PTR_TYPEDEF(ID3D11Counter);
_WRL_PTR_TYPEDEF(ID3D11DepthStencilState);
_WRL_PTR_TYPEDEF(ID3D11Device);
_WRL_PTR_TYPEDEF(ID3D11Device1);
_WRL_PTR_TYPEDEF(ID3D11Device2);
_WRL_PTR_TYPEDEF(ID3D11DeviceChild);
_WRL_PTR_TYPEDEF(ID3D11DeviceContext);
_WRL_PTR_TYPEDEF(ID3D11DeviceContext1);
_WRL_PTR_TYPEDEF(ID3D11DeviceContext2);
_WRL_PTR_TYPEDEF(ID3DDeviceContextState);
_WRL_PTR_TYPEDEF(ID3D11InputLayout);
_WRL_PTR_TYPEDEF(ID3D11Predicate);
_WRL_PTR_TYPEDEF(ID3D11Query);
_WRL_PTR_TYPEDEF(ID3D11RasterizerState);
_WRL_PTR_TYPEDEF(ID3D11RasterizerState1);
_WRL_PTR_TYPEDEF(ID3D11SamplerState);
_WRL_PTR_TYPEDEF(ID3D11Debug);
_WRL_PTR_TYPEDEF(ID3D11InfoQueue);
_WRL_PTR_TYPEDEF(ID3D11RefDefaultTrackingOptions);
_WRL_PTR_TYPEDEF(ID3D11RefTrackingOptions);
_WRL_PTR_TYPEDEF(ID3D11SwitchToRef);
_WRL_PTR_TYPEDEF(ID3D11TracingDevice);
_WRL_PTR_TYPEDEF(ID3D11Buffer);
_WRL_PTR_TYPEDEF(ID3D11DepthStencilView);
_WRL_PTR_TYPEDEF(ID3D11RenderTargetView);
_WRL_PTR_TYPEDEF(ID3D11Resource);
_WRL_PTR_TYPEDEF(ID3D11ShaderResourceView);
_WRL_PTR_TYPEDEF(ID3D11Texture1D);
_WRL_PTR_TYPEDEF(ID3D11Texture2D);
_WRL_PTR_TYPEDEF(ID3D11Texture3D);
_WRL_PTR_TYPEDEF(ID3D11UnorderedAccessView);
_WRL_PTR_TYPEDEF(ID3D11View);
_WRL_PTR_TYPEDEF(ID3D11ClassInstance);
_WRL_PTR_TYPEDEF(ID3D11ClassLinkage);
_WRL_PTR_TYPEDEF(ID3D11ComputeShader);
_WRL_PTR_TYPEDEF(ID3D11DomainShader);
_WRL_PTR_TYPEDEF(ID3D11GeometryShader);
_WRL_PTR_TYPEDEF(ID3D11HullShader);
_WRL_PTR_TYPEDEF(ID3D11PixelShader);
_WRL_PTR_TYPEDEF(ID3D11ShaderReflection);
_WRL_PTR_TYPEDEF(ID3D11ShaderReflectionConstantBuffer);
_WRL_PTR_TYPEDEF(ID3D11ShaderReflectionType);
_WRL_PTR_TYPEDEF(ID3D11ShaderReflectionVariable);
_WRL_PTR_TYPEDEF(ID3D11ShaderTrace);
_WRL_PTR_TYPEDEF(ID3D11ShaderTraceFactory);
_WRL_PTR_TYPEDEF(ID3D11VertexShader);
_WRL_PTR_TYPEDEF(ID3DBlob);
_WRL_PTR_TYPEDEF(ID3DInclude);
_WRL_PTR_TYPEDEF(ID3DUserDefinedAnnotation);

// DirectComposition

_WRL_PTR_TYPEDEF(IDCompositionAnimation);
_WRL_PTR_TYPEDEF(IDCompositionClip);
_WRL_PTR_TYPEDEF(IDCompositionDevice);
_WRL_PTR_TYPEDEF(IDCompositionDevice2);
_WRL_PTR_TYPEDEF(IDCompositionDeviceDebug);
_WRL_PTR_TYPEDEF(IDCompositionDesktopDevice);
_WRL_PTR_TYPEDEF(IDCompositionEffect);
_WRL_PTR_TYPEDEF(IDCompositionEffectGroup);
_WRL_PTR_TYPEDEF(IDCompositionMatrixTransform);
_WRL_PTR_TYPEDEF(IDCompositionMatrixTransform3D);
_WRL_PTR_TYPEDEF(IDCompositionRectangleClip);
_WRL_PTR_TYPEDEF(IDCompositionRotateTransform);
_WRL_PTR_TYPEDEF(IDCompositionRotateTransform3D);
_WRL_PTR_TYPEDEF(IDCompositionScaleTransform);
_WRL_PTR_TYPEDEF(IDCompositionScaleTransform3D);
_WRL_PTR_TYPEDEF(IDCompositionSkewTransform);
_WRL_PTR_TYPEDEF(IDCompositionSurface);
_WRL_PTR_TYPEDEF(IDCompositionSurfaceFactory);
_WRL_PTR_TYPEDEF(IDCompositionTarget);
_WRL_PTR_TYPEDEF(IDCompositionTransform);
_WRL_PTR_TYPEDEF(IDCompositionTransform3D);
_WRL_PTR_TYPEDEF(IDCompositionTranslateTransform);
_WRL_PTR_TYPEDEF(IDCompositionTranslateTransform3D);
_WRL_PTR_TYPEDEF(IDCompositionVirtualSurface);
_WRL_PTR_TYPEDEF(IDCompositionVisual);
_WRL_PTR_TYPEDEF(IDCompositionVisual2);



namespace sf{

  /* inline template <class Exc = win32_error_exception> void throw_if_err<>()(HRESULT hr)
  {
  if(hr != S_OK){throw Exc(hr);}
  };*/


  ID2D1BitmapPtr load_bitmap_from_file(
    ID2D1HwndRenderTargetPtr render_target,
    IWICImagingFactoryPtr wic_factory,
    std::wstring uri,
    uint32_t destination_width = 0,
    uint32_t destination_height = 0
    );

  /** WNDCLASSEXラッパクラス */
  struct window_class_ex
  {
    window_class_ex(
      const wchar_t*  menu_name ,
      const std::wstring&  class_name ,
      HINSTANCE   hInstance = NULL,
      WNDPROC     lpfnWndProc = ::DefWindowProcW,
      uint32_t        style = CS_HREDRAW | CS_VREDRAW,
      int32_t     cbClsExtra  = 0,
      int32_t     cbWndExtra = sizeof(LONG_PTR), 
      HICON       hIcon = ::LoadIcon(NULL,IDI_APPLICATION),
      HCURSOR     hCursor = ::LoadCursor(NULL, IDC_ARROW),
      HBRUSH      hbrBackground = ::CreateSolidBrush(0xff000000),
      HICON       hIconSm = NULL
      ) : is_register_(false)
    {

      if(::GetClassInfoExW(hInstance,class_name.c_str(),&wndclass_) == 0)
      {
        if(::GetLastError() == ERROR_CLASS_DOES_NOT_EXIST)
        {
          OutputDebugStringW(class_name.c_str());
          ::ZeroMemory(&wndclass_,sizeof(wndclass_));
          wndclass_.lpszMenuName = (LPCWSTR)menu_name;
          wndclass_.lpszClassName = class_name.c_str();
          wndclass_.cbSize = sizeof(::WNDCLASSEXW);
          wndclass_.cbWndExtra = cbWndExtra;
          wndclass_.hInstance = hInstance;
          wndclass_.lpfnWndProc = lpfnWndProc;
          wndclass_.style = style;
          wndclass_.cbClsExtra = cbClsExtra;
          wndclass_.hIcon = hIcon;
          wndclass_.hCursor = hCursor;
          wndclass_.hbrBackground = hbrBackground;
          wndclass_.hIconSm = hIconSm;
          atom_ = ::RegisterClassExW(&wndclass_) ;
          BOOST_ASSERT(atom_ != 0);
          is_register_ = true;
          //OutputDebugStringW((boost::wformat(L"~~~~~~ %s class register sucess!! ~~~~~~~~~~") % class_name.c_str()).str().c_str());
        } else {
          throw win32_error_exception();
        }
      } else {
        is_register_ = true;
      }
    };

    ~window_class_ex()
    {
      if(is_register_){
        ::UnregisterClassW(wndclass_.lpszClassName,wndclass_.hInstance);
      }
    }

  private:
    bool is_register_;
    ATOM atom_;
    ::WNDCLASSEXW wndclass_;
  };

  struct get_dc {
    get_dc(HWND hwnd) : hwnd_(hwnd),hdc_(GetDC(hwnd)) {}
    HDC get(){return hdc_;}
    ~get_dc(){::ReleaseDC(hwnd_,hdc_);}
  private:
    HDC hdc_;
    HWND hwnd_;
  };

  struct get_window_dc {
    get_window_dc(HWND hwnd) : hwnd_(hwnd),hdc_(GetWindowDC(hwnd)) {}
    HDC get(){return hdc_;}
    ~get_window_dc(){::ReleaseDC(hwnd_,hdc_);}
  private:
    HDC hdc_;
    HWND hwnd_;
  };

  struct compatible_dc {
    compatible_dc(HDC hdc) : hdc_(::CreateCompatibleDC(hdc)){}; 
    ~compatible_dc(){::DeleteDC(hdc_);};
    HDC get() { return hdc_;};
  private:
    HDC hdc_;
  };

  struct ref_dc {
    ref_dc(HDC& hdc) : hdc_(hdc) {};
    ~ref_dc(){};
    HDC get() { return hdc_;};
  private:
    HDC& hdc_;
  };

  struct d2_dc {
    d2_dc(ID2D1GdiInteropRenderTargetPtr& ptr,D2D1_DC_INITIALIZE_MODE mode) :hdc_(0),ptr_(ptr)
    {
      hr_ = ptr->GetDC(mode,&hdc_);
    };
    ~d2_dc(){ptr_->ReleaseDC(NULL);};
    HDC get() { return hdc_;};
  private:
    HRESULT hr_;
    HDC hdc_;
    ID2D1GdiInteropRenderTargetPtr& ptr_;
  };

  template <typename Holder>
  struct device_context
  {
    explicit device_context(Holder* holder) : holder_(holder){};
    ~device_context() {}
    operator HDC(){return holder_->get();}
  private:
    std::unique_ptr<Holder> holder_;
  };

  //struct handle_holder : boost::noncopyable
  //{
  //  explicit handle_holder(HANDLE handle) : handle_(handle) {};
  //  ~handle_holder(){if(handle_) ::CloseHandle(handle_);}
  //  operator HANDLE(){return handle_;}
  //private:
  //  HANDLE handle_;
  //};


  struct HBITMAP_deleter {
    typedef HBITMAP pointer;
    void operator ()(HBITMAP handle) {
      if (handle) {
        ::DeleteObject(handle);
      }
    }
  };

  //template <typename Handle,typename Handle_Deleter>
  //struct handle_holder {
  //  typedef boost::unique_ptr<Handle,Handle_Deleter> holder_type;
  //  handle_holder(Handle handle) : holder_(handle) {}
  //  operator Handle(){return holder_->get();}
  //private:
  //  holder_type holder_;
  //};

  typedef std::unique_ptr<HBITMAP,HBITMAP_deleter> bitmap_holder;

  typedef device_context<d2_dc> d2_dc_type;

  struct paint_struct 
  {
    paint_struct(HWND hwnd) : hwnd_(hwnd)
    {
      ::BeginPaint(hwnd,&paintstruct_);
    }
    ~paint_struct() {::EndPaint(hwnd_,&paintstruct_);}
    PAINTSTRUCT* operator->(){return &paintstruct_;}
  private:
    HWND hwnd_;
    PAINTSTRUCT paintstruct_;
  };

  // GDI オブジェクト管理テンプレート
  template <class GdiObject> 
  struct gdi_object: boost::noncopyable
  {
    explicit gdi_object(GdiObject obj) : gdiobj_(obj) {}
    ~gdi_object(){::DeleteObject(gdiobj_);}
    operator GdiObject(){return gdiobj_;}
  private:
    GdiObject gdiobj_;
  };

  //
  struct select_object 
  {
    select_object(HDC dc,HGDIOBJ o) : dc_(dc),o_(::SelectObject(dc,o)) {}
    ~select_object(){::SelectObject(dc_,o_);}
  private:
    HDC dc_;
    HGDIOBJ o_;
  };

  // Direct2D BeginDrawヘルパ関数
  template <typename T >
  struct begin_draw
  {
    typedef std::function<void(HRESULT hr)> err_handler_type;

    begin_draw(T& render_target,err_handler_type& handler = err_handler_type([](HRESULT hr)->void{throw sf::win32_error_exception(hr);}))
      : render_target_(render_target) ,
      handler_(handler)
    {render_target->BeginDraw();}

    ~begin_draw(){ 
      HRESULT hr = S_OK;
      hr = render_target_->EndDraw();
      if( hr != S_OK)
      {
        handler_(hr);
      }
    }
  private:
    T& render_target_;
    err_handler_type handler_;
  };

  struct mouse
  {
    mouse() : x_(0.0f),y_(0.0f),left_button_(false),middle_button_(false),right_button_(false){}
  private:
    float x_,y_;
    bool left_button_,middle_button_,right_button_;
  };

 // 
  struct av_mm_thread_characteristics
  {
    av_mm_thread_characteristics(std::wstring& str) : task_name_(str)
    {
      handle_ = ::AvSetMmThreadCharacteristicsW(str.c_str(),(LPDWORD)&task_index_);
    }

    bool set_priority(AVRT_PRIORITY p){return (::AvSetMmThreadPriority(handle_,p) == TRUE);}

    ~av_mm_thread_characteristics()
    {
      ::AvRevertMmThreadCharacteristics(handle_);
    }

  private:
    std::wstring task_name_;
    uint32_t task_index_;
    HANDLE handle_;
  };

  struct widget
  {
    void draw();
    float x_,y_;
  };

  typedef sf::begin_draw<ID2D1BitmapRenderTargetPtr> begin_draw_bitmap;
  typedef sf::begin_draw<ID2D1HwndRenderTargetPtr> begin_draw_hwnd;

  // ウィンドウプロシージャの識別用クラス
  struct wndproc
  {
    typedef WNDPROC proc_type;
    typedef INT_PTR return_type;
    static inline return_type def_wnd_proc(HWND hwnd, uint32_t message, WPARAM wParam, LPARAM lParam)
    {
      return ::DefWindowProcW(hwnd, message, wParam, lParam);
    }
  };

  // ダイアログプロシージャの識別用クラス
  struct dlgproc
  {
    typedef DLGPROC proc_type;
    typedef INT_PTR return_type;
    static inline return_type def_wnd_proc(HWND hwnd, uint32_t message, WPARAM wParam, LPARAM lParam)
    {
      return FALSE;
    }
  };

  template<typename Window>
  inline void send_message(Window& wnd, uint32_t message, WPARAM wparam, LPARAM lparam)
  {
    ::SendMessage(reinterpret_cast<HWND>(wnd.raw_handle()), message, wparam, lparam);
  }

  template<typename Window>
  inline void send_message(std::unique_ptr<Window>& wnd, uint32_t message, WPARAM wparam, LPARAM lparam)
  {
    ::SendMessage(reinterpret_cast<HWND>(wnd->raw_handle()), message, wparam, lparam);
  }

  inline void send_message(HWND hwnd, uint32_t message, WPARAM wparam, LPARAM lparam)
  {
    ::SendMessage(hwnd, message, wparam, lparam);
  }


  template<typename Window>
  inline void post_message(Window& wnd, uint32_t message, WPARAM wparam, LPARAM lparam)
  {
    ::PostMessage(reinterpret_cast<HWND>( wnd.raw_handle()), message, wparam, lparam);
  }

  template<typename Window>
  inline void post_message(std::unique_ptr<Window>& wnd, uint32_t message, WPARAM wparam, LPARAM lparam)
  {
    ::PostMessage(reinterpret_cast<HWND>(wnd->raw_handle()), message, wparam, lparam);
  }

  inline void post_message(HWND hwnd, uint32_t message, WPARAM wparam, LPARAM lparam)
  {
    ::PostMessage(hwnd, message, wparam, lparam);
  }

  template<typename Window>
  inline void enable(Window& wnd, bool enable)
  {
    ::EnableWindow(reinterpret_cast<HWND>(wnd.raw_handle()), enable ? TRUE : FALSE);
  }

  template<typename Window>
  inline void enable(std::unique_ptr<Window>& wnd, bool enable)
  {
    ::EnableWindow(reinterpret_cast<HWND>(wnd->raw_handle()), enable ? TRUE : FALSE);
  }



  inline void message_box(HWND hwnd, const std::wstring& text, const std::wstring& caption, uint32_t type = MB_OK)
  {
    ::MessageBox(hwnd, text.c_str(), caption.c_str(), type);
  }

  template<typename Window>
  inline void message_box(Window& wnd, const std::wstring& text, const std::wstring& caption, uint32_t type = MB_OK)
  {
    ::MessageBox(reinterpret_cast<HWND>(wnd.raw_handle()), text.c_str(), caption.c_str(), type);
  }

  // ウィンドウのサブクラス化

  struct sub_class : boost::noncopyable {
    typedef std::function<LRESULT(HWND hwnd, uint32_t message, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass,DWORD_PTR dwRefData)> call_back_t;
    typedef LRESULT(sub_class::*mem_func_t)(HWND, UINT, WPARAM, LPARAM, UINT_PTR, DWORD_PTR);
   
    sub_class(base_window& window, call_back_t& call_back, DWORD_PTR data) : window_(window), call_back_(call_back), thunk_((LONG_PTR)this)
    {
      sub_class_proc_ = (SUBCLASSPROC) thunk_.getCode();
      SetWindowSubclass((HWND) window_.raw_handle(), sub_class_proc_, (UINT_PTR) &id_, data);
    }

    ~sub_class()
    {
      RemoveWindowSubclass((HWND) window_.raw_handle(), sub_class_proc_, (UINT_PTR) &id_);
    }
  private:
    // 64bitモードのみで動作するサンクコード
    // HookProcの呼び出しにthisを加えてメンバー関数として呼び出すコード
    struct sub_class_proc_thunk : public Xbyak::CodeGenerator {
      sub_class_proc_thunk(LONG_PTR this_addr)
      {
        // メンバ関数のアドレスを取得
        auto temp = &sub_class::sub_class_proc;
        // メンバ関数のアドレスをLONG_PTRにキャストする
        // 普通にキャストできないので、ポインタのアドレスをvoid**にキャストして参照する
        LONG_PTR proc = reinterpret_cast<LONG_PTR>(*(void**) &temp);

        // 引数の位置をひとつ後ろにずらす
        mov(r10, r9);
        mov(r9, r8);
        mov(r8, rdx);
        mov(rdx, rcx);
        // thisのアドレスを第一引数(rcx)に格納する
        mov(rcx, (LONG_PTR) this_addr);
        // スタックにある変数を取り出す
        // 戻り先アドレス+作業用変数
        lea(r11, ptr[rsp + 40]);
        push(ptr[r11 + 8]);//第6変数
        push(ptr[r11]);//第5変数
        push(r10);//第4変数
        // 関数呼び出し
        // 作業用スタックの確保
        sub(rsp, 32);
        mov(r10, proc);
        call(r10);
        // スタックの清掃
        // 作業用スタック + 変数3つ分
        add(rsp, 32 + 8 * 3);
        ret(0);
      }
    };

    // 汎用性を高めるために、関数をファンクタ(std::function)で呼び換えるためのラッパー
    LRESULT sub_class_proc(
      HWND hWnd,
      UINT uMsg,
      WPARAM wParam,
      LPARAM lParam,
      UINT_PTR uIdSubclass,
      DWORD_PTR dwRefData
      ){
      // ファンクタの呼び出し
      return call_back_(hWnd, uMsg, wParam, lParam, uIdSubclass, dwRefData);
    }

    UINT id_;
    sub_class_proc_thunk thunk_;
    base_window& window_;
    call_back_t& call_back_;
    SUBCLASSPROC sub_class_proc_;
  };

  // フック

  struct hook : boost::noncopyable {

    // フック処理ファンクションオブジェクト
    typedef std::function<LRESULT(int nCode, WPARAM wp, LPARAM lp)> hook_proc_t;
    hook(int idHook,        // フックタイプ
      hook_proc_t& proc,  // フックプロシージャ
      HINSTANCE hMod,    // アプリケーションインスタンスのハンドル
      DWORD dwThreadId   // スレッドの識別子
      ) : proc_(proc), thunk_((LONG_PTR)this), thread_id_(dwThreadId)
    {
      hook_ = SetWindowsHookEx(
        idHook,        // フックタイプ
        (HOOKPROC)thunk_.getCode(),     // フックプロシージャ
        hMod,    // アプリケーションインスタンスのハンドル
        dwThreadId   // スレッドの識別子
        );
    }

    ~hook(){
      UnhookWindowsHookEx(hook_);
    }
    HHOOK get_handle(){ return hook_; }
  private:

    // 64bitモードのみで動作するサンクコード
    // HookProcの呼び出しにthisを加えてメンバー関数として呼び出すコード
    struct hook_proc_thunk : public Xbyak::CodeGenerator {
      hook_proc_thunk(LONG_PTR this_addr)
      {
        // メンバ関数のアドレスを取得
        auto temp = &hook::hook_proc;
        // メンバ関数のアドレスをLONG_PTRにキャストする
        // 普通にキャストできないので、ポインタのアドレスをvoid**にキャストして参照する
        LONG_PTR proc = reinterpret_cast<LONG_PTR>(*(void**) &temp);

        // 引数の位置をひとつ後ろにずらす
        mov(r9, r8);
        mov(r8, rdx);
        mov(rdx, rcx);
        // thisのアドレスを第一引数(rcx)に格納する
        mov(rcx, this_addr);
        // 関数呼び出し
        // 作業用スタックの確保
        sub(rsp, 32);
        mov(r10, proc);
        call(r10);
        // スタックの清掃
        add(rsp, 32);
        ret(0);
      }
    };

    // 汎用性を高めるために、関数をファンクタ(std::function)で呼び換えるためのラッパー
    LRESULT hook_proc(int nCode, WPARAM wp, LPARAM lp)
    {
      return proc_(nCode, wp, lp);
    }
    hook_proc_thunk thunk_;
    hook_proc_t& proc_;
    HHOOK hook_;
    DWORD thread_id_;
  };

  inline LRESULT CALLBACK dummyProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
  {
    return DefWindowProcW(hwnd, uMsg, wParam, lParam);
  }

}