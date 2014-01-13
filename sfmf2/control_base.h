#pragma once

namespace sf{
  class control_base
  {
  public:
    typedef std::function<LRESULT (control_base& base, UINT uMsg, WPARAM wParam, LPARAM lParam) > subclass_func_t;
    typedef std::unique_ptr<control_base> control_base_ptr;
 
    template <typename ParentWindowType> 
    control_base(std::wstring& class_name,
      std::wstring& window_name, 
      subclass_func_t func,
      ParentWindowType& parent,
      int x = 0,                // ウィンドウの横方向の位置
      int y = 0,                // ウィンドウの縦方向の位置
      int nWidth = CW_USEDEFAULT,           // ウィンドウの幅
      int nHeight = CW_USEDEFAULT,          // ウィンドウの高さ
      HMENU hMenu = NULL,          // メニューハンドルまたは子識別子
      DWORD dwExStyle = 0,
      DWORD dwStyle = WS_CHILD | WS_VISIBLE,        // ウィンドウスタイル
      HINSTANCE hInstance = GetModuleHandle(NULL)  // アプリケーションインスタンスのハンドル
      ) : thunk_((LONG_PTR)this), func_(std::move(func))
    {
      hwnd_ = CreateWindowEx(dwExStyle,class_name.c_str(),window_name.c_str(),dwStyle,x,y,nWidth,nHeight,reinterpret_cast<HWND>(parent.raw_handle()),hMenu,hInstance,NULL);
      if (hwnd_ == NULL){
        OutputDebugStringW((boost::wformat(L"~~~~~~ %s failed!! ~~~~~~~~~~") % class_name.c_str()).str().c_str() );
        throw sf::win32_error_exception();
      }
      proc_ = (SUBCLASSPROC) thunk_.getCode();
      SetWindowSubclass(hwnd_, proc_, (UINT_PTR) &id_subclass_, NULL);
    }

    virtual ~control_base()
    {
      RemoveWindowSubclass(hwnd_, proc_, (UINT_PTR) &id_subclass_);
    };
    
    LRESULT subclass_proc_(HWND hWnd,UINT uMsg,WPARAM wParam, LPARAM lParam )
    {

      //return DefSubclassProc(hWnd,uMsg,wParam,lParam);
      assert(hWnd == (HWND)this->raw_handle());
      return func_(*this, uMsg, wParam, lParam);
    }

    void* raw_handle(){ return hwnd_; }
  private:
    UINT id_subclass_;
    HWND hwnd_;
    SUBCLASSPROC proc_;
    subclass_func_t func_;
    // メンバー関数を直接呼び出す。
    struct subclassproc_thunk : public Xbyak::CodeGenerator {
      subclassproc_thunk(LONG_PTR this_addr)
      {
        LRESULT(control_base::*pmemfunc)(HWND, UINT, WPARAM, LPARAM) = &control_base::subclass_proc_;
        LONG_PTR  proc = (LONG_PTR)(*(void**) &pmemfunc);
        // 引数の位置をひとつ後ろにずらす
        mov(r10, r9);
        mov(r9, r8);
        mov(r8, rdx);
        mov(rdx, rcx);
        // thisのアドレスをrcxに格納する
        mov(rcx, (LONG_PTR) this_addr);
        // 第5引数をスタックに格納
        push(r10);
        sub(rsp, 32);
        mov(r10, proc);
        // メンバ関数呼び出し
        call(r10);
        add(rsp, 40);
        ret(0);
      }
    };

    subclassproc_thunk thunk_;
  protected:
  };
}




