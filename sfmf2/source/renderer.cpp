#include "stdafx.h"
#include "renderer.h"
#include "graphics.h"
#include "application.h"

using namespace sf;
using namespace DirectX;

/////////////////////////////////////////////////////////////////////////////
// Direct X関係のコード
/////////////////////////////////////////////////////////////////////////////

//template<typename Window >
//window_renderer<Window>::window_renderer(Window& window) : window_(window)
//{
//  ZeroMemory(&mode_desc_, sizeof(mode_desc_));
//  ZeroMemory(&swap_chain_desc_, sizeof(swap_chain_desc_));
//  ZeroMemory(&swap_chain_fullscreen_desc_, sizeof(swap_chain_fullscreen_desc_));
//  // スワップチェインの作成
//  create_swap_chain(window_.is_fullscreen());
//  CHK(graphics::instance()->dcomp_desktop_device()->CreateTargetForHwnd((HWND) window_.raw_handle(), TRUE, &dcomp_target_));
//  // スワップチェイン依存リソースの生成
//  create_swapchain_dependent_resources();
//  create_d3d_resources();
//  create_dcomp_resources();
//  // ALT+ENTERを禁止（フルスクリーンのみ）
//  CHK(graphics::instance()->dxgi_factory()->MakeWindowAssociation((HWND) window_.raw_handle(), DXGI_MWA_NO_ALT_ENTER));
//
//}
//
//template<typename Window >
//window_renderer<Window>::~window_renderer()
//{
//}
//
//
//
//template<typename Window >
//void window_renderer<Window>::create_d3d_resources()
//{
//
//}
//
//template<typename Window>
//void window_renderer<Window>::create_dcomp_resources()
//{
//
//}

//
//template<typename Window>
//void window_renderer<Window>::create_swap_chain(bool fullscreen)
//{
//
//}
//
//
//template<typename Window>
//void  window_renderer<Window>::create_d2d_render_target()
//{
//
//}
//
//// Direc2D　描画ターゲットの設定
//
//template<typename Window>
//void  window_renderer<Window>::discard_d2d_render_target()
//{
//}
//
//template<typename Window>
//void  window_renderer<Window>::init_view_matrix()
//{
//
//}
//
//template<typename Window>
//void  window_renderer<Window>::create_swapchain_dependent_resources()
//{
//
//}
//
//template<typename Window>
//void  window_renderer<Window>::discard_swapchain_dependent_resources()
//{
//}
//
//
//template<typename Window>
//void  window_renderer<Window>::discard()
//{
//
//}
//
//// スワップチェインをリストアする
//
//template<typename Window>
//void  window_renderer<Window>::restore_swapchain_and_dependent_resources()
//{
//
//}
//
//template<typename Window>
//void window_renderer<Window>::render()
//{
// }



 


