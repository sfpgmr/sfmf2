#pragma once
namespace sf{

  struct video_renderer_resources
  {
    video_renderer_resources(int w, int h
    , ID3D11DeviceContext2Ptr& target_ctx
    , ID3D11ShaderResourceViewPtr& target_srv
    , ID3D11RenderTargetViewPtr& target_rtv// �e�N�X�`���p�`��^�[�Q�b�g�Ƃ���
    ,ID3D11DepthStencilViewPtr& target_dv// �e�N�X�`���p�[�x�r���[
    ,ID3D11Texture2DPtr& target_tex// �r�f�I�����_�����O�e�N�X�`��
    ,ID3D11Texture2DPtr& target_depth_tex// �[�x�o�b�t�@�e�N�X�`��
    ,ID2D1Bitmap1Ptr& target_bitmap// Direct2D�p�r�f�I�r�b�g�}�b�v
    ,ID2D1DeviceContext1Ptr& d2d_ctx// Direct2D �R���e�L�X�g
    ) : width(w),height(h)
    , d3d_context(target_ctx), resource_view(target_srv), render_target_view(target_rtv)
    , depth_view(target_dv), texture(target_tex), depth_texture(target_depth_tex), video_bitmap(target_bitmap), d2d_context(d2d_ctx)
    {
    }

    video_renderer_resources(video_renderer_resources& src)
      : width(src.width), height(src.height)
      , d3d_context(src.d3d_context), resource_view(src.resource_view), render_target_view(src.render_target_view)
      , depth_view(src.depth_view), texture(src.texture), depth_texture(src.depth_texture), video_bitmap(src.video_bitmap), d2d_context(src.d2d_context)
    {
    }

    unsigned int width;
    unsigned int height;
    // Direct2D/3D���\�[�X
    ID3D11DeviceContext2Ptr& d3d_context;// �R���e�L�X�g
    ID3D11ShaderResourceViewPtr&    resource_view;// �v���r���[�Ƃ��ĉ�ʂɕ\�����邽�߂ɗp����
    ID3D11RenderTargetViewPtr& render_target_view;// �e�N�X�`���p�`��^�[�Q�b�g�Ƃ���
    ID3D11DepthStencilViewPtr& depth_view;// �e�N�X�`���p�[�x�r���[
    ID3D11Texture2DPtr& texture;// �r�f�I�����_�����O�e�N�X�`��
    ID3D11Texture2DPtr& depth_texture;// �[�x�o�b�t�@�e�N�X�`��
    ID2D1Bitmap1Ptr& video_bitmap;// Direct2D�p�r�f�I�r�b�g�}�b�v
    ID2D1DeviceContext1Ptr& d2d_context;// Direct2D �R���e�L�X�g
  };

  template <typename Renderer>
  class h264_renderer : public Concurrency::agent
  {
  public:
    typedef boost::signals2::signal<void(int progress)> progress_t;
    h264_renderer(std::wstring& source, std::wstring& destination, unsigned int width = 1280, unsigned int height = 720);
    void run();
    virtual ~h264_renderer();
    progress_t& progress();
  private:
    struct impl;
    std::unique_ptr<impl> impl_;
  };
}

