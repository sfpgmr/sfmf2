#pragma once
#include "video_renderer.h"

namespace sf{
  class wavelet_renderer_base
  {
  public:
    struct init_params_t {
      std::wstring title;
      int sample_length;
    };
    explicit wavelet_renderer_base(sf::video_renderer_resources& res, init_params_t& p);

    void init_view_matrix();
    void discard();
    void render(LONGLONG t, INT16* wave_data, int length);
    void render(LONGLONG t, int samplepos, audio_samples_t& audio_samples);
    virtual ~wavelet_renderer_base();
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


    std::wstring                                    text_;
    DWRITE_TEXT_METRICS	                            text_metrics_;
    ID2D1SolidColorBrushPtr white_brush_;
    ID2D1DrawingStateBlockPtr state_;
    IDWriteTextLayoutPtr       text_layout_;
    IDWriteTextFormatPtr		text_format_;

    DirectX::XMMATRIX mat_world_;
    DirectX::XMMATRIX mat_view_;
    DirectX::XMMATRIX mat_projection_;
    DirectX::XMFLOAT4 mesh_color_;

    int length_;

    std::unique_ptr<double []> wt_;
    std::unique_ptr<double []> wt_1_;

    int buffer_index_;

  };

  typedef h264_renderer<wavelet_renderer_base> wavelet_renderer;
}

