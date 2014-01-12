#pragma once
#include "sf_windows_base.h"
#include "graphics.h"
namespace sf {
  class window_renderer
  {
  public:
    explicit window_renderer(base_window& window);
    virtual void render();
    virtual ~window_renderer();
    void create_d2d_render_target();
    void discard_d2d_render_target();
    void discard();
    void create_d3d_resources();
    void create_dcomp_resources();
    void create_swap_chain(bool fullscreen = false);
    void init_view_matrix();
    void create_swapchain_dependent_resources();
    void discard_swapchain_dependent_resources();
    void restore_swapchain_and_dependent_resources();
    
    IDCompositionTargetPtr& dcomp_target(){ return dcomp_target_; }
    IDCompositionVisual2Ptr& dcomp_root_visual(){ return dcomp_root_visual_; }

  private:

    base_window& window_;

    ID2D1Bitmap1Ptr d2d1_target_bitmap_;
  
    IDXGISurface2Ptr dxgi_back_buffer_;
    DXGI_MODE_DESC1 mode_desc_;
    DXGI_SWAP_CHAIN_DESC1 swap_chain_desc_;
    DXGI_SWAP_CHAIN_FULLSCREEN_DESC swap_chain_fullscreen_desc_;
    IDXGISwapChain1Ptr dxgi_swap_chain_;

    IDCompositionTargetPtr dcomp_target_;
    IDCompositionVisual2Ptr dcomp_root_visual_;
    IDCompositionRotateTransformPtr rot_, rot_child_;
    IDCompositionScaleTransformPtr scale_;
    IDCompositionTranslateTransformPtr trans_;

  };

  template <typename Renderer = window_renderer,typename SrcHandle = HWND>
  void add_dcomp_content_to_root(Renderer& renderer, SrcHandle src, bool commit = true)
  {
    IDCompositionDesktopDevicePtr& dcomp_desktop_device(graphics::instance()->dcomp_desktop_device());
    IDCompositionDevice2Ptr& dcomp_device2(graphics::instance()->dcomp_device2());
    IDCompositionTargetPtr& dcomp_target(renderer.dcomp_target());
    IDCompositionVisual2Ptr& dcomp_root_visual(renderer.dcomp_root_visual());
    Microsoft::WRL::ComPtr<IUnknown> surf;
    CHK(dcomp_desktop_device->CreateSurfaceFromHwnd(src, &surf));
    IDCompositionVisual2Ptr visual;
    CHK(dcomp_device2->CreateVisual(&visual));
    CHK(visual->SetContent(surf.Get()));
    CHK(dcomp_root_visual->AddVisual(visual.Get(),FALSE,nullptr));
   // CHK(dcomp_target->SetRoot(dcomp_root_visual.Get()));

    if (commit){
      CHK(dcomp_device2->Commit());
      CHK(dcomp_desktop_device->Commit());

    }
  }
}

