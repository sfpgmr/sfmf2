#pragma once
#include "video_renderer.h"

namespace sf{
  class test_renderer_base
  {
  public:
    explicit test_renderer_base(sf::video_renderer_resources& res);
    void init_view_matrix();
    void discard();
    void render(LONGLONG t);
    virtual ~test_renderer_base();
  private:
    sf::video_renderer_resources res_;
    LONGLONG time_;
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

    DirectX::XMMATRIX mat_world_;
    DirectX::XMMATRIX mat_view_;
    DirectX::XMMATRIX mat_projection_;
    DirectX::XMFLOAT4 mesh_color_;

  };

  typedef h264_renderer<test_renderer_base> test_renderer;
}

