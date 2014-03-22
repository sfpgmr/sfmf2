#pragma once
#include "sf_windows_base.h"
#include "graphics.h"
namespace sf {

  template<typename Window = sf::base_window>
  class window_renderer
  {
  public:
	  explicit window_renderer(Window& window) : window_(window)
	  {
		  ZeroMemory(&mode_desc_, sizeof(mode_desc_));
		  ZeroMemory(&swap_chain_desc_, sizeof(swap_chain_desc_));
		  ZeroMemory(&swap_chain_fullscreen_desc_, sizeof(swap_chain_fullscreen_desc_));
		  // スワップチェインの作成
		  create_swap_chain(window_.is_fullscreen());
		  CHK(graphics::instance()->dcomp_desktop_device()->CreateTargetForHwnd((HWND) window_, TRUE, &dcomp_target_));
		  // スワップチェイン依存リソースの生成
		  create_swapchain_dependent_resources();
		  create_d3d_resources();
		  create_dcomp_resources();
		  // ALT+ENTERを禁止（フルスクリーンのみ）
		  CHK(graphics::instance()->dxgi_factory()->MakeWindowAssociation((HWND) window_, DXGI_MWA_NO_ALT_ENTER));
	  }
	  virtual void render()
	  {

		  if (GetActiveWindow() == window_.hwnd()){
			  concurrency::critical_section::scoped_lock lock(application::instance()->video_critical_section());
			  RECT rc_frame, rc_window;
			  DwmGetWindowAttribute((HWND) window_, DWMWA_EXTENDED_FRAME_BOUNDS, (PVOID) &rc_frame, sizeof(RECT));
			  GetWindowRect((HWND) window_, &rc_window);

			  rc_frame.left = rc_frame.left - rc_window.left;
			  rc_frame.right = rc_window.right - rc_frame.right;
			  rc_frame.top = rc_frame.top - rc_window.top;
			  rc_frame.bottom = rc_window.bottom - rc_frame.bottom;

			  static float rot = 0.0f;
			  float color[4] = { 0.0f, 0.0f, 0.0f, 0.5f };

			  //auto& d3d_context(graphics::instance()->d3d_context());
			  auto& d2d_context = graphics::instance()->d2d_context();


			  // 描画ターゲットのクリア
			  // d3d_context->ClearRenderTargetView(d3d_render_target_view_.Get(), color);
			  // 深度バッファのクリア
			  //d3d_context->ClearDepthStencilView(depth_view_.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

			  d2d_context->BeginDraw();
			  d2d_context->Clear();


			  D2D_RECT_F layout_rect_ = D2D1::RectF(0.0f, 100.0f, 400.0f, 100.0f);
			  // Text Formatの作成
			  //CHK(write_factory->CreateTextFormat(
			  //  L"ＭＳ ゴシック",                // Font family name.
			  //  NULL,                       // Font collection (NULL sets it to use the system font collection).
			  //  DWRITE_FONT_WEIGHT_REGULAR,
			  //  DWRITE_FONT_STYLE_NORMAL,
			  //  DWRITE_FONT_STRETCH_NORMAL,
			  //  16.000f,
			  //  L"ja-jp",
			  //  &write_text_format_
			  //  ));
			  d2d_context->SetTransform(D2D1::Matrix3x2F::Identity());
			  ID2D1SolidColorBrushPtr brush, line_brush;
			  d2d_context->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::OrangeRed), &brush);
			  d2d_context->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.5f), &line_brush);
			  // d2d_context->CreateBitmapFromDxgiSurface();

			  {
				  D2D_POINT_2F start, end;
				  for (float i = 0; i < window_.width() + 1.0f; i += 16.0f)
				  {
					  start.x = end.x = i;
					  end.y = window_.height();
					  start.y = 0.0f;
					  d2d_context->DrawLine(start, end, line_brush.Get(), 0.5f);
				  }

				  for (float i = 0; i < window_.height() + 1.0f; i += 16.0f)
				  {
					  start.y = end.y = i;
					  end.x = window_.width();
					  start.x = 0.0f;
					  d2d_context->DrawLine(start, end, line_brush.Get(), 0.5f);
				  }

			  }

			  if (video_bitmap_){
				  D2D1_SIZE_F s(video_bitmap_->GetSize());
				  d2d_context->DrawBitmap(video_bitmap_.Get(), D2D1::RectF(30.0f, 250.0f, 320.0f + 30.0f, 200.0f + 250.0f), 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, D2D1::RectF(0.0f, 0.0f, s.width, s.height));
			  }

			  static int count;
			  count++;
			  //std::wstring m((boost::wformat(L"ＡＡＢＢＣＣＤＤＥＥＦＦTEST表示%08d　 ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789アイウエオあいうえお") % count).str());
			  //d2d_context->DrawTextW(
			  //  m.c_str(),
			  //  m.size(),
			  //  write_text_format_.Get(),
			  //  layout_rect_,
			  //  brush.Get());

			  d2d_context->EndDraw();

			  // フリップ

			  DXGI_PRESENT_PARAMETERS parameters = {};
			  static int off = 0;
			  POINT offset = { 0, off-- };
			  RECT srect = { 0, 0, window_.width(), window_.height() };
			  parameters.DirtyRectsCount = 0;
			  parameters.pDirtyRects = nullptr;
			  parameters.pScrollRect = nullptr;
			  parameters.pScrollOffset = nullptr;
			  if (FAILED(dxgi_swap_chain_->Present1(1, 0, &parameters)))
			  {
				  restore_swapchain_and_dependent_resources();
			  };
		  }

	  }
	  virtual ~window_renderer(){}
	  void create_d2d_render_target()
	  {

		  // DXGIサーフェースからDirect2D描画用ビットマップを作成
		  CHK(dxgi_swap_chain_->GetBuffer(0, IID_PPV_ARGS(&dxgi_back_buffer_)));
		  D2D1_BITMAP_PROPERTIES1 bitmap_properties =
			  D2D1::BitmapProperties1(
			  D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
			  D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE),
			  window_.dpi().dpix(),
			  window_.dpi().dpiy()
			  );

		  auto& d2d_context = graphics::instance()->d2d_context();

		  CHK(d2d_context->CreateBitmapFromDxgiSurface(
			  dxgi_back_buffer_.Get(), &bitmap_properties, &d2d1_target_bitmap_));

		  // Direct2D描画ターゲットの設定
		  d2d_context->SetTarget(d2d1_target_bitmap_.Get());
	  }
	  void discard_d2d_render_target()
	  {
		  auto& d2d_context = graphics::instance()->d2d_context();
		  d2d_context->SetTarget(nullptr);
		  d2d1_target_bitmap_.Reset();
	  }
	  void discard()
	  {


		  discard_swapchain_dependent_resources();
		  dxgi_swap_chain_.Reset();

		  graphics::instance()->discard_device();

	  }
	void create_d3d_resources(){}
	void create_dcomp_resources()
	{

		auto& dcomp_device2(graphics::instance()->dcomp_device2());
		auto& d2d_context(graphics::instance()->d2d_context());
		auto& write_factory(graphics::instance()->write_factory());

		IDCompositionSurfacePtr dcomp_surf;
		RECT surf_rect = { 0, 0, 200, 200 };

		CHK(dcomp_device2->CreateSurface(surf_rect.right, surf_rect.bottom, swap_chain_desc_.Format, DXGI_ALPHA_MODE_PREMULTIPLIED, &dcomp_surf));
		IDXGISurfacePtr dxgi_surf;
		POINT offset;
		/*
		CHK(dcomp_surf->BeginDraw(&surf_rect, IID_PPV_ARGS(&dxgi_surf), &offset));

		D2D1_BITMAP_PROPERTIES1 bitmap_prop = D2D1::BitmapProperties1(
		D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
		D2D1::PixelFormat(swap_chain_desc_.Format, D2D1_ALPHA_MODE_PREMULTIPLIED),
		window_.dpi().dpix(),
		window_.dpi().dpiy());

		ID2D1ImagePtr backup;
		ID2D1Bitmap1Ptr d2dtarget_bitmap;
		CHK(d2d_context->CreateBitmapFromDxgiSurface(dxgi_surf.Get(), &bitmap_prop, &d2dtarget_bitmap));

		d2d_context->GetTarget(&backup);
		d2d_context->SetTarget(d2dtarget_bitmap.Get());

		ID2D1SolidColorBrushPtr brush, tbrush;
		float alpha = 1.0f;
		CHK(d2d_context->CreateSolidColorBrush(D2D1::ColorF(1.0f, 0.0f, 0.0f, alpha), &brush));
		CHK(d2d_context->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f), &tbrush));
		IDWriteTextFormatPtr format;
		// Text Formatの作成
		CHK(write_factory->CreateTextFormat(
		L"メイリオ",                // Font family name.
		NULL,                       // Font collection (NULL sets it to use the system font collection).
		DWRITE_FONT_WEIGHT_REGULAR,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		12.000f,
		L"ja-jp",
		&format));

		d2d_context->BeginDraw();

		float w = surf_rect.right / 2.0f, h = surf_rect.bottom / 2.0f;
		std::wstring t = L"DirectComposition1";

		d2d_context->FillRectangle(D2D1::RectF(0.0f, 0.0f, w, h), brush.Get());
		d2d_context->DrawTextW(t.c_str(), t.size(), format.Get(), D2D1::RectF(0.0f, 0.0f, w, h), tbrush.Get());

		brush->SetColor(D2D1::ColorF(0.0f, 1.0f, 0.0f, alpha));
		d2d_context->FillRectangle(D2D1::RectF(w, 0.0f, w + w, h), brush.Get());
		t = L"DirectComposition2";
		d2d_context->DrawTextW(t.c_str(), t.size(), format.Get(), D2D1::RectF(w, 0.0f, w + w, h), tbrush.Get());
		brush->SetColor(D2D1::ColorF(0.0f, 0.0f, 1.0f, alpha));
		t = L"DirectComposition4";
		d2d_context->FillRectangle(D2D1::RectF(w, h, w + w, h + h), brush.Get());
		d2d_context->DrawTextW(t.c_str(), t.size(), format.Get(), D2D1::RectF(w, h, w + w, h + h), tbrush.Get());
		brush->SetColor(D2D1::ColorF(1.0f, 0.0f, 1.0f, alpha));
		d2d_context->FillRectangle(D2D1::RectF(0, h, w, h + h), brush.Get());
		t = L"DirectComposition3";
		d2d_context->DrawTextW(t.c_str(), t.size(), format.Get(), D2D1::RectF(0, h, w, h + h), tbrush.Get());

		d2d_context->EndDraw();
		dcomp_surf->EndDraw();

		brush.Reset();
		*/

		IDCompositionVisual2Ptr v, v1, v2, v3, v4, v5;
		CHK(dcomp_device2->CreateVisual(&v));
		dcomp_root_visual_ = v;
		CHK(v->SetContent(dcomp_surf.Get()));

		// CHK(dcomp_device2->CreateVisual(&v5));
		// CHK(v5->SetContent(dxgi_swap_chain_.Get()));


		//      v->SetOffsetX(width_ / 2.0f);
		//      v->SetOffsetY(height_ / 5.0f);

		dcomp_target_->SetRoot(v.Get());

		/*
		CHK(dcomp_device2->CreateVisual(&v1));
		CHK(v1->SetContent(dcomp_surf.Get()));
		CHK(dcomp_device2->CreateVisual(&v2));
		CHK(v2->SetContent(dcomp_surf.Get()));
		CHK(dcomp_device2->CreateVisual(&v3));
		CHK(v3->SetContent(dcomp_surf.Get()));
		CHK(dcomp_device2->CreateVisual(&v4));
		CHK(v4->SetContent(dcomp_surf.Get()));

		v1->SetOffsetY(window_.height() / 5.0f);
		v2->SetOffsetY(window_.height() / 5.0f - h);
		v3->SetOffsetY(window_.height() / 5.0f);
		v4->SetOffsetY(window_.height() / 5.0f - h);

		v1->SetOffsetX(w * 2.0f);
		v2->SetOffsetX(w * 3.0f);
		v3->SetOffsetX(w * 3.0f);
		v4->SetOffsetX(w * 4.0f);

		IDCompositionRectangleClipPtr clip;
		dcomp_device2->CreateRectangleClip(&clip);
		clip->SetLeft(0.0f);
		clip->SetRight(w - 1.0f);
		clip->SetTop(0.0f);
		clip->SetBottom(h - 1.0f);
		CHK(clip->SetTopLeftRadiusX(3.0f));
		CHK(clip->SetBottomLeftRadiusX(3.0f));

		//v1->SetClip(D2D1::RectF(0.0f,0.0f,w-1.0f,h-1.0f));
		v1->SetClip(clip.Get());
		v2->SetClip(D2D1::RectF(0.0f, h, w - 1.0f, h + h - 1.0f));
		v3->SetClip(D2D1::RectF(w, 0.0f, w + w - 1.0f, h - 1.0f));
		v4->SetClip(D2D1::RectF(w, h, w + w - 1.0f, h + h - 1.0f));

		//v->AddVisual(v5.Get(), FALSE, nullptr);
		v->AddVisual(v1.Get(), FALSE, nullptr);
		v->AddVisual(v2.Get(), FALSE, nullptr);
		v->AddVisual(v3.Get(), FALSE, nullptr);
		v->AddVisual(v4.Get(), FALSE, nullptr);

		// 2D Transform のセットアップ
		{
		IDCompositionTransform* transforms[3];

		IDCompositionTransformPtr transform_group;

		CHK(dcomp_device2->CreateRotateTransform(&rot_));
		CHK(dcomp_device2->CreateRotateTransform(&rot_child_));
		CHK(dcomp_device2->CreateScaleTransform(&scale_));
		CHK(dcomp_device2->CreateTranslateTransform(&trans_));


		rot_->SetCenterX(w);
		rot_->SetCenterY(h);
		rot_->SetAngle(0.0f);

		rot_child_->SetCenterX(w / 2.0f);
		rot_child_->SetCenterY(w / 2.0f);

		scale_->SetCenterX(w);
		scale_->SetCenterY(h);
		scale_->SetScaleX(2.0f);
		scale_->SetScaleY(2.0f);

		trans_->SetOffsetX((window_.width() - surf_rect.right) / 2.0f);
		trans_->SetOffsetY((window_.height() - surf_rect.bottom) / 2.0f);

		transforms[0] = rot_.Get();
		transforms[1] = scale_.Get();
		transforms[2] = trans_.Get();

		CHK(dcomp_device2->CreateTransformGroup(transforms, 3, &transform_group));
		//v5->SetTransform(transform_group.Get());
		v1->SetTransform(transform_group.Get());
		v2->SetTransform(transform_group.Get());
		v3->SetTransform(rot_child_.Get());
		v4->SetTransform(rot_child_.Get());
		}

		{
		// Opacityのアニメーション
		IDCompositionAnimationPtr anim;
		CHK(dcomp_device2->CreateAnimation(&anim));
		anim->AddCubic(0.0f, 0.0f, 1.0f / 4.0f, 0.0f, 0.0f);
		anim->AddCubic(4.0f, 1.0f, -1.0f / 4.0f, 0.0f, 0.0f);
		anim->AddRepeat(8.0f, 8.0f);
		//anim->End(10.0f,0.0f);

		IDCompositionEffectGroupPtr effect;
		dcomp_device2->CreateEffectGroup(&effect);
		effect->SetOpacity(anim.Get());

		IDCompositionAnimationPtr anim3d;
		CHK(dcomp_device2->CreateAnimation(&anim3d));
		anim3d->AddCubic(0.0f, 0.0f, 360.0f / 8.0f, 0.0f, 0.0f);
		anim3d->AddRepeat(8.0f, 8.0f);

		IDCompositionRotateTransform3DPtr rot3d;
		dcomp_device2->CreateRotateTransform3D(&rot3d);
		rot3d->SetAngle(anim3d.Get());
		rot3d->SetAxisZ(0.0f);
		rot3d->SetAxisY(0.0f);
		rot3d->SetAxisX(1.0f);
		rot3d->SetCenterX(w);
		rot3d->SetCenterY(w);

		//   rot3d->SetAxisX(1.0f);

		effect->SetTransform3D(rot3d.Get());

		// 3D変換のアニメーション

		v1->SetEffect(effect.Get());
		//v5->SetEffect(effect.Get());
		}
		*/
		dcomp_device2->Commit();
		graphics::instance()->dcomp_desktop_device()->Commit();

		//d2d_context->SetTarget(backup.Get());

	}
	void create_swap_chain(bool fullscreen = false)
	{
		auto& d3d_context(graphics::instance()->d3d_context());
		auto& d3d_device(graphics::instance()->d3d_device());

		// スワップチェーンの作成

		//RECT rect;
		//::GetWindowRect(hwnd, &rect);
		swap_chain_desc_.Width = window_.width();
		swap_chain_desc_.Height = window_.height();
		swap_chain_desc_.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		swap_chain_desc_.Scaling = DXGI_SCALING_NONE;//DXGI_SCALING_STRETCH
		swap_chain_desc_.Stereo = 0;
		swap_chain_desc_.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		swap_chain_desc_.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swap_chain_desc_.BufferCount = 2;
		//desc.SampleDesc = msaa;
		swap_chain_desc_.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
		//    swap_chain_desc_.Flags = DXGI_SWAP_CHAIN_//DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		swap_chain_desc_.SampleDesc.Count = 1;

		swap_chain_fullscreen_desc_.RefreshRate.Numerator = 60;
		swap_chain_fullscreen_desc_.RefreshRate.Denominator = 1;
		swap_chain_fullscreen_desc_.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swap_chain_fullscreen_desc_.Windowed = fullscreen ? FALSE : TRUE;
		swap_chain_fullscreen_desc_.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

		CHK(graphics::instance()->dxgi_factory()->CreateSwapChainForHwnd(d3d_device.Get(), (HWND) window_, &swap_chain_desc_, &swap_chain_fullscreen_desc_, graphics::instance()->dxgi_output().Get(), &dxgi_swap_chain_));
		//  CHK(dxgi_factory_->CreateSwapChainForComposition(d3d_device.Get(), &swap_chain_desc_, dxgi_output_.Get(), &dxgi_swap_chain_));

	}
    void init_view_matrix();
	void create_swapchain_dependent_resources()
	{

		auto& d3d_context(graphics::instance()->d3d_context());
		auto& d3d_device(graphics::instance()->d3d_device());

		create_d2d_render_target();

		// Direct 3Dリソースの作成 /////////////////////////////////////////

	}
	void discard_swapchain_dependent_resources(){


		if (window_.is_fullscreen()){
			dxgi_swap_chain_->SetFullscreenState(FALSE, nullptr);
		}

		discard_d2d_render_target();

		dxgi_back_buffer_.Reset();
	}
	void restore_swapchain_and_dependent_resources()
	{

		// フルスクリーンからいったんウィンドウモードに戻す
		if (window_.is_fullscreen())
		{
			CHK(dxgi_swap_chain_->GetFullscreenState(FALSE, nullptr));
		}

		// スワップチェインに依存しているリソースを解放する
		discard_swapchain_dependent_resources();

		CHK(dxgi_swap_chain_->ResizeBuffers(2, swap_chain_desc_.Width, swap_chain_desc_.Height, swap_chain_desc_.Format, swap_chain_desc_.Flags));

		// フルスクリーンの場合、もとに戻す。
		if (window_.is_fullscreen()){
			BOOL f = window_.is_fullscreen() ? TRUE : FALSE;
			CHK(dxgi_swap_chain_->GetFullscreenState(&f, nullptr));
		}

		// スワップチェインに依存しているリソースを再作成する
		create_swapchain_dependent_resources();
	}
    
    IDCompositionTargetPtr& dcomp_target(){ return dcomp_target_; }
    IDCompositionVisual2Ptr& dcomp_root_visual(){ return dcomp_root_visual_; }
    ID2D1Bitmap1Ptr& video_bitmap(){ return video_bitmap_; };
  private:

	Window& window_;

    ID2D1Bitmap1Ptr d2d1_target_bitmap_;
    ID2D1Bitmap1Ptr video_bitmap_;
  
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

