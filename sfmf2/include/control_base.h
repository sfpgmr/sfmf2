#pragma once

namespace sf{

class control_base {
public:
	control_base(){};
	virtual ~control_base() {};
	virtual HWND hwnd() = 0;
	virtual void * raw_handle() = 0;
};

// ポインタ型
typedef std::unique_ptr<control_base> control_base_ptr;

struct empty_t {};

template < typename BaseClass = empty_t >
  class control_impl : public control_base ,public BaseClass 
  {
  public:
    typedef control_impl<BaseClass> this_type;
    // サブクラス化した後に呼ばれるファンクタ定義
	typedef std::function<LRESULT(this_type& base, UINT uMsg, WPARAM wParam, LPARAM lParam) > subclass_func_t;
	friend class subclass_func_t;
 
    template <typename ParentWindowType> 
	control_impl(std::wstring& class_name,
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
      hwnd_ = CreateWindowEx(dwExStyle,class_name.c_str(),window_name.c_str(),dwStyle,x,y,nWidth,nHeight,reinterpret_cast<HWND>(parent.hwnd()),hMenu,hInstance,NULL);
      if (hwnd_ == NULL){
        throw sf::win32_error_exception();
      }
      proc_ = (SUBCLASSPROC) thunk_.getCode();
      // サブクラス化
      SetWindowSubclass(hwnd_, proc_, (UINT_PTR) &id_subclass_, NULL);
    }

	virtual ~control_impl()
    {
      // サブクラス化解除
      RemoveWindowSubclass(hwnd_, proc_, (UINT_PTR) &id_subclass_);
    };
    
    // ファンクタを呼び出すための転送メンバ関数
    LRESULT subclass_proc_(HWND hWnd,UINT uMsg,WPARAM wParam, LPARAM lParam )
    {

      //return DefSubclassProc(hWnd,uMsg,wParam,lParam);
      assert(hWnd == (HWND)this->raw_handle());
      return func_(*this, uMsg, wParam, lParam);
    }

    void* raw_handle(){ return hwnd_; }
	HWND hwnd(){ return hwnd_; }
  private:
    UINT id_subclass_;
    HWND hwnd_;
    SUBCLASSPROC proc_;
    subclass_func_t func_;
    // メンバー関数を直接呼び出すサンクというかグルーコード。
    struct subclassproc_thunk : public Xbyak::CodeGenerator {
      subclassproc_thunk(LONG_PTR this_addr)
      {
		  LRESULT(sf::control_impl<BaseClass>::*pmemfunc)(HWND, UINT, WPARAM, LPARAM) = &sf::control_impl<BaseClass>::subclass_proc_;
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




