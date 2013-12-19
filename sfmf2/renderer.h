#pragma once
#include "sf_windows_base.h"
namespace sf {

  //class graphics_resources 
  //{
  //private:
  //  ID2D1Factory2Ptr d2d_factory_;
  //  ID2D1DevicePtr d2d_device_;
  //  ID2D1Device1Ptr d2d_device_;
  //  ID2D1DeviceContext1Ptr d2d_context_;
  //  IDWriteFactory2Ptr write_factory_;
  //  IWICImagingFactory2Ptr wic_imaging_factory_;
  //  IDXGIFactory3Ptr dxgi_factory_;
  //  IDXGIAdapter2Ptr dxgi_adapter_;
  //  IDXGIOutput2Ptr dxgi_output_;
  //  IDXGIDevice3Ptr dxgi_device_;
  //};

  class window_renderer
  {
  public:
    explicit window_renderer(base_window& window);
    virtual void render();
    virtual ~window_renderer();
    void create_device_independent_resources();
    void create_device();
    void create_d2d_render_target();
    void discard_d2d_render_target();
    void create_d3d_resources();
    void create_dcomp_resources();
    void create_swap_chain(bool fullscreen = false);
    void init_view_matrix();
    void create_swapchain_dependent_resources();
    void discard_swapchain_dependent_resources();
    void discard_device();
    void discard_device_independant_resources();
    void restore_swapchain_and_dependent_resources();
    void get_dxgi_information();
  private:
    base_window& window_;

    ID2D1Factory2Ptr d2d_factory_;
    ID2D1Bitmap1Ptr d2d1_target_bitmap_;
    ID2D1DevicePtr d2d_device_;
    ID2D1DeviceContext1Ptr d2d_context_;

    IDWriteFactory2Ptr write_factory_;
    IWICImagingFactoryPtr wic_imaging_factory_;
//    IDWriteTextFormatPtr write_text_format_;
    
    IDXGIFactory2Ptr dxgi_factory_;
    IDXGIAdapter2Ptr dxgi_adapter_;
    IDXGIOutput2Ptr dxgi_output_;
    IDXGIDevice3Ptr dxgi_device_;
    IDXGISurface2Ptr dxgi_back_buffer_;
    DXGI_MODE_DESC1 mode_desc_;
    DXGI_SWAP_CHAIN_DESC1 swap_chain_desc_;
    DXGI_SWAP_CHAIN_FULLSCREEN_DESC swap_chain_fullscreen_desc_;
    IDXGISwapChain1Ptr dxgi_swap_chain_;
    std::wstring dxgi_info_;

    ID3D11Device2Ptr d3d_device_;
    ID3D11DeviceContext2Ptr d3d_context_;
    ID3D11Texture2DPtr back_buffer_;

    ID3D11RenderTargetViewPtr d3d_render_target_view_;
    ID3D11Texture2DPtr d3d_depth_texture_;
    ID3D11DepthStencilViewPtr depth_view_;
    ID3D11VertexShaderPtr v_shader_;
    ID3D11InputLayoutPtr input_layout_;
    ID3D11PixelShaderPtr p_shader_;
    ID3D11BufferPtr v_buffer_;
    ID3D11BufferPtr i_buffer_;
    ID3D11BufferPtr cb_never_changes_;
    ID3D11BufferPtr cb_change_on_resize_;
    ID3D11BufferPtr cb_changes_every_frame_;
    ID3D11ShaderResourceViewPtr shader_res_view_;
    ID3D11SamplerStatePtr sampler_state_;


    /*   ID3D11SamplerStatePtr cube_sampler_state_;
    ID3D11Texture2DPtr cube_texture_;
    ID3D11Texture2DPtr cube_depth_texture_;
    ID3D11ShaderResourceViewPtr cube_shader_res_view_;
    ID3D11RenderTargetViewPtr cube_view_;
    ID3D11DepthStencilViewPtr cube_depth_view_;*/

    IDCompositionDesktopDevicePtr dcomp_desktop_device_;
    IDCompositionDevice2Ptr dcomp_device2_;
    IDCompositionTargetPtr dcomp_target_;
    IDCompositionRotateTransformPtr rot_, rot_child_;
    IDCompositionScaleTransformPtr scale_;
    IDCompositionTranslateTransformPtr trans_;

    DirectX::XMMATRIX                            mat_world_;
    DirectX::XMMATRIX                            mat_view_;
    DirectX::XMMATRIX                            mat_projection_;
    DirectX::XMMATRIX                            cube_mat_projection_;

    DirectX::XMFLOAT4                            mesh_color_;
  };

}

