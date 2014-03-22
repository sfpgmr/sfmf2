/*
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
#include "fluidcs11_window.h"
#include "renderer.h"
#include "CommDlg.h"
#include "icon.h"
#include "timer.h"
#include "exception.h"
#include "application.h"
//#include "config_tab_dialog.h"
//#include "info_tab_dialog.h"
//#include "midi_config_tab_dialog.h"

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

	// Hit test the frame for resizing and moving.
	LRESULT HitTestNCA(MARGINS& margin,HWND hWnd, WPARAM wParam, LPARAM lParam)
	{
		// Get the point coordinates for the hit test.
		POINT ptMouse = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

		// Get the window rectangle.
		RECT rcWindow;
		GetWindowRect(hWnd, &rcWindow);

		// Get the frame rectangle, adjusted for the style without a caption.
		RECT rcFrame = { 0 };
		AdjustWindowRectEx(&rcFrame, WS_OVERLAPPEDWINDOW & ~WS_CAPTION, FALSE, NULL);

		// Determine if the hit test is for resizing. Default middle (1,1).
		USHORT uRow = 1;
		USHORT uCol = 1;
		bool fOnResizeBorder = false;

		// Determine if the point is at the top or bottom of the window.
		if (ptMouse.y >= rcWindow.top && ptMouse.y < rcWindow.top + margin.cyTopHeight)
		{
			fOnResizeBorder = (ptMouse.y < (rcWindow.top - rcFrame.top));
			uRow = 0;
		}
		else if (ptMouse.y < rcWindow.bottom && ptMouse.y >= rcWindow.bottom - margin.cyBottomHeight)
		{
			uRow = 2;
		}

		// Determine if the point is at the left or right of the window.
		if (ptMouse.x >= rcWindow.left && ptMouse.x < rcWindow.left + margin.cxLeftWidth)
		{
			uCol = 0; // left side
		}
		else if (ptMouse.x < rcWindow.right && ptMouse.x >= rcWindow.right - margin.cxRightWidth)
		{
			uCol = 2; // right side
		}

		// Hit test (HTTOPLEFT, ... HTBOTTOMRIGHT)
		LRESULT hitTests[3][3] =
		{
			{ HTTOPLEFT, fOnResizeBorder ? HTTOP : HTCAPTION, HTTOPRIGHT },
			{ HTLEFT, HTNOWHERE, HTRIGHT },
			{ HTBOTTOMLEFT, HTBOTTOM, HTBOTTOMRIGHT },
		};

		return hitTests[uRow][uCol];
	}

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

	/** window ベースクラス */

	struct fluidcs11_window::impl : public sf::base_win32_window2<sf::fluidcs11_window::impl, window_renderer>
	{
		using proc_t = sf::wndproc;
		using base_t = sf::base_win32_window2<sf::fluidcs11_window::impl, window_renderer>;

		const int margins_LEFT = 8;
		const int margins_RIGHT = 8;
		const int margins_TOP = 32;
		const int margins_BOTTOM = 8;

		impl(const std::wstring& menu_name, const std::wstring& name, bool fit_to_display, float width = 160, float height = 100)
		: base_win32_window2(menu_name, name, fit_to_display, width, height)
		, timer_(*this, 100)
		, icon_(IDI_ICON1)
		/*,mesh_color_(0.7f, 0.7f, 0.7f, 1.0f)*/
		, thumb_start_(false)
		, child_class_(L"SFCHILD", L"SFCHILD", HINST_THISCOMPONENT, dummyProc, CS_HREDRAW | CS_VREDRAW)
		//,hook_proc_(std::bind(&impl::hook_proc, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)),
		//hook_(WH_CALLWNDPROC, hook_proc_, HINST_THISCOMPONENT, GetCurrentThreadId())
		{
			margins_ = { dpi_.scale_x(margins_LEFT), dpi_.scale_y(margins_TOP), dpi_.scale_x(margins_RIGHT), dpi_.scale_y(margins_BOTTOM) };
			fullscreen_ = false;
			//on_render.connect(boost::bind(&impl::render,this));
		};

		~impl(){
			//safe_release(dxgi_factory_);
		};

		//LRESULT hook_proc(int nCode, WPARAM wp, LPARAM lp)
		//{
		//  if (nCode == HC_ACTION)
		//  {
		//    //OutputDebugStringW(L"TEST\n");

		//    CWPSTRUCT* cwps = reinterpret_cast<CWPSTRUCT*>(lp);
		//    if (cwps->message == WM_CREATE)
		//    {
		//      WCHAR szClass[MAX_PATH];
		//      GetClassNameW(cwps->hwnd, szClass, MAX_PATH);
		//      std::wcout << szClass;
		//    }
		//  }
		//   return CallNextHookEx(hook_.get_handle(),nCode, wp, lp);
		//}
		// -------------------------------------------------------------------
		// ウィンドウプロシージャ
		// -------------------------------------------------------------------

		void create(){
			//create_device_independent_resources();
			//    icon_ = ::LoadIconW(HINST_THISCOMPONENT,MAKEINTRESOURCE(IDI_ICON1));
			register_class(this->name_.c_str(), CS_HREDRAW | CS_VREDRAW, 0, 0, icon_.get());
			create_window();
			SetWindowTextW(*this,name_.c_str());
			ShowWindow(*this, SW_SHOW);

			init_ = true;

			// 半透明ウィンドウを有効にする。
			//BOOL dwmEnable;
			//DwmIsCompositionEnabled (&dwmEnable); 
			//if (dwmEnable) EnableBlurBehind(*this);

		}

 		void discard_device()
		{
			renderer_->discard();
			sf::graphics::instance()->discard_device();
		}

		void render()
		{
			renderer_->render();
		}

		void video_bitmap(ID2D1Bitmap1Ptr& bitmap)
		{
			renderer_->video_bitmap() = bitmap;
		}

		result_t window_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			LRESULT rt = 0;
			bool p = !DwmDefWindowProc(hwnd, message, wParam, lParam, &rt);
			if (message == WM_NCHITTEST && rt == 0)
			{
				rt = HitTestNCA(margins_, hwnd, wParam, lParam);

				if (rt != HTNOWHERE)
				{
					p = false;
				}

			}
			if (p) {
				switch (message)
				{
				case WM_NCCREATE:
					break;
				case WM_CREATE:
					return on_create(reinterpret_cast<CREATESTRUCT*>(lParam));
				case WM_INITDIALOG:
					break;
				case WM_SIZE:
					return on_size(wParam, LOWORD(lParam), HIWORD(lParam));
				case WM_PAINT:
					return on_paint();
				case WM_DISPLAYCHANGE:
					return on_display_change(wParam, LOWORD(lParam), HIWORD(lParam));
				case WM_ERASEBKGND:
					return on_erase_backgroud(reinterpret_cast<HDC>(wParam));
				case WM_HSCROLL:
					return on_hscroll(LOWORD(wParam), HIWORD(wParam), reinterpret_cast<HWND>(lParam));
				case WM_VSCROLL:
					//return on_vscroll(LOWORD(wParam), HIWORD(wParam), reinterpret_cast<HWND>(lParam));
				case WM_LBUTTONDOWN:
					//return on_left_mouse_button_down(
					//	wParam, dpi_.scale_x(
					//	GET_X_LPARAM(lParam)), dpi_.scale_y(GET_Y_LPARAM(lParam)))
					//	;
					break;
					//case WM_LBUTTONUP:
					//	return on_left_mouse_button_up(
					//		wParam, dpi_.scale_x(
					//		GET_X_LPARAM(lParam)), dpi_.scale_y(GET_Y_LPARAM(lParam)))
					//		;
					//case WM_LBUTTONDBLCLK:
					//	return on_left_mouse_button_double_click(wParam,
					//		dpi_.scale_x(
					//		GET_X_LPARAM(lParam)), dpi_.scale_y(GET_Y_LPARAM(lParam)))
					;
				case WM_ACTIVATE:

					return on_activate(LOWORD(wParam), HIWORD(wParam) != 0);
					//case WM_MOUSEMOVE:
					//{
					//	return on_mouse_move(wParam,
					//		dpi_.scale_x(
					//		GET_X_LPARAM(lParam)), dpi_.scale_y(GET_Y_LPARAM(lParam)))
					//		;
					//	//					on_mouse_move(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam),wParam); 
					//}
					//case WM_MOUSEWHEEL:
					//	return on_mouse_wheel(GET_KEYSTATE_WPARAM(wParam), GET_WHEEL_DELTA_WPARAM(wParam),
					//		dpi_.scale_x(
					//		GET_X_LPARAM(lParam)), dpi_.scale_y(GET_Y_LPARAM(lParam)))
					//		;
					//case WM_MOUSELEAVE:
					//	return on_mouse_leave();
				case WM_KEYDOWN:
					return on_key_down(wParam, lParam & 0xffff0000, LOWORD(lParam));
					//case WM_KEYUP:
					//	return on_key_up(wParam, lParam & 0xffff0000, LOWORD(lParam));
					//case WM_APPCOMMAND:
					//	return on_app_command(GET_APPCOMMAND_LPARAM(lParam), GET_DEVICE_LPARAM(lParam), GET_KEYSTATE_LPARAM(lParam));
				case WM_COMMAND:
					return on_command(wParam, lParam);
				case WM_DESTROY:
					return on_destroy();
				case WM_CLOSE:
					return on_close();
				case WM_TIMER:
					return on_timer(wParam);
				case WM_NOTIFY:
					//return on_notify(reinterpret_cast<NMHDR*>(lParam));
				case WM_DWMCOMPOSITIONCHANGED:
					//return on_dwm_composition_changed();
				case WM_NCCALCSIZE:
					return on_nccalcsize(reinterpret_cast<NCCALCSIZE_PARAMS*>(lParam));

				}

				// 他のWindowメッセージを派生クラス側でフックできるようにする
				return proc_t::def_wnd_proc(hwnd, message, wParam, lParam);
			}
			else {
				return  rt;
			}



		};

		result_t on_key_down(uint32_t vkey, uint32_t ext_key, uint32_t repeat)
		{
		  if(vkey == VK_ESCAPE)
		  {
		    PostMessage( hwnd_, WM_CLOSE, 0, 0 );
		  }
		  return std::is_same<proc_t,wndproc>::value?0:FALSE; 
		}


		result_t on_create(CREATESTRUCT *p)
		{
			// ウィンドウ全体を半透明にする
			SetLayeredWindowAttributes(
				hwnd_,
				RGB(0, 0, 0), // color key
				100, // alpha value
				LWA_ALPHA | LWA_COLORKEY);

			// ウィンドウの指定領域を半透明にする
			// リージョンの設定
			//HRGN rgn = CreateRectRgn(0, 0, width_, height_);
			//SetWindowRgn(hwnd_, rgn, FALSE);
			renderer_.reset(new renderer_t(*this));
			timer_.start();
			show_window(hwnd_,SW_SHOW);
			SetWindowTextW(hwnd_,title_.c_str());

			// Inform application of the frame change.
			RECT rc;
			GetWindowRect(hwnd_, &rc);
			SetWindowPos(hwnd_,
				NULL,
				rc.left, rc.top,
				rc.right - rc.left, rc.bottom - rc.top,
				SWP_FRAMECHANGED);
			init_content();
			return TRUE;
			;
		}

		void init_content()
		{
			// 
			InitCommonControls();
			// タブコントロール
			// タブコントロール

			tab_.reset(new sf::control_impl<>(std::wstring(WC_TABCONTROLW), std::wstring(L"MainTab"),
				[this](sf::control_impl<>& base, UINT uMsg, WPARAM wParam, LPARAM lParam)->LRESULT
			{
				static int count = 0;
				switch (uMsg){
				case WM_ERASEBKGND:
					return FALSE;
					break;
				case WM_PAINT:
//					paint_struct ps(base.hwnd());
					break;
				}
				return DefSubclassProc( base.hwnd(), uMsg, wParam, lParam);
			}, *this, dpi_.scale_x(8), dpi_.scale_y(8), width_ - dpi_.scale_x(16), height_ - dpi_.scale_y(16), (HMENU) 10, WS_EX_LAYERED, WS_TABSTOP | WS_VISIBLE | WS_CHILD));

			TCITEM ti = { 0 };
			ti.mask = TCIF_TEXT | TCIF_IMAGE; 
			ti.pszText = L"TEST";
			ti.cchTextMax = 32;
			ti.iImage = -1;
			TabCtrl_InsertItem( tab_->hwnd(), 0, &ti);
			ti.pszText = L"TEST2";
			TabCtrl_InsertItem(tab_->hwnd(), 1, &ti);
			SetLayeredWindowAttributes(tab_->hwnd(), RGB(0, 0, 0), 255, LWA_ALPHA | LWA_COLORKEY);
			DWORD style = TabCtrl_GetExtendedStyle(tab_->hwnd());
			TabCtrl_SetExtendedStyle(tab_->hwnd(), style | TCS_EX_FLATSEPARATORS);
			calc_tab_item_rect();

				// 子ベース
			child_base_.reset(new control_impl<>(std::wstring(L"SFCHILD"), std::wstring(L"SFCHILD"),
				[this](control_impl<>& base, UINT uMsg, WPARAM wParam, LPARAM lParam)->LRESULT
			{
				switch (uMsg){
				case WM_PAINT:
				{
					sf::paint_struct ps( base.hwnd());
					break;
				}
				case WM_ERASEBKGND:
					return FALSE;
				case WM_COMMAND:
					// 子コントロールにメッセージを転送する
					SendMessage((HWND) lParam, WM_COMMAND, wParam, lParam);
					break;
				case WM_NOTIFY:
				{
					// 子コントロールにメッセージを転送する
					NMHDR* nmhdr((NMHDR*) lParam);
					SendMessage(nmhdr->hwndFrom, WM_NOTIFY, wParam, lParam);

				}
					break;
				}
				return DefSubclassProc( base.hwnd(), uMsg, wParam, lParam);
			}, *this, dpi_.scale_x(8), dpi_.scale_y(8), width_ - dpi_.scale_x(16), height_ - dpi_.scale_y(16), (HMENU) 1, WS_EX_LAYERED, WS_CLIPSIBLINGS | WS_CHILD, HINST_THISCOMPONENT));

			SetLayeredWindowAttributes( child_base_->hwnd(), RGB(0, 0, 0), 255, LWA_ALPHA | LWA_COLORKEY);
			ShowWindow( child_base_->hwnd(), SW_SHOW);

			// 元ファイル用テキストボックス

			// ファイル選択ダイアログを表示するボタン
			src_path_btn_.reset(new control_impl<>(std::wstring(L"BUTTON"), std::wstring(L"元ファイル"),
				[this](control_impl<>& base, UINT uMsg, WPARAM wParam, LPARAM lParam)->LRESULT
			{
				static int count = 0;
				switch (uMsg){
				case WM_ERASEBKGND:
					return FALSE;
					break;
				case WM_COMMAND:
				{
					switch (HIWORD(wParam)){
					case BN_CLICKED:
						//sf::message_box( base.hwnd(), L"Test", L"Test");
						application& app(*application::instance());
						DOUT(L"BUTTON PRESSED\n");
						OPENFILENAMEW fn;
						wchar_t file[MAX_PATH];
						ZeroMemory(&fn, sizeof(fn));
						if (app.renderer_target_path().size() > 0){
							_tcscpy(file, app.renderer_source_path().c_str());
						}
						fn.lStructSize = sizeof(fn);
						fn.hwndOwner = hwnd_;
						fn.nMaxFile = MAX_PATH;
						fn.lpstrFile = file;
						fn.lpstrFilter = L"*.WAV\0";
						fn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
						fn.lpstrDefExt = L"WAV";
						fn.nFilterIndex = 1;
						fn.lpstrFile[0] = L'\0';
						if (!GetOpenFileName(&fn))
						{
							DWORD errcd = GetLastError();
							if (errcd != 0){
								sf::local_memory<wchar_t> buf;
								FormatMessage(				//エラー表示文字列作成
									FORMAT_MESSAGE_ALLOCATE_BUFFER |
									FORMAT_MESSAGE_FROM_SYSTEM |
									FORMAT_MESSAGE_IGNORE_INSERTS,
									NULL, errcd,
									MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
									(LPTSTR) &buf, 0, NULL);

								MessageBox(NULL, buf.get(), NULL, MB_OK);	//メッセージ表示
							}

						}
						else {
							//            boost::filesystem::path p(fn.lpstrFile);
							application::instance()->renderer_source_path(fn.lpstrFile);
							//send_message(src_path_text_, WM_SETTEXT, 0, (LPARAM)fn.lpstrFile);
						}
						break;
					}
				}
				}
				return DefSubclassProc( base.hwnd(), uMsg, wParam, lParam);
			}, *child_base_, 30, 30, 100, 30, 0, 0, WS_TABSTOP | WS_VISIBLE | WS_CHILD));

			// エンコード元ファイル表示テキスト
			src_path_text_.reset(new control_impl<>(std::wstring(L"STATIC"), std::wstring(L""),
				[this](control_impl<>& base, UINT uMsg, WPARAM wParam, LPARAM lParam)->LRESULT
			{
				return DefSubclassProc( base.hwnd(), uMsg, wParam, lParam);
			}, *child_base_, 130, 30, 400, 30, (HMENU) 100, 0, WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON));

			// エンコード先ファイル表示ボタン
			dest_path_btn_.reset(new control_impl<>(std::wstring(L"BUTTON"), std::wstring(L"先ファイル"),
				[this](control_impl<>& base, UINT uMsg, WPARAM wParam, LPARAM lParam)->LRESULT
			{
				static int count = 0;
				switch (uMsg){
				case WM_COMMAND:
				{
					switch (HIWORD(wParam)){
					case BN_CLICKED:
						application& app(*application::instance());
						//sf::message_box( base.hwnd(), L"Test", L"Test");
						DOUT(L"BUTTON PRESSED\n");
						OPENFILENAMEW fn;
						wchar_t file[MAX_PATH];
						ZeroMemory(&fn, sizeof(fn));
						if (app.renderer_source_path().size() > 0){
							_tcscpy(file, app.renderer_source_path().c_str());
						}
						fn.lStructSize = sizeof(fn);
						fn.hwndOwner = hwnd_;
						fn.nMaxFile = MAX_PATH;
						fn.lpstrFile = file;
						fn.lpstrFilter = L"*.M4V\0";
						fn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
						fn.lpstrDefExt = L"M4V";
						fn.nFilterIndex = 1;
						fn.lpstrFile[0] = L'\0';
						if (!GetSaveFileName(&fn))
						{
							DWORD errcd = GetLastError();
							if (errcd != 0){
								sf::local_memory<wchar_t> buf;
								FormatMessage(				//エラー表示文字列作成
									FORMAT_MESSAGE_ALLOCATE_BUFFER |
									FORMAT_MESSAGE_FROM_SYSTEM |
									FORMAT_MESSAGE_IGNORE_INSERTS,
									NULL, errcd,
									MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
									(LPTSTR) &buf, 0, NULL);

								MessageBox(NULL, buf.get(), NULL, MB_OK);	//メッセージ表示
							}
						}
						else{
							application::instance()->renderer_target_path(fn.lpstrFile);
						}
						break;
					}
				}
					break;
				}
				return DefSubclassProc( base.hwnd(), uMsg, wParam, lParam);
			}, *child_base_, 30, 60, 100, 30, (HMENU) 100, 0, WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON));

			// エンコード元ファイル表示テキスト
			dest_path_text_.reset(new control_impl<>(std::wstring(L"STATIC"), std::wstring(L""),
				[this](control_impl<>& base, UINT uMsg, WPARAM wParam, LPARAM lParam)->LRESULT
			{
				return DefSubclassProc( base.hwnd(), uMsg, wParam, lParam);
			}, *child_base_, 130, 60, 400, 30, (HMENU) 100, 0, WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON));


			// 処理実行ボタン
			render_button_.reset(new control_impl<>(std::wstring(L"BUTTON"), std::wstring(L"実行"),
				[this](control_impl<>& base, UINT uMsg, WPARAM wParam, LPARAM lParam)->LRESULT
			{
				static int count = 0;
				switch (uMsg){
				case WM_COMMAND:
				{
					switch (HIWORD(wParam)){
					case BN_CLICKED:
						auto* this_ptr = this;
						application::instance()->execute_rendering(
							std::bind(&fluidcs11_window::impl::progress, this_ptr, std::placeholders::_1),
							std::bind(&fluidcs11_window::impl::complete, this_ptr, std::placeholders::_1)
							);
						break;
					}
				}
					break;

				}
				return DefSubclassProc( base.hwnd(), uMsg, wParam, lParam);
			}, *child_base_, 600, 30, 100, 30, 0, 0, WS_TABSTOP | WS_VISIBLE | WS_CHILD));

			EnableWindow( render_button_->hwnd(), FALSE);
			SetLayeredWindowAttributes( hwnd_, 0, 255, LWA_ALPHA);

			application::instance()->renderer_enable_status_changed().connect(
				[this](bool enabled) -> void{
				EnableWindow( render_button_->hwnd(), enabled ? TRUE : FALSE);
				if (enabled){
					post_message(render_button_, WM_SETFOCUS, NULL, NULL);
				}
			}
			);

			application::instance()->renderer_source_path_changed().connect(
				[this]() -> void {
				send_message(src_path_text_, WM_SETTEXT, 0, (LPARAM) application::instance()->renderer_source_path().c_str());
			}
			);

			application::instance()->renderer_target_path_changed().connect(
				[this]() -> void {
				send_message(dest_path_text_, WM_SETTEXT, 0, (LPARAM) application::instance()->renderer_target_path().c_str());
			}
			);

			// テスト表示プログレスバー
			progress_.reset(new control_impl<>(std::wstring(PROGRESS_CLASS), std::wstring(L"テスト2"),
				[this](control_impl<>& base, UINT uMsg, WPARAM wParam, LPARAM lParam)->LRESULT
			{
				return DefSubclassProc( base.hwnd(), uMsg, wParam, lParam);
			}, *child_base_, 30, 120, 600, 30, 0, 0, WS_TABSTOP | WS_VISIBLE | WS_CHILD));

			// タイトルテキスト
			title_text_.reset(new control_impl<>(std::wstring(L"EDIT"), std::wstring(L""),
				[this](control_impl<>& base, UINT uMsg, WPARAM wParam, LPARAM lParam)->LRESULT
			{
				switch (uMsg){
				case WM_COMMAND:
					switch (HIWORD(wParam)){
					case EN_CHANGE:
					{
						wchar_t ptmp[256];
						Edit_GetText((HWND)lParam, ptmp, ARRAYSIZE(ptmp));
						application::instance()->renderer_video_title().assign(ptmp);
					}
						break;
					}
					break;
				}
				return DefSubclassProc( base.hwnd(), uMsg, wParam, lParam);
			}, *child_base_, 30, 180, 600, 30, 0, 0, WS_TABSTOP | WS_VISIBLE | WS_CHILD));

			compute_time_.reset(new control_impl<>(std::wstring(PROGRESS_CLASS), std::wstring(L"テスト2"),
				[this](control_impl<>& base, UINT uMsg, WPARAM wParam, LPARAM lParam)->LRESULT
			{
				return DefSubclassProc( base.hwnd(), uMsg, wParam, lParam);
			}, *child_base_, 30, 120, 600, 30, 0, 0, WS_TABSTOP | WS_VISIBLE | WS_CHILD));

			// 生成時間表示
			compute_time_.reset(new control_impl<>(std::wstring(L"EDIT"), std::wstring(L""),
				[this](control_impl<>& base, UINT uMsg, WPARAM wParam, LPARAM lParam)->LRESULT
			{
				return DefSubclassProc( base.hwnd(), uMsg, wParam, lParam);
			}, *child_base_, 30, 480, 600, 30, 0, 0, WS_VISIBLE | WS_CHILD));

			show_window(child_base_, SW_HIDE);
			show_window(tab_, SW_HIDE);


			add_dcomp_content_to_root(*renderer_, tab_->hwnd());
			add_dcomp_content_to_root(*renderer_,  child_base_->hwnd());


		}

		void progress(int progress)
		{
			send_message(progress_, PBM_SETPOS, progress, 0);
		}


		void complete(std::chrono::duration<double>& time)
		{
			send_message(compute_time_, WM_SETTEXT, 0, (LPARAM) (boost::wformat(L"Encoding Time: %d sec") % time.count()).str().c_str());
		}

		result_t on_activate(int active, bool minimized)
		{
			// Extend the frame into the client area.

			CHK(DwmExtendFrameIntoClientArea(hwnd_, &margins_));
			activate_ = (active != 0);
			if(activate_ && init_ && renderer_)
			{
			renderer_->restore_swapchain_and_dependent_resources();
			}
			return std::is_same<proc_t,wndproc>::value?0:FALSE;
		}

		virtual result_t on_nccalcsize(NCCALCSIZE_PARAMS* pncsp)
		{
			pncsp->rgrc[0].left = pncsp->rgrc[0].left + 0;
			pncsp->rgrc[0].top = pncsp->rgrc[0].top + 0;
			pncsp->rgrc[0].right = pncsp->rgrc[0].right - 0;
			pncsp->rgrc[0].bottom = pncsp->rgrc[0].bottom - 0;

			return  std::is_same<proc_t, wndproc>::value ? 0 : FALSE;

		}

		result_t on_paint()
		{
			{
				sf::paint_struct p(hwnd_);
				render();
			}
			return  std::is_same<proc_t, wndproc>::value ? 0 : FALSE;
		}

		result_t on_size(uint32_t flag, uint32_t width, uint32_t height)
		{
			// バックバッファなどに関係するインターフェースを解放する
			// バックバッファを解放する
			if (init_)
			{
				int height = client_height_;
				int width = client_width_;

				calc_client_size();

				if (tab_ && tab_->hwnd()){
//					RECT r = { 0, 0, width_ ,height_};
//					TabCtrl_AdjustRect(tab_->hwnd(),FALSE,&r);
					SetWindowPos( tab_->hwnd(), 0, 8, 8, client_width_ - 16, client_height_ - 16,  SWP_NOOWNERZORDER | SWP_NOZORDER);
					SetWindowPos( child_base_->hwnd(), 0, 8, 8, client_width_ - 16, client_height_ - 16, SWP_NOOWNERZORDER | SWP_NOZORDER);

				}

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

			if (init_ && !(width == 0 || height == 0))
			{
				update_window_size();
				calc_client_size();
			}

			return TRUE;
		}

		LRESULT on_display_change(uint32_t bpp, uint32_t h_resolution, uint32_t v_resolution)
		{
			InvalidateRect(hwnd_,nullptr,FALSE);
			return TRUE;
		}

		LRESULT on_erase_backgroud(HDC dc)
		{
			return FALSE;
		}

		LRESULT on_hscroll(uint32_t state, uint32_t position, HWND ctrl_hwnd)
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
			on_closed_();
			//slider_.detatch();
			src_path_btn_.reset();
			timer_.player_stop();
			// 後始末
			discard_device();
			// レンダーターゲットのリリース
			//safe_release(dcr_);
			//      safe_release(render_target_);
			// Windowの破棄
			graphics::instance()->discard_device_independant_resources();
			BOOL ret(::DestroyWindow(hwnd_));
			BOOST_ASSERT(ret != 0);
			return TRUE;
		}

		LRESULT on_command(WPARAM wparam, LPARAM lparam)
		{
			SendMessage( (HWND)lparam, WM_COMMAND, wparam, lparam);
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

		// invalidate_rect();
		return TRUE;
		}

	private:

		void calc_tab_item_rect()
		{
			RECT rci;
			TabCtrl_GetItemRect(tab_->hwnd(), 0, &rci);
			RECT r;
			GetClientRect(tab_->hwnd(), &r);
			RECT rw;
			GetWindowRect(tab_->hwnd(), &rw);
			POINT pt = { rw.left, rw.top };
			ScreenToClient( hwnd(), &pt);

			//tab_item_rect_.left = pt.x + ;
			//tab_item_rect_.top = pt.y;


			//set_pos(HWND_TOP,
			//	pt.x ,
			//	pt.y ,
			//	r.right - cxMargin * 2 - 1,
			//	r.bottom - rci.bottom - cyMargin * 2 - 1
			//	, SWP_NOZORDER
			//	);

		}

		// hook hook_;
		// hook::hook_proc_t hook_proc_;
		sf::timer timer_;
		bool thumb_start_;
		//slider slider_;

		float client_width_, client_height_;

		//IDXGISwapChainPtr dxgi_swap_chain_;
		// std::wstring dxgi_info_;
		
		icon icon_;
		//bool init_;
		sf::window_class_ex child_class_;
		std::unique_ptr<control_base> child_base_, src_path_text_, dest_path_text_, src_path_btn_, dest_path_btn_, render_button_, progress_,
			title_text_, compute_time_,tab_;
		D2D1_SIZE_U icon_size_;
		RECT tab_item_rect_;

		// thisとhwndをつなぐthunkクラス
		// メンバー関数を直接呼び出す。
		//struct hwnd_this_thunk2 : public Xbyak::CodeGenerator {
		//	hwnd_this_thunk2(LONG_PTR this_addr, const void * proc)
		//	{
		//		// 引数の位置をひとつ後ろにずらす
		//		mov(r10, r9);
		//		mov(r9, r8);
		//		mov(r8, rdx);
		//		mov(rdx, rcx);
		//		// thisのアドレスをrcxに格納する
		//		mov(rcx, (LONG_PTR) this_addr);
		//		// 第5引数をスタックに格納
		//		push(r10);
		//		sub(rsp, 32);
		//		mov(r10, (LONG_PTR) proc);
		//		// メンバ関数呼び出し
		//		call(r10);
		//		add(rsp, 40);
		//		ret(0);
		//	}
		//};

		//hwnd_this_thunk2 thunk_info_;
		//  hwnd_this_thunk2 thunk_config_;
		//proc_t proc_info_;
		//  proc_t proc_config_;

	};

	fluidcs11_window::fluidcs11_window(const std::wstring& menu_name, const std::wstring& name, bool fit_to_display, float width, float height)
		: impl_(new impl(menu_name, name, fit_to_display, width, height))
	{

	};

	HWND fluidcs11_window::hwnd() const { return impl_->hwnd(); };
	void fluidcs11_window::create(){ impl_->create(); };
	float fluidcs11_window::width() const { return impl_->width(); }
	float fluidcs11_window::height() const { return impl_->height(); }
	sf::dpi& fluidcs11_window::dpi() { return impl_->dpi(); }
	MARGINS& fluidcs11_window::margins(){ return impl_->margins(); }
	bool fluidcs11_window::is_fullscreen() { return impl_->is_fullscreen(); };
	base_window::on_closed_t& fluidcs11_window::on_closed(){ return impl_->on_closed(); }
	void fluidcs11_window::render(){ impl_->render(); };
	// void fluidcs11_window::player_ready(){impl_->player_ready();};
	void fluidcs11_window::video_bitmap(ID2D1Bitmap1Ptr& bitmap)
	{
		impl_->video_bitmap(bitmap);
	}


	fluidcs11_window_ptr create_fluidcs11_window
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
		RECT    rect = { 0, 0, width, height };
		//::AdjustWindowRectEx( &rect, WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME, FALSE, WS_EX_APPWINDOW | WS_EX_TOPMOST );
		fluidcs11_window* p = new fluidcs11_window(menu_name, name, fit_to_display, rect.right - rect.left, rect.bottom - rect.top);
		p->create();
		return fluidcs11_window_ptr(p);
	}

}

