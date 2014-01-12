#pragma once
namespace sf{

  struct video_renderer_resources
  {
    video_renderer_resources(int w, int h
    , ID3D11DeviceContext2Ptr& target_ctx
    , ID3D11ShaderResourceViewPtr& target_srv
    , ID3D11RenderTargetViewPtr& target_rtv// テクスチャ用描画ターゲットとする
    ,ID3D11DepthStencilViewPtr& target_dv// テクスチャ用深度ビュー
    ,ID3D11Texture2DPtr& target_tex// ビデオレンダリングテクスチャ
    ,ID3D11Texture2DPtr& target_depth_tex// 深度バッファテクスチャ
    ,ID2D1Bitmap1Ptr& target_bitmap// Direct2D用ビデオビットマップ
    ,ID2D1DeviceContext1Ptr& d2d_ctx// Direct2D コンテキスト
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
    // Direct2D/3Dリソース
    ID3D11DeviceContext2Ptr& d3d_context;// コンテキスト
    ID3D11ShaderResourceViewPtr&    resource_view;// プレビューとして画面に表示するために用いる
    ID3D11RenderTargetViewPtr& render_target_view;// テクスチャ用描画ターゲットとする
    ID3D11DepthStencilViewPtr& depth_view;// テクスチャ用深度ビュー
    ID3D11Texture2DPtr& texture;// ビデオレンダリングテクスチャ
    ID3D11Texture2DPtr& depth_texture;// 深度バッファテクスチャ
    ID2D1Bitmap1Ptr& video_bitmap;// Direct2D用ビデオビットマップ
    ID2D1DeviceContext1Ptr& d2d_context;// Direct2D コンテキスト
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

