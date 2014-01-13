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
      int x = 0,                // �E�B���h�E�̉������̈ʒu
      int y = 0,                // �E�B���h�E�̏c�����̈ʒu
      int nWidth = CW_USEDEFAULT,           // �E�B���h�E�̕�
      int nHeight = CW_USEDEFAULT,          // �E�B���h�E�̍���
      HMENU hMenu = NULL,          // ���j���[�n���h���܂��͎q���ʎq
      DWORD dwExStyle = 0,
      DWORD dwStyle = WS_CHILD | WS_VISIBLE,        // �E�B���h�E�X�^�C��
      HINSTANCE hInstance = GetModuleHandle(NULL)  // �A�v���P�[�V�����C���X�^���X�̃n���h��
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
    // �����o�[�֐��𒼐ڌĂяo���B
    struct subclassproc_thunk : public Xbyak::CodeGenerator {
      subclassproc_thunk(LONG_PTR this_addr)
      {
        LRESULT(control_base::*pmemfunc)(HWND, UINT, WPARAM, LPARAM) = &control_base::subclass_proc_;
        LONG_PTR  proc = (LONG_PTR)(*(void**) &pmemfunc);
        // �����̈ʒu���ЂƂ��ɂ��炷
        mov(r10, r9);
        mov(r9, r8);
        mov(r8, rdx);
        mov(rdx, rcx);
        // this�̃A�h���X��rcx�Ɋi�[����
        mov(rcx, (LONG_PTR) this_addr);
        // ��5�������X�^�b�N�Ɋi�[
        push(r10);
        sub(rsp, 32);
        mov(r10, proc);
        // �����o�֐��Ăяo��
        call(r10);
        add(rsp, 40);
        ret(0);
      }
    };

    subclassproc_thunk thunk_;
  protected:
  };
}




