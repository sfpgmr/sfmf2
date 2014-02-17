#include "stdafx.h"
#include "graphics.h"
#include "application.h"
#include "csocean_renderer.h"
#include "fft4g.h"

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

using namespace sf;
using namespace DirectX;
using namespace DirectX::SimpleMath;


csocean_renderer_base::csocean_renderer_base(video_renderer_resources& res, csocean_renderer_base::init_params_t& p) : text_(p.title), res_(res), time_(0)
{
  //m_fSize = 1.0f;
  ///////////////////////////////////////////////////////////////////
  // Direct3Dリソースの生成
  ///////////////////////////////////////////////////////////////////

  auto& d3d_context(res_.d3d_context);
  auto& d3d_device(graphics::instance()->d3d_device());
  // Direct 3Dリソースの生成
  HRESULT hr;
  DWORD compile_flag = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
  compile_flag |= D3DCOMPILE_DEBUG;
#endif

  // Compile the Shaders

  //std::wstring render_path(application::instance()->base_directory() + L"\\FluidRender.fx");


  // Setup the camera's view parameters
  eye_ = Vector4(1562.24f, 854.291f, -1224.99f,0.0f);
  at_ = Vector4(1562.91f, 854.113f, -1225.71f,0.0f);
  up_ = Vector4(0.0f, 1.0f, 0.0f, 0.0f);

  init_view_matrix();

  //  g_Camera.SetViewParams(&vecEye, &vecAt);

  // Create an OceanSimulator object and D3D11 rendering resources
  CreateOceanSimAndRender();

  // Sky box
  std::wstring tex_path(app_base_directory::instance()->resource_dir() + L"sky_cube.dds");
  CreateDDSTextureFromFile(d3d_device.Get(), tex_path.c_str(), nullptr, g_pSRV_SkyCube.GetAddressOf());
  g_pSRV_SkyCube->GetResource((ID3D11Resource**) g_pSkyCubeMap.GetAddressOf());

  g_Skybox.reset(new CSkybox11(d3d_device, d3d_context, 50.0f, g_pSkyCubeMap, g_pSRV_SkyCube, res_.width, res_.height));

  // OMステージに登録する
  d3d_context->OMSetRenderTargets(1, res_.render_target_view.GetAddressOf(), res_.depth_view.Get());

  // ビューポートの設定
  D3D11_VIEWPORT vp;
  vp.Width = res_.width;//client_width_;
  vp.Height = res_.height;//client_height_;
  vp.MinDepth = 0.0f;
  vp.MaxDepth = 1.0f;
  vp.TopLeftX = 0;
  vp.TopLeftY = 0;
  d3d_context->RSSetViewports(1, &vp);

  ID3D11RasterizerState* hpRasterizerState = NULL;
  D3D11_RASTERIZER_DESC hRasterizerDesc = {
    D3D11_FILL_SOLID,
    D3D11_CULL_NONE,	//ポリゴンの裏表を無くす
    FALSE,
    0,
    0.0f,
    FALSE,
    FALSE,
    FALSE,
    FALSE,
    FALSE
  };

  d3d_device->CreateRasterizerState(&hRasterizerDesc, &hpRasterizerState);
  d3d_context->RSSetState(hpRasterizerState);

    // Direct 2D リソースの作成

	res_.d2d_context->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &white_brush_);

	ZeroMemory(&text_metrics_, sizeof(DWRITE_TEXT_METRICS));

	// デバイスに依存するリソースを作成します。
	CHK(
		graphics::instance()->write_factory()->CreateTextFormat(
		L"Meiryo",
		nullptr,
		DWRITE_FONT_WEIGHT_LIGHT,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		48.0f,
		L"en-US",
		&text_format_
		)
		);

	CHK(text_format_->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR));

	CHK(graphics::instance()->d2d_factory()->CreateDrawingStateBlock(&state_));

	//  title(t);

	application::instance()->video_bitmap(res.video_bitmap);

	//  title(L".WAVファイルからM4Vファイルを生成するサンプル");


	//D2D1::Matrix3x2F mat2d = D2D1::Matrix3x2F::Rotation(90.0f);
	//mat2d.Invert();
	// mat2d._22 = - 1.0f;

	//res_.d2d_context->SetTransform(mat2d);

	//init_ = true;// 初期化完了
	CHK(
		graphics::instance()->write_factory()->CreateTextLayout(
		text_.c_str(),
		(uint32) text_.length(),
		text_format_.Get(),
		res_.width, // 入力テキストの最大幅。
		40.0f, // 入力テキストの最大高さ。
		&text_layout_
		)
		);
	CHK(text_layout_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING));
	CHK(text_layout_->GetMetrics(&text_metrics_));

	// 1 Tick当たりのサンプル数を2のべき乗数に丸める
	length_ = 1;
	for (int i = 0; i < 32; ++i){
		length_ = length_ << 1;
		if (length_ > p.sample_length){
			length_ = length_ >> 2;
			break;
		}
	}

  // FFT用変数の初期化

	a_.reset(new double[length_]);
	a_1_.reset(new double[length_]);
	w_.reset(new double[length_ * 5 / 4]);
	w_1_.reset(new double[length_ * 5 / 4]);
	w_[0] = w_1_[0] = 0;
	ip_.reset(new int[(int) ceil(2.0 + sqrt((double) length_))]);
	ip_1_.reset(new int[(int) ceil(2.0 + sqrt((double) length_))]);
	ip_[0] = ip_1_[0] = 0;
	log_.reset(new double[length_ / 2 + 1]);
	log_1_.reset(new double[length_ / 2 + 1]);

  // オブジェクトの生成チェック

  assert(g_pMeshIB);
  assert(g_pMeshVB);
  assert(g_pMeshLayout);

  assert(g_pOceanSurfVS);
  assert(g_pOceanSurfPS);
  assert(g_pWireframePS);

  assert(g_pFresnelMap);
  assert(g_pSRV_Fresnel);
  assert(g_pSRV_Perlin);
  assert(g_pSRV_ReflectCube);

  assert(g_pHeightSampler);
  assert(g_pGradientSampler);
  assert(g_pFresnelSampler);
  assert(g_pPerlinSampler);
  assert(g_pCubeSampler);

  assert(g_pPerCallCB);
  // assert(g_pPerFrameCB);
  assert(g_pShadingCB);

  assert(g_pRSState_Solid);
  assert(g_pRSState_Wireframe);
  assert(g_pDSState_Disable);
  assert(g_pBState_Transparent);

}



void csocean_renderer_base::init_view_matrix()
{
	auto& d3d_context(res_.d3d_context);

  mat_world_ = XMMatrixIdentity();

	// ビュー行列のセットアップ
	mat_view_ = XMMatrixLookAtLH(eye_, at_, up_);
 // //	cb_never_changes cnc;
	//cnc.mView = XMMatrixTra//nspose(mat_view_);
	////cnc.vLightColor = SimpleMath::Vector4( 1.0f, 0.5f, 0.5f, 1.0f );
	//cnc.vLightDir = SimpleMath::Vector4(0.577f, 0.577f, -0.977f, 1.0f);
	//// 定数バッファに格納
	//d3d_context->UpdateSubresource(cb_never_changes_.Get(), 0, NULL, &cnc, 0, 0);

	// 投影行列のセットアップ
	mat_projection_ = XMMatrixPerspectiveFovLH(XM_PIDIV4, (float) res_.width / (float) res_.height, 0.01f, 20000.0f);
	//cb_change_on_resize ccor;
	//ccor.mProjection = XMMatrixTranspose(mat_projection_);
	//// 定数バッファに格納
	//d3d_context->UpdateSubresource(cb_change_on_resize_.Get(), 0, NULL, &ccor, 0, 0);
}

void csocean_renderer_base::render(LONGLONG t, int samplepos, audio_samples_t& audio_samples)
{

  auto& d3d_context(res_.d3d_context);

  // OMステージに登録する
  d3d_context->OMSetRenderTargets(1, res_.render_target_view.GetAddressOf(), res_.depth_view.Get());

  float color[4] = { 0.1f, 0.2f, 0.4f, 0.0f };
  d3d_context->ClearRenderTargetView(res_.render_target_view.Get(), color);
  d3d_context->ClearDepthStencilView(res_.depth_view.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

  // Sky box rendering
  Matrix mView = Matrix(1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1) * mat_view_;
  Matrix mProj = mat_projection_;
  Matrix mWorldViewProjection = mView * mProj;

  if (!g_RenderWireframe)
    g_Skybox->D3D11Render(mWorldViewProjection, d3d_context);

  // Time
  float fTime = (double) t / (double) MFCLOCK_FREQUENCY_HNS;
  static double app_time = fTime;
  static double app_prev_time = fTime;
  if (g_PauseSimulation == false)
    app_time += fTime - app_prev_time;
  app_prev_time = fTime;

  g_pOceanSimulator->updateDisplacementMap(app_time,d3d_context);

  // Ocean rendering
 // ID3D11ShaderResourceView* tex_displacement = g_pOceanSimulator->getD3D11DisplacementMap();
 // ID3D11ShaderResourceView* tex_gradient = g_pOceanSimulator->getD3D11GradientMap();
  d3d_context->OMSetRenderTargets(1, res_.render_target_view.GetAddressOf(), res_.depth_view.Get());

  if (g_RenderWireframe)
    renderWireframe(g_pOceanSimulator->getD3D11DisplacementMap(), (float) app_time );
  else
    renderShaded(g_pOceanSimulator->getD3D11DisplacementMap(), g_pOceanSimulator->getD3D11GradientMap(), (float) app_time );
//  while (elapsed_time > 0.0f)
//  {
////    SimulateFluid(elapsed_time);
////    elapsed_time -= g_fMaxAllowableTimeStep;
//  }
//  //RenderFluid(elapsed_time);


  ID2D1DeviceContext* context = res_.d2d_context.Get();

  context->SetTarget(res_.video_bitmap.Get());
  context->SaveDrawingState(state_.Get());
  context->BeginDraw();

  //// 右下隅に配置
  //D2D1::Matrix3x2F screenTranslation = D2D1::Matrix3x2F::Translation(
  //  0.0f, logicalSize.Height);
  ////		logicalSize.Height - m_textMetrics.layoutHeight
  ////	);

  ////D2D1::Matrix3x2F screenTranslation = D2D1::Matrix3x2F::Translation(0.0f,0.0f);
  //screenTranslation._22 = screenTranslation._22 * -1.0f;

  //context->SetTransform(screenTranslation * m_deviceResources->GetOrientationTransform2D());

  // FFTの実行

  int size = audio_samples[0].size();
  int start = samplepos;
  int end = samplepos + length_;
  for (int i = start,j = 0; i < end; ++i,++j)
  {
    if (i < size)
    {
      // ハニング窓
      a_[j] = han_window(i, length_) * audio_samples[0][i];
      a_1_[j] = han_window(i, length_) * audio_samples[1][i];
      //DOUT(boost::wformat(L"%f\t%f\t%d\n") % a_[i] % (double(wave_data[i * 2]) / 32768.0) % wave_data[i * 2]);
    }
    else {
      a_[j] = 0;
      a_1_[j] = 0;
    }
  }

  ip_[0] = ip_1_[0] = 0;
  w_[0] = w_1_[0] = 0;
  rdft(length_, 1, a_.get(), ip_.get(), w_.get()); // 離散フーリエ変換
  rdft(length_, 1, a_1_.get(), ip_1_.get(), w_1_.get()); // 離散フーリエ変換

  log_[0] = log10(a_[0]) / length_;
  log_1_[0] = log10(a_1_[0]) / length_;
  log_[length_ / 2] = log10(a_[1]) / length_;
  log_1_[length_ / 2] = log10(a_1_[1]) / length_;

  for (int i = 1, end = length_ / 2; i < end; ++i)
  {
    /*		log_[i] = log(power(a_[i * 2], a_[i * 2 + 1])) / length_ * 5000;
    log_1_[i] = log(power(a_1_[i * 2], a_1_[i * 2 + 1])) / length_ * 5000;*/
    log_[i] = 20.0 * log10(power(a_[i * 2], a_[i * 2 + 1])) /*/ length_ * 25000*/;
    log_1_[i] =  20.0 * log10(power(a_1_[i * 2], a_1_[i * 2 + 1]))/* / length_ * 25000*/;
    //   DOUT(boost::wformat(L"%f\t%f\t%f\n") % a_[i * 2] % a_[i * 2 + 1] % (log_[i]));
    //TRACE(L"%f\t%f\t%e\n", wavdata[i*2], wavdata[i*2+1], Adft_log[i]);
  }

  // 波形データを表示する

//  {
//    /*******/
//    const double delta = (144.0 - 36.0) / res_.width;
//    const float delta2 = res_.width / (144.0 - 36.0);
//    const int li = length_ / 2;
//
//    int pos = 0,pos_bkp = 0;
//    double note = 36;
//    double min_db = -40.0;
//    const double  step = 44100. / (double(length_) / 2.);
//    for (float i = 0; i < res_.width; ++i){
//      pos =  (int)(pow(2, (note - 69.0) / 12.) * 440. / step);
//      if (pos >= length_ / 2) break;
//
//      double l = (((log_[pos] - 20.0) < min_db) ? min_db : (log_[pos] - 20.0));
//      double r = (((log_1_[pos] - 20.0)  < min_db) ? min_db : (log_1_[pos] - 20.0));
//
//      float left_top = (-l) * 5  + 150.0;
//      float right_top = (-r) * 5 + 500.0;
//
////      white_brush_->SetColor(hsv2rgb(270.0 + l * 140. / -min_db, 0.99, 0.99, (0.99 + l / -min_db)*0.5));
//      white_brush_->SetColor(D2D1::ColorF(1.0f, 1.0f, 1.0f, (0.99 + l / -min_db)*0.7));
//      context->FillRectangle(D2D1::RectF(i, left_top, i + 1, 350.0f),/*::Point2F(i, 250.0f), D2D1::Point2F(i, left), */ white_brush_.Get());
//      white_brush_->SetColor(D2D1::ColorF(1.0f, 1.0f, 1.0f, (0.99 + r / -min_db)*0.7));
////      white_brush_->SetColor(hsv2rgb(270.0 + r * 140. / -min_db, 0.99, 0.99, (0.99 + r / -min_db)*0.5));
//      context->FillRectangle(D2D1::RectF(i, right_top, i + 1, 700.0f),/*::Point2F(i, 250.0f), D2D1::Point2F(i, left), */ white_brush_.Get());
//      //      context->DrawLine(D2D1::Point2F(i, 550.0f), D2D1::Point2F(i, right), white_brush_.Get());
//      note += delta;
//    }
  //  //white_brush_->SetColor(D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.5f));
  //  //for (float i = 0; i < res_.width; i+= delta2)
  //  //{
  //  //  context->DrawLine(D2D1::Point2F(i, 0.0f), D2D1::Point2F(i, res_.height), white_brush_.Get());
  //  //}
  //}

 
  white_brush_->SetColor(D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.5f));
  context->DrawTextLayout(
    D2D1::Point2F(0.f, 0.f),
    text_layout_.Get(),
    white_brush_.Get()
    );

  // D2DERR_RECREATE_TARGET をここで無視します。このエラーは、デバイスが失われたことを示します。
  // これは、Present に対する次回の呼び出し中に処理されます。
  HRESULT hr = context->EndDraw();
  if (hr != D2DERR_RECREATE_TARGET)
  {
    CHK(hr);
  }

  context->RestoreDrawingState(state_.Get());

}

void csocean_renderer_base::render(LONGLONG time, INT16* wave_data, int length)
{
  auto& d3d_context(res_.d3d_context);
}



csocean_renderer_base::~csocean_renderer_base()
{
  cleanupRenderResource();
}

void csocean_renderer_base::initRenderResource(const OceanParameter& ocean_param)
{
  auto& d3d_context(res_.d3d_context);
  auto& d3d_device(graphics::instance()->d3d_device());

  g_PatchLength = ocean_param.patch_length;
  g_DisplaceMapDim = ocean_param.dmap_dim;
  g_WindDir = ocean_param.wind_dir;

  // D3D buffers
  createSurfaceMesh();
  createFresnelMap();
  loadTextures();

  // HLSL
  // Vertex & pixel shaders
  {
    std::wstring shader_path(app_base_directory::instance()->shader_dir() + L"ocean_shading.hlsl");
    ID3DBlobPtr pBlobOceanSurfVS;

    sf::create_shader_blob_from_file(shader_path, "OceanSurfVS", "vs_4_0", pBlobOceanSurfVS);
    sf::create_shader_from_file(d3d_device,shader_path, "OceanSurfPS", "ps_4_0", g_pOceanSurfPS);
    sf::create_shader_from_file(d3d_device,shader_path, "WireframePS", "ps_4_0", g_pWireframePS);

    sf::create_shader(d3d_device, pBlobOceanSurfVS->GetBufferPointer(), pBlobOceanSurfVS->GetBufferSize(), NULL, g_pOceanSurfVS);

    // Input layout
    D3D11_INPUT_ELEMENT_DESC mesh_layout_desc[] =
    {
      { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    CHK(d3d_device->CreateInputLayout(mesh_layout_desc, 1, pBlobOceanSurfVS->GetBufferPointer(), pBlobOceanSurfVS->GetBufferSize(), &g_pMeshLayout));
  }

  // Constants
  D3D11_BUFFER_DESC cb_desc;
  cb_desc.Usage = D3D11_USAGE_DYNAMIC;
  cb_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  cb_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
  cb_desc.MiscFlags = 0;
  cb_desc.ByteWidth = PAD16(sizeof(Const_Per_Call));
  cb_desc.StructureByteStride = 0;
  CHK(d3d_device->CreateBuffer(&cb_desc, NULL, &g_pPerCallCB));

  Const_Shading shading_data;
  // Grid side length * 2
  shading_data.g_TexelLength_x2 = g_PatchLength / g_DisplaceMapDim * 2;;
  // Color
  shading_data.g_SkyColor = g_SkyColor;
  shading_data.g_WaterbodyColor = g_WaterbodyColor;
  // Texcoord
  shading_data.g_UVScale = 1.0f / g_PatchLength;
  shading_data.g_UVOffset = 0.5f / g_DisplaceMapDim;
  // Perlin
  shading_data.g_PerlinSize = g_PerlinSize;
  shading_data.g_PerlinAmplitude = g_PerlinAmplitude;
  shading_data.g_PerlinGradient = g_PerlinGradient;
  shading_data.g_PerlinOctave = g_PerlinOctave;
  // Multiple reflection workaround
  shading_data.g_BendParam = g_BendParam;
  // Sun streaks
  shading_data.g_SunColor = g_SunColor;
  shading_data.g_SunDir = g_SunDir;
  shading_data.g_Shineness = g_Shineness;

  D3D11_SUBRESOURCE_DATA cb_init_data;
  cb_init_data.pSysMem = &shading_data;
  cb_init_data.SysMemPitch = 0;
  cb_init_data.SysMemSlicePitch = 0;

  cb_desc.Usage = D3D11_USAGE_IMMUTABLE;
  cb_desc.CPUAccessFlags = 0;
  cb_desc.ByteWidth = PAD16(sizeof(Const_Shading));
  cb_desc.StructureByteStride = 0;
  CHK(d3d_device->CreateBuffer(&cb_desc, &cb_init_data, &g_pShadingCB));

  // Samplers
  D3D11_SAMPLER_DESC sam_desc;
  sam_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
  sam_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
  sam_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
  sam_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
  sam_desc.MipLODBias = 0;
  sam_desc.MaxAnisotropy = 1;
  sam_desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
  sam_desc.BorderColor[0] = 1.0f;
  sam_desc.BorderColor[1] = 1.0f;
  sam_desc.BorderColor[2] = 1.0f;
  sam_desc.BorderColor[3] = 1.0f;
  sam_desc.MinLOD = 0;
  sam_desc.MaxLOD = FLT_MAX;
  CHK(d3d_device->CreateSamplerState(&sam_desc, &g_pHeightSampler));

  sam_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
  CHK(d3d_device->CreateSamplerState(&sam_desc, &g_pCubeSampler));

  sam_desc.Filter = D3D11_FILTER_ANISOTROPIC;
  sam_desc.MaxAnisotropy = 8;
  CHK(d3d_device->CreateSamplerState(&sam_desc, &g_pGradientSampler));

  sam_desc.MaxLOD = FLT_MAX;
  sam_desc.MaxAnisotropy = 4;
  CHK(d3d_device->CreateSamplerState(&sam_desc, &g_pPerlinSampler));

  sam_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
  sam_desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
  sam_desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
  sam_desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
  CHK(d3d_device->CreateSamplerState(&sam_desc, &g_pFresnelSampler));

  // State blocks
  D3D11_RASTERIZER_DESC ras_desc = {};
  ras_desc.FillMode = D3D11_FILL_SOLID;
  ras_desc.CullMode = D3D11_CULL_NONE;
  ras_desc.FrontCounterClockwise = FALSE;
  ras_desc.DepthBias = 0;
  ras_desc.SlopeScaledDepthBias = 0.0f;
  ras_desc.DepthBiasClamp = 0.0f;
  ras_desc.DepthClipEnable = TRUE;
  ras_desc.ScissorEnable = FALSE;
  ras_desc.MultisampleEnable = TRUE;
  ras_desc.AntialiasedLineEnable = FALSE;

  CHK(d3d_device->CreateRasterizerState(&ras_desc, &g_pRSState_Solid));

  ras_desc.FillMode = D3D11_FILL_WIREFRAME;

  d3d_device->CreateRasterizerState(&ras_desc, &g_pRSState_Wireframe);
  assert(g_pRSState_Wireframe);

  D3D11_DEPTH_STENCIL_DESC depth_desc = {};
  depth_desc.DepthEnable = FALSE;
  depth_desc.StencilEnable = FALSE;
  CHK(d3d_device->CreateDepthStencilState(&depth_desc, &g_pDSState_Disable));

  D3D11_BLEND_DESC blend_desc = {};
  blend_desc.AlphaToCoverageEnable = FALSE;
  blend_desc.IndependentBlendEnable = FALSE;
  blend_desc.RenderTarget[0].BlendEnable = TRUE;
  blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
  blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
  blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
  blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
  blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
  blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
  blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
  CHK(d3d_device->CreateBlendState(&blend_desc, &g_pBState_Transparent));
}

void csocean_renderer_base::cleanupRenderResource()
{
  g_pMeshIB.Reset();
  g_pMeshVB.Reset();
  g_pMeshLayout.Reset();

  g_pOceanSurfVS.Reset();
  g_pOceanSurfPS.Reset();
  g_pWireframePS.Reset();

  g_pFresnelMap.Reset();
  g_pSRV_Fresnel.Reset();
  g_pSRV_Perlin.Reset();
  g_pSRV_ReflectCube.Reset();

  g_pHeightSampler.Reset();
  g_pGradientSampler.Reset();
  g_pFresnelSampler.Reset();
  g_pPerlinSampler.Reset();
  g_pCubeSampler.Reset();

  g_pPerCallCB.Reset();
  g_pPerFrameCB.Reset();
  g_pShadingCB.Reset();

  g_pRSState_Solid.Reset();
  g_pRSState_Wireframe.Reset();
  g_pDSState_Disable.Reset();
  g_pBState_Transparent.Reset();

  g_render_list.clear();
}

#define MESH_INDEX_2D(x, y)	(((y) + vert_rect.bottom) * (g_MeshDim + 1) + (x) + vert_rect.left)

// Generate boundary mesh for a patch. Return the number of generated indices
int csocean_renderer_base::generateBoundaryMesh(int left_degree, int right_degree, int bottom_degree, int top_degree,
  RECT vert_rect, DWORD* output)
{
  // Triangle list for bottom boundary
  int i, j;
  int counter = 0;
  int width = vert_rect.right - vert_rect.left;

  if (bottom_degree > 0)
  {
    int b_step = width / bottom_degree;

    for (i = 0; i < width; i += b_step)
    {
      output[counter++] = MESH_INDEX_2D(i, 0);
      output[counter++] = MESH_INDEX_2D(i + b_step / 2, 1);
      output[counter++] = MESH_INDEX_2D(i + b_step, 0);

      for (j = 0; j < b_step / 2; j++)
      {
        if (i == 0 && j == 0 && left_degree > 0)
          continue;

        output[counter++] = MESH_INDEX_2D(i, 0);
        output[counter++] = MESH_INDEX_2D(i + j, 1);
        output[counter++] = MESH_INDEX_2D(i + j + 1, 1);
      }

      for (j = b_step / 2; j < b_step; j++)
      {
        if (i == width - b_step && j == b_step - 1 && right_degree > 0)
          continue;

        output[counter++] = MESH_INDEX_2D(i + b_step, 0);
        output[counter++] = MESH_INDEX_2D(i + j, 1);
        output[counter++] = MESH_INDEX_2D(i + j + 1, 1);
      }
    }
  }

  // Right boundary
  int height = vert_rect.top - vert_rect.bottom;

  if (right_degree > 0)
  {
    int r_step = height / right_degree;

    for (i = 0; i < height; i += r_step)
    {
      output[counter++] = MESH_INDEX_2D(width, i);
      output[counter++] = MESH_INDEX_2D(width - 1, i + r_step / 2);
      output[counter++] = MESH_INDEX_2D(width, i + r_step);

      for (j = 0; j < r_step / 2; j++)
      {
        if (i == 0 && j == 0 && bottom_degree > 0)
          continue;

        output[counter++] = MESH_INDEX_2D(width, i);
        output[counter++] = MESH_INDEX_2D(width - 1, i + j);
        output[counter++] = MESH_INDEX_2D(width - 1, i + j + 1);
      }

      for (j = r_step / 2; j < r_step; j++)
      {
        if (i == height - r_step && j == r_step - 1 && top_degree > 0)
          continue;

        output[counter++] = MESH_INDEX_2D(width, i + r_step);
        output[counter++] = MESH_INDEX_2D(width - 1, i + j);
        output[counter++] = MESH_INDEX_2D(width - 1, i + j + 1);
      }
    }
  }

  // Top boundary
  if (top_degree > 0)
  {
    int t_step = width / top_degree;

    for (i = 0; i < width; i += t_step)
    {
      output[counter++] = MESH_INDEX_2D(i, height);
      output[counter++] = MESH_INDEX_2D(i + t_step / 2, height - 1);
      output[counter++] = MESH_INDEX_2D(i + t_step, height);

      for (j = 0; j < t_step / 2; j++)
      {
        if (i == 0 && j == 0 && left_degree > 0)
          continue;

        output[counter++] = MESH_INDEX_2D(i, height);
        output[counter++] = MESH_INDEX_2D(i + j, height - 1);
        output[counter++] = MESH_INDEX_2D(i + j + 1, height - 1);
      }

      for (j = t_step / 2; j < t_step; j++)
      {
        if (i == width - t_step && j == t_step - 1 && right_degree > 0)
          continue;

        output[counter++] = MESH_INDEX_2D(i + t_step, height);
        output[counter++] = MESH_INDEX_2D(i + j, height - 1);
        output[counter++] = MESH_INDEX_2D(i + j + 1, height - 1);
      }
    }
  }

  // Left boundary
  if (left_degree > 0)
  {
    int l_step = height / left_degree;

    for (i = 0; i < height; i += l_step)
    {
      output[counter++] = MESH_INDEX_2D(0, i);
      output[counter++] = MESH_INDEX_2D(1, i + l_step / 2);
      output[counter++] = MESH_INDEX_2D(0, i + l_step);

      for (j = 0; j < l_step / 2; j++)
      {
        if (i == 0 && j == 0 && bottom_degree > 0)
          continue;

        output[counter++] = MESH_INDEX_2D(0, i);
        output[counter++] = MESH_INDEX_2D(1, i + j);
        output[counter++] = MESH_INDEX_2D(1, i + j + 1);
      }

      for (j = l_step / 2; j < l_step; j++)
      {
        if (i == height - l_step && j == l_step - 1 && top_degree > 0)
          continue;

        output[counter++] = MESH_INDEX_2D(0, i + l_step);
        output[counter++] = MESH_INDEX_2D(1, i + j);
        output[counter++] = MESH_INDEX_2D(1, i + j + 1);
      }
    }
  }

  return counter;
}

// Generate boundary mesh for a patch. Return the number of generated indices
int csocean_renderer_base::generateInnerMesh(RECT vert_rect, DWORD* output)
{
  int i, j;
  int counter = 0;
  int width = vert_rect.right - vert_rect.left;
  int height = vert_rect.top - vert_rect.bottom;

  bool reverse = false;
  for (i = 0; i < height; i++)
  {
    if (reverse == false)
    {
      output[counter++] = MESH_INDEX_2D(0, i);
      output[counter++] = MESH_INDEX_2D(0, i + 1);
      for (j = 0; j < width; j++)
      {
        output[counter++] = MESH_INDEX_2D(j + 1, i);
        output[counter++] = MESH_INDEX_2D(j + 1, i + 1);
      }
    }
    else
    {
      output[counter++] = MESH_INDEX_2D(width, i);
      output[counter++] = MESH_INDEX_2D(width, i + 1);
      for (j = width - 1; j >= 0; j--)
      {
        output[counter++] = MESH_INDEX_2D(j, i);
        output[counter++] = MESH_INDEX_2D(j, i + 1);
      }
    }

    reverse = !reverse;
  }

  return counter;
}

void csocean_renderer_base::createSurfaceMesh()
{
  auto d3d_device(graphics::instance()->d3d_device());

  // --------------------------------- Vertex Buffer -------------------------------
  int num_verts = (g_MeshDim + 1) * (g_MeshDim + 1);
  D3D11_SUBRESOURCE_DATA init_data = { 0 };
  {
    std::unique_ptr<ocean_vertex[]> pV(new ocean_vertex[num_verts]);
    assert(pV);

    int i, j;
    for (i = 0; i <= g_MeshDim; i++)
    {
      for (j = 0; j <= g_MeshDim; j++)
      {
        pV[i * (g_MeshDim + 1) + j].index_x = (float) j;
        pV[i * (g_MeshDim + 1) + j].index_y = (float) i;
      }
    }

    D3D11_BUFFER_DESC vb_desc;
    vb_desc.ByteWidth = num_verts * sizeof(ocean_vertex);
    vb_desc.Usage = D3D11_USAGE_IMMUTABLE;
    vb_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vb_desc.CPUAccessFlags = 0;
    vb_desc.MiscFlags = 0;
    vb_desc.StructureByteStride = sizeof(ocean_vertex);

    init_data.pSysMem = pV.get();
    init_data.SysMemPitch = 0;
    init_data.SysMemSlicePitch = 0;

    d3d_device->CreateBuffer(&vb_desc, &init_data, &g_pMeshVB);

  }

  // --------------------------------- Index Buffer -------------------------------
  // The index numbers for all mesh LODs (up to 256x256)
  {
    const int index_size_lookup[] = { 0, 0, 4284, 18828, 69444, 254412, 956916, 3689820, 14464836 };

    memset(&g_mesh_patterns[0][0][0][0][0], 0, sizeof(g_mesh_patterns));

    g_Lods = 0;
    for (int i = g_MeshDim; i > 1; i >>= 1)
      g_Lods++;

    // Generate patch meshes. Each patch contains two parts: the inner mesh which is a regular
    // grids in a triangle strip. The boundary mesh is constructed w.r.t. the edge degrees to
    // meet water-tight requirement.
    std::unique_ptr<DWORD[]> index_array(new DWORD[index_size_lookup[g_Lods]]);

    int offset = 0;
    int level_size = g_MeshDim;

    // Enumerate patterns
    for (int level = 0; level <= g_Lods - 2; level++)
    {
      int left_degree = level_size;

      for (int left_type = 0; left_type < 3; left_type++)
      {
        int right_degree = level_size;

        for (int right_type = 0; right_type < 3; right_type++)
        {
          int bottom_degree = level_size;

          for (int bottom_type = 0; bottom_type < 3; bottom_type++)
          {
            int top_degree = level_size;

            for (int top_type = 0; top_type < 3; top_type++)
            {
              QuadRenderParam* pattern = &g_mesh_patterns[level][left_type][right_type][bottom_type][top_type];

              // Inner mesh (triangle strip)
              RECT inner_rect;
              inner_rect.left = (left_degree == level_size) ? 0 : 1;
              inner_rect.right = (right_degree == level_size) ? level_size : level_size - 1;
              inner_rect.bottom = (bottom_degree == level_size) ? 0 : 1;
              inner_rect.top = (top_degree == level_size) ? level_size : level_size - 1;

              int num_new_indices = generateInnerMesh(inner_rect, index_array.get() + offset);

              pattern->inner_start_index = offset;
              pattern->num_inner_verts = (level_size + 1) * (level_size + 1);
              pattern->num_inner_faces = num_new_indices - 2;
              offset += num_new_indices;

              // Boundary mesh (triangle list)
              int l_degree = (left_degree == level_size) ? 0 : left_degree;
              int r_degree = (right_degree == level_size) ? 0 : right_degree;
              int b_degree = (bottom_degree == level_size) ? 0 : bottom_degree;
              int t_degree = (top_degree == level_size) ? 0 : top_degree;

              RECT outer_rect = { 0, level_size, level_size, 0 };
              num_new_indices = generateBoundaryMesh(l_degree, r_degree, b_degree, t_degree, outer_rect, index_array.get() + offset);

              pattern->boundary_start_index = offset;
              pattern->num_boundary_verts = (level_size + 1) * (level_size + 1);
              pattern->num_boundary_faces = num_new_indices / 3;
              offset += num_new_indices;

              top_degree /= 2;
            }
            bottom_degree /= 2;
          }
          right_degree /= 2;
        }
        left_degree /= 2;
      }
      level_size /= 2;
    }

    assert(offset == index_size_lookup[g_Lods]);

    D3D11_BUFFER_DESC ib_desc;
    ib_desc.ByteWidth = index_size_lookup[g_Lods] * sizeof(DWORD);
    ib_desc.Usage = D3D11_USAGE_IMMUTABLE;
    ib_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ib_desc.CPUAccessFlags = 0;
    ib_desc.MiscFlags = 0;
    ib_desc.StructureByteStride = sizeof(DWORD);

    init_data.pSysMem = index_array.get();

    d3d_device->CreateBuffer(&ib_desc, &init_data, &g_pMeshIB);
  }
}

void csocean_renderer_base::createFresnelMap()
{
  auto& d3d_context(res_.d3d_context);
  auto& d3d_device(graphics::instance()->d3d_device());

  std::unique_ptr<DWORD[]> buffer(new DWORD[FRESNEL_TEX_SIZE]);


  XMVECTOR ref_index = XMVectorSet(1.33f,1.33f,1.33f,1.33f);
  XMVECTOR sky_blending = XMVectorSet(g_SkyBlending, g_SkyBlending, g_SkyBlending, g_SkyBlending);
  XMVECTOR one = XMVectorSet(1.0f,1.0f,1.0f,1.0f);
  const float fresnel_tex_size = (float)FRESNEL_TEX_SIZE;
  DWORD* buf_ptr(buffer.get());

  for (int i = 0; i < FRESNEL_TEX_SIZE ; i += 4)
  {
    float i_f = (float) i;
    XMVECTOR cos_a = XMVectorSet(i_f / fresnel_tex_size, (i_f + 1.0f) / fresnel_tex_size, (i_f + 2.0f) / fresnel_tex_size, (i_f + 3.0f) / fresnel_tex_size);
    // Using water's refraction index 1.33
    XMVECTOR fresnel = XMConvertVectorFloatToUInt(XMVectorMultiply(XMFresnelTerm(cos_a, ref_index), XMVectorSet(255.0f, 255.f, 255.f, 255.f)), 0);
   // = (DWORD) (powf(1 / (1 + cos_a), g_SkyBlending) * 255);
    XMVECTOR sky_blend =
      XMVectorAndInt(
      XMConvertVectorFloatToUInt(
      XMVectorMultiply(
      XMVectorMultiply(
        XMVectorPow(
        XMVectorMultiply(one, XMVectorReciprocalEst(XMVectorAdd(one, cos_a))), sky_blending)
        , XMVectorSet(255.0f, 255.f, 255.f, 255.f)), XMVectorSet(256.f, 256.f, 256.f, 256.f)), 0)
        , XMVectorSetInt(0xffffff00, 0xffffff00, 0xffffff00, 0xffffff00));

    XMStoreInt4((uint32_t*) buf_ptr, XMVectorOrInt(sky_blend, fresnel));
    buf_ptr += 4;
    //buffer[i] = (sky_blend << 8) | fresnel;
  }

  D3D11_TEXTURE1D_DESC tex_desc;
  tex_desc.Width = FRESNEL_TEX_SIZE;
  tex_desc.MipLevels = 1;
  tex_desc.ArraySize = 1;
  tex_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  tex_desc.Usage = D3D11_USAGE_IMMUTABLE;
  tex_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
  tex_desc.CPUAccessFlags = 0;
  tex_desc.MiscFlags = 0;

  D3D11_SUBRESOURCE_DATA init_data;
  init_data.pSysMem = buffer.get();
  init_data.SysMemPitch = 0;
  init_data.SysMemSlicePitch = 0;

  CHK(d3d_device->CreateTexture1D(&tex_desc, &init_data, &g_pFresnelMap));

  // Create shader resource
  D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
  srv_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
  srv_desc.Texture1D.MipLevels = 1;
  srv_desc.Texture1D.MostDetailedMip = 0;

  CHK(d3d_device->CreateShaderResourceView(g_pFresnelMap.Get(), &srv_desc, &g_pSRV_Fresnel));
}

void csocean_renderer_base::loadTextures()
{
  auto& d3d_context(res_.d3d_context);
  auto& d3d_device(graphics::instance()->d3d_device());

  CHK(CreateDDSTextureFromFile(d3d_device.Get(), (app_base_directory::instance()->resource_dir() + L"perlin_noise.dds").c_str(), nullptr, &g_pSRV_Perlin));
  CHK(CreateDDSTextureFromFile(d3d_device.Get(), (app_base_directory::instance()->resource_dir() + L"reflect_cube.dds").c_str(), nullptr, &g_pSRV_ReflectCube));
}

bool csocean_renderer_base::checkNodeVisibility(const QuadNode& quad_node)
{
  // Plane equation setup

  Matrix matProj = mat_projection_;
  
  // Left plane
  float fov_x = atan(1.0f / matProj(0,0));
  Vector4 plane_left(cos(fov_x), 0, sin(fov_x), 0);

  // Right plane
  Vector4 plane_right(-cos(fov_x), 0, sin(fov_x), 0);

  // Bottom plane
  float fov_y = atan(1.0f / matProj(1,1));
  Vector4 plane_bottom(0, cos(fov_y), sin(fov_y), 0);
  // Top plane
  Vector4 plane_top(0, -cos(fov_y), sin(fov_y), 0);

  // Test quad corners against view frustum in view space
  // SimpleMath::Vector4 corner_verts[4];
  Vector4 corner_verts[4];
  corner_verts[0] = Vector4(quad_node.bottom_left.x, quad_node.bottom_left.y, 0, 1);
  corner_verts[1] = corner_verts[0] + Vector4(quad_node.length, 0, 0, 0);
  corner_verts[2] = corner_verts[0] + Vector4(quad_node.length, quad_node.length, 0, 0);
  corner_verts[3] = corner_verts[0] + Vector4(0, quad_node.length, 0, 0);

  Matrix matView = Matrix(1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1) * mat_view_;
  ;
  corner_verts[0] = Vector4::Transform(corner_verts[0], matView);
  corner_verts[1] = Vector4::Transform(corner_verts[1], matView);
  corner_verts[2] = Vector4::Transform(corner_verts[2], matView);
  corner_verts[3] = Vector4::Transform(corner_verts[3], matView);

  // Test against eye plane
  if (corner_verts[0].z < 0 && corner_verts[1].z < 0 && corner_verts[2].z < 0 && corner_verts[3].z < 0)
    return false;

  // Test against left plane
  float dist_0 = corner_verts[0].Dot(plane_left);
  float dist_1 = corner_verts[1].Dot(plane_left);
  float dist_2 = corner_verts[2].Dot(plane_left);
  float dist_3 = corner_verts[3].Dot(plane_left);

  if (dist_0 < 0 && dist_1 < 0 && dist_2 < 0 && dist_3 < 0)
    return false;

  // Test against right plane
  dist_0 = corner_verts[0].Dot(plane_right);
  dist_1 = corner_verts[1].Dot(plane_right);
  dist_2 = corner_verts[2].Dot(plane_right);
  dist_3 = corner_verts[3].Dot(plane_right);

  if (dist_0 < 0 && dist_1 < 0 && dist_2 < 0 && dist_3 < 0)
    return false;

  // Test against bottom plane
  dist_0 = corner_verts[0].Dot(plane_bottom);
  dist_1 = corner_verts[1].Dot(plane_bottom);
  dist_2 = corner_verts[2].Dot(plane_bottom);
  dist_3 = corner_verts[3].Dot(plane_bottom);

  if (dist_0 < 0 && dist_1 < 0 && dist_2 < 0 && dist_3 < 0)
    return false;

  // Test against top plane
  dist_0 = corner_verts[0].Dot(plane_top);
  dist_1 = corner_verts[1].Dot(plane_top);
  dist_2 = corner_verts[2].Dot(plane_top);
  dist_3 = corner_verts[3].Dot(plane_top);

  if (dist_0 < 0 && dist_1 < 0 && dist_2 < 0 && dist_3 < 0)
    return false;

  return true;
}

float csocean_renderer_base::estimateGridCoverage(const QuadNode& quad_node,  float screen_area)
{
  // Estimate projected area

  // Test 16 points on the quad and find out the biggest one.
  const static float sample_pos[16][2] =
  {
    { 0, 0 },
    { 0, 1 },
    { 1, 0 },
    { 1, 1 },
    { 0.5f, 0.333f },
    { 0.25f, 0.667f },
    { 0.75f, 0.111f },
    { 0.125f, 0.444f },
    { 0.625f, 0.778f },
    { 0.375f, 0.222f },
    { 0.875f, 0.556f },
    { 0.0625f, 0.889f },
    { 0.5625f, 0.037f },
    { 0.3125f, 0.37f },
    { 0.8125f, 0.704f },
    { 0.1875f, 0.148f },
  };

  Matrix matProj = mat_projection_;
  Vector3 eye_point(eye_.x, eye_.z, eye_.y);
  float grid_len_world = quad_node.length / g_MeshDim;

  float max_area_proj = 0;
  for (int i = 0; i < 16; i++)
  {
    Vector3 test_point(quad_node.bottom_left.x + quad_node.length * sample_pos[i][0], quad_node.bottom_left.y + quad_node.length * sample_pos[i][1], 0);
    Vector3 eye_vec = test_point - eye_point;
    float dist = eye_vec.Length();

    float area_world = grid_len_world * grid_len_world;// * abs(eye_point.z) / sqrt(nearest_sqr_dist);
    float area_proj = area_world * matProj(0, 0) * matProj(1, 1) / (dist * dist);

    if (max_area_proj < area_proj)
      max_area_proj = area_proj;
  }

  float pixel_coverage = max_area_proj * screen_area * 0.25f;

  return pixel_coverage;
}

bool csocean_renderer_base::isLeaf(const QuadNode& quad_node)
{
  return (quad_node.sub_node[0] == -1 && quad_node.sub_node[1] == -1 && quad_node.sub_node[2] == -1 && quad_node.sub_node[3] == -1);
}

int csocean_renderer_base::searchLeaf(const std::vector<QuadNode>& node_list, const Vector2& point)
{
  int index = -1;

  int size = (int) node_list.size();
  QuadNode node = node_list[size - 1];

  while (!isLeaf(node))
  {
    bool found = false;

    for (int i = 0; i < 4; i++)
    {
      index = node.sub_node[i];
      if (index == -1)
        continue;

      QuadNode sub_node = node_list[index];
      if (point.x >= sub_node.bottom_left.x && point.x <= sub_node.bottom_left.x + sub_node.length &&
        point.y >= sub_node.bottom_left.y && point.y <= sub_node.bottom_left.y + sub_node.length)
      {
        node = sub_node;
        found = true;
        break;
      }
    }

    if (!found)
      return -1;
  }

  return index;
}

csocean_renderer_base::QuadRenderParam& csocean_renderer_base::selectMeshPattern(const QuadNode& quad_node)
{
  // Check 4 adjacent quad.
  Vector2 point_left = Vector2(quad_node.bottom_left.x, quad_node.bottom_left.y) + Vector2(-g_PatchLength * 0.5f, quad_node.length * 0.5f);
  int left_adj_index = searchLeaf(g_render_list, point_left);

  Vector2 point_right =  quad_node.bottom_left + Vector2(quad_node.length + g_PatchLength * 0.5f, quad_node.length * 0.5f);
  int right_adj_index = searchLeaf(g_render_list, point_right);

  Vector2 point_bottom = quad_node.bottom_left + Vector2(quad_node.length * 0.5f, -g_PatchLength * 0.5f);
  int bottom_adj_index = searchLeaf(g_render_list, point_bottom);

  Vector2 point_top = quad_node.bottom_left + Vector2(quad_node.length * 0.5f, quad_node.length + g_PatchLength * 0.5f);
  int top_adj_index = searchLeaf(g_render_list, point_top);

  int left_type = 0;
  if (left_adj_index != -1 && g_render_list[left_adj_index].length > quad_node.length * 0.999f)
  {
    QuadNode adj_node = g_render_list[left_adj_index];
    float scale = adj_node.length / quad_node.length * (g_MeshDim >> quad_node.lod) / (g_MeshDim >> adj_node.lod);
    if (scale > 3.999f)
      left_type = 2;
    else if (scale > 1.999f)
      left_type = 1;
  }

  int right_type = 0;
  if (right_adj_index != -1 && g_render_list[right_adj_index].length > quad_node.length * 0.999f)
  {
    QuadNode adj_node = g_render_list[right_adj_index];
    float scale = adj_node.length / quad_node.length * (g_MeshDim >> quad_node.lod) / (g_MeshDim >> adj_node.lod);
    if (scale > 3.999f)
      right_type = 2;
    else if (scale > 1.999f)
      right_type = 1;
  }

  int bottom_type = 0;
  if (bottom_adj_index != -1 && g_render_list[bottom_adj_index].length > quad_node.length * 0.999f)
  {
    QuadNode adj_node = g_render_list[bottom_adj_index];
    float scale = adj_node.length / quad_node.length * (g_MeshDim >> quad_node.lod) / (g_MeshDim >> adj_node.lod);
    if (scale > 3.999f)
      bottom_type = 2;
    else if (scale > 1.999f)
      bottom_type = 1;
  }

  int top_type = 0;
  if (top_adj_index != -1 && g_render_list[top_adj_index].length > quad_node.length * 0.999f)
  {
    QuadNode adj_node = g_render_list[top_adj_index];
    float scale = adj_node.length / quad_node.length * (g_MeshDim >> quad_node.lod) / (g_MeshDim >> adj_node.lod);
    if (scale > 3.999f)
      top_type = 2;
    else if (scale > 1.999f)
      top_type = 1;
  }

  // Check lookup table, [L][R][B][T]
  return g_mesh_patterns[quad_node.lod][left_type][right_type][bottom_type][top_type];
}

// Return value: if successful pushed into the list, return the position. If failed, return -1.
int csocean_renderer_base::buildNodeList(QuadNode& quad_node)
{
  auto d3d_context(res_.d3d_context);
  // Check against view frustum
  if (!checkNodeVisibility(quad_node))
    return -1;

  // Estimate the min grid coverage
  UINT num_vps = 1;
  D3D11_VIEWPORT vp;
  d3d_context->RSGetViewports(&num_vps, &vp);
  float min_coverage = estimateGridCoverage(quad_node, (float) vp.Width * vp.Height);

  // Recursively attatch sub-nodes.
  bool visible = true;
  if (min_coverage > g_UpperGridCoverage && quad_node.length > g_PatchLength)
  {
    // Recursive rendering for sub-quads.
    QuadNode sub_node_0 = { quad_node.bottom_left, quad_node.length / 2, 0, { -1, -1, -1, -1 } };
    quad_node.sub_node[0] = buildNodeList(sub_node_0) ;

    QuadNode sub_node_1 = { quad_node.bottom_left + Vector2(quad_node.length / 2, 0), quad_node.length / 2, 0, { -1, -1, -1, -1 } };
    quad_node.sub_node[1] = buildNodeList(sub_node_1);

    QuadNode sub_node_2 = { quad_node.bottom_left + Vector2(quad_node.length / 2, quad_node.length / 2), quad_node.length / 2, 0, { -1, -1, -1, -1 } };
    quad_node.sub_node[2] = buildNodeList(sub_node_2);

    QuadNode sub_node_3 = { quad_node.bottom_left + Vector2(0, quad_node.length / 2), quad_node.length / 2, 0, { -1, -1, -1, -1 } };
    quad_node.sub_node[3] = buildNodeList(sub_node_3);

    visible = !isLeaf(quad_node);
  }

  if (visible)
  {
    // Estimate mesh LOD
    int lod = 0;
    for (lod = 0; lod < g_Lods - 1; lod++)
    {
      if (min_coverage > g_UpperGridCoverage)
        break;
      min_coverage *= 4;
    }

    // We don't use 1x1 and 2x2 patch. So the highest level is g_Lods - 2.
    quad_node.lod = min(lod, g_Lods - 2);
  }
  else
    return -1;

  // Insert into the list
  int position = (int) g_render_list.size();
  g_render_list.push_back(quad_node);

  return position;
}

void csocean_renderer_base::renderShaded(ID3D11ShaderResourceViewPtr& displacemnet_map, ID3D11ShaderResourceViewPtr& gradient_map,float time)
{
  auto d3d_context(res_.d3d_context);
  // Build rendering list
  g_render_list.clear();
  float ocean_extent = g_PatchLength * (1 << g_FurthestCover);
  QuadNode root_node = { Vector2(-ocean_extent * 0.5f, -ocean_extent * 0.5f), ocean_extent, 0, { -1, -1, -1, -1 } };
  buildNodeList(root_node);

  // Matrices
  Matrix matView = Matrix(1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1) * mat_view_;
  Matrix matProj = mat_projection_;

  // VS & PS
  d3d_context->VSSetShader(g_pOceanSurfVS.Get(), NULL, 0);
  d3d_context->PSSetShader(g_pOceanSurfPS.Get(), NULL, 0);

  // Textures
  ID3D11ShaderResourceView* vs_srvs[2] = { displacemnet_map.Get(), g_pSRV_Perlin.Get() };
  d3d_context->VSSetShaderResources(0, 2, &vs_srvs[0]);

  ID3D11ShaderResourceView* ps_srvs[4] = { g_pSRV_Perlin.Get(), gradient_map.Get(), g_pSRV_Fresnel.Get(), g_pSRV_ReflectCube.Get() };
  d3d_context->PSSetShaderResources(1, 4, &ps_srvs[0]);

  // Samplers
  ID3D11SamplerState* vs_samplers[2] = { g_pHeightSampler.Get(), g_pPerlinSampler.Get() };
  d3d_context->VSSetSamplers(0, 2, &vs_samplers[0]);

  ID3D11SamplerState* ps_samplers[4] = { g_pPerlinSampler.Get(), g_pGradientSampler.Get(), g_pFresnelSampler.Get(), g_pCubeSampler.Get() };
  d3d_context->PSSetSamplers(1, 4, &ps_samplers[0]);

  // IA setup
  d3d_context->IASetIndexBuffer(g_pMeshIB.Get(), DXGI_FORMAT_R32_UINT, 0);

  ID3D11Buffer* vbs[1] = { g_pMeshVB.Get() };
  UINT strides[1] = { sizeof(ocean_vertex) };
  UINT offsets[1] = { 0 };
  d3d_context->IASetVertexBuffers(0, 1, &vbs[0], &strides[0], &offsets[0]);

  d3d_context->IASetInputLayout(g_pMeshLayout.Get());

  // State blocks
  d3d_context->RSSetState(g_pRSState_Solid.Get());

  // Constants
  ID3D11Buffer* cbs[1] = { g_pShadingCB.Get() };
  d3d_context->VSSetConstantBuffers(2, 1, cbs);
  d3d_context->PSSetConstantBuffers(2, 1, cbs);

  // We assume the center of the ocean surface at (0, 0, 0).
  for (int i = 0; i < (int) g_render_list.size(); i++)
  {
    QuadNode& node = g_render_list[i];

    if (!isLeaf(node))
      continue;

    // Check adjacent patches and select mesh pattern
    QuadRenderParam& render_param = selectMeshPattern(node);

    // Find the right LOD to render
    int level_size = g_MeshDim;
    for (int lod = 0; lod < node.lod; lod++)
      level_size >>= 1;

    // Matrices and constants
    Const_Per_Call call_consts = {};
    // Expand of the local coordinate to world space patch size
    Matrix matScale = Matrix::CreateScale(node.length / level_size, node.length / level_size, 0);
    call_consts.g_matLocal = matScale.Transpose();

    // WVP matrix
    Matrix matWorld;
    matWorld.Translation(Vector3(node.bottom_left.x, node.bottom_left.y, 0.0f));
    Matrix matWVP = matWorld * matView * matProj;
    call_consts.g_matWorldViewProj = matWVP.Transpose();

    // Texcoord for perlin noise
    Vector2 uv_base = node.bottom_left / g_PatchLength * g_PerlinSize;
    call_consts.g_UVBase = uv_base;

    // Constant g_PerlinSpeed need to be adjusted mannually
    Vector2 perlin_move = -g_WindDir * time * g_PerlinSpeed;
    call_consts.g_PerlinMovement = perlin_move;

    // Eye point
    Matrix matInvWV = (matWorld * matView).Invert();
    Vector3 vLocalEye(0, 0, 0);
    vLocalEye = vLocalEye.Transform(vLocalEye, matInvWV);
    call_consts.g_LocalEye = vLocalEye;

    // Update constant buffer
    D3D11_MAPPED_SUBRESOURCE mapped_res;
    d3d_context->Map(g_pPerCallCB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_res);
    *(Const_Per_Call*) mapped_res.pData = call_consts;
    d3d_context->Unmap(g_pPerCallCB.Get(), 0);

    cbs[0] = g_pPerCallCB.Get();
    d3d_context->VSSetConstantBuffers(4, 1, cbs);
    d3d_context->PSSetConstantBuffers(4, 1, cbs);

    // Perform draw call
    if (render_param.num_inner_faces > 0)
    {
      // Inner mesh of the patch
      d3d_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
      d3d_context->DrawIndexed(render_param.num_inner_faces + 2, render_param.inner_start_index, 0);
    }

    if (render_param.num_boundary_faces > 0)
    {
      // Boundary mesh of the patch
      d3d_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
      d3d_context->DrawIndexed(render_param.num_boundary_faces * 3, render_param.boundary_start_index, 0);
    }
  }

  // Unbind
  vs_srvs[0] = NULL;
  vs_srvs[1] = NULL;
  d3d_context->VSSetShaderResources(0, 2, &vs_srvs[0]);

  ps_srvs[0] = NULL;
  ps_srvs[1] = NULL;
  ps_srvs[2] = NULL;
  ps_srvs[3] = NULL;
  d3d_context->PSSetShaderResources(1, 4, &ps_srvs[0]);
}

void csocean_renderer_base::renderWireframe(ID3D11ShaderResourceViewPtr& displacemnet_map,float time)
{
  auto d3d_context(res_.d3d_context);

  // Build rendering list
  g_render_list.clear();
  float ocean_extent = g_PatchLength * (1 << g_FurthestCover);
  QuadNode root_node = { Vector2(-ocean_extent * 0.5f, -ocean_extent * 0.5f), ocean_extent, 0, { -1, -1, -1, -1 } };
  buildNodeList(root_node);

  // Matrices
  Matrix matView = Matrix(1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1) * mat_view_;
  Matrix matProj = mat_projection_;

  // VS & PS
  d3d_context->VSSetShader(g_pOceanSurfVS.Get(), NULL, 0);
  d3d_context->PSSetShader(g_pWireframePS.Get(), NULL, 0);

  // Textures
  ID3D11ShaderResourceView* vs_srvs[2] = { displacemnet_map.Get(), g_pSRV_Perlin.Get() };
  d3d_context->VSSetShaderResources(0, 2, &vs_srvs[0]);

  // Samplers
  ID3D11SamplerState* vs_samplers[2] = { g_pHeightSampler.Get(), g_pPerlinSampler.Get() };
  d3d_context->VSSetSamplers(0, 2, &vs_samplers[0]);

  ID3D11SamplerState* ps_samplers[4] = { NULL, NULL, NULL, NULL };
  d3d_context->PSSetSamplers(1, 4, &ps_samplers[0]);

  // IA setup
  d3d_context->IASetIndexBuffer(g_pMeshIB.Get(), DXGI_FORMAT_R32_UINT, 0);

  ID3D11Buffer* vbs[1] = { g_pMeshVB.Get() };
  UINT strides[1] = { sizeof(ocean_vertex) };
  UINT offsets[1] = { 0 };
  d3d_context->IASetVertexBuffers(0, 1, &vbs[0], &strides[0], &offsets[0]);

  d3d_context->IASetInputLayout(g_pMeshLayout.Get());

  // State blocks
  d3d_context->RSSetState(g_pRSState_Wireframe.Get());

  // Constants
  ID3D11Buffer* cbs[1] = { g_pShadingCB.Get() };
  d3d_context->VSSetConstantBuffers(2, 1, cbs);
  d3d_context->PSSetConstantBuffers(2, 1, cbs);

  // We assume the center of the ocean surface is at (0, 0, 0).
  for (int i = 0; i < (int) g_render_list.size(); i++)
  {
    QuadNode& node = g_render_list[i];

    if (!isLeaf(node))
      continue;

    // Check adjacent patches and select mesh pattern
    QuadRenderParam& render_param = selectMeshPattern(node);

    // Find the right LOD to render
    int level_size = g_MeshDim;
    for (int lod = 0; lod < node.lod; lod++)
      level_size >>= 1;

    // Matrices and constants
    Const_Per_Call call_consts;

    // Expand of the local coordinate to world space patch size
    Matrix matScale = Matrix::CreateScale(node.length / level_size, node.length / level_size, 0);
    call_consts.g_matLocal = matScale.Transpose();

    // WVP matrix
    Matrix matWorld;
    matWorld.Translation(Vector3(node.bottom_left.x, node.bottom_left.y, 0));

    Matrix matWVP = matWorld * matView * matProj;
    call_consts.g_matWorldViewProj = matWVP.Transpose();

    // Texcoord for perlin noise
    Vector2 uv_base = node.bottom_left / g_PatchLength * g_PerlinSize;
    call_consts.g_UVBase = uv_base;

    // Constant g_PerlinSpeed need to be adjusted mannually
    Vector2 perlin_move = -g_WindDir * time * g_PerlinSpeed;
    call_consts.g_PerlinMovement = perlin_move;

    // Eye point
    Matrix matInvWV = (matWorld * matView).Invert();
 
    Vector3 vLocalEye(0, 0, 0);
    vLocalEye = Vector3::Transform(vLocalEye, matInvWV);
    call_consts.g_LocalEye = vLocalEye;

    // Update constant buffer
    D3D11_MAPPED_SUBRESOURCE mapped_res;
    d3d_context->Map(g_pPerCallCB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_res);
    *(Const_Per_Call*) mapped_res.pData = call_consts;
    d3d_context->Unmap(g_pPerCallCB.Get(), 0);

    cbs[0] = g_pPerCallCB.Get();
    d3d_context->VSSetConstantBuffers(4, 1, cbs);
    d3d_context->PSSetConstantBuffers(4, 1, cbs);

    // Perform draw call
    if (render_param.num_inner_faces > 0)
    {
      // Inner mesh of the patch
      d3d_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
      d3d_context->DrawIndexed(render_param.num_inner_faces + 2, render_param.inner_start_index, 0);
    }

    if (render_param.num_boundary_faces > 0)
    {
      // Boundary mesh of the patch
      d3d_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
      d3d_context->DrawIndexed(render_param.num_boundary_faces * 3, render_param.boundary_start_index, 0);
    }
  }

  // Unbind
  vs_srvs[0] = NULL;
  vs_srvs[1] = NULL;
  d3d_context->VSSetShaderResources(0, 2, &vs_srvs[0]);

  // Restore states
  d3d_context->RSSetState(g_pRSState_Solid.Get());
}

void csocean_renderer_base::CreateOceanSimAndRender()
{
  auto d3d_context(res_.d3d_context);
  auto d3d_device(graphics::instance()->d3d_device());

  // Create ocean simulating object
  // Ocean object
  OceanParameter ocean_param;

  // The size of displacement map. In this sample, it's fixed to 512.
  ocean_param.dmap_dim = 512;
  // The side length (world space) of square patch
  ocean_param.patch_length = 2000.0f;
  // Adjust this parameter to control the simulation speed
  ocean_param.time_scale = 0.8f;
  // A scale to control the amplitude. Not the world space height
  ocean_param.wave_amplitude = 0.35f;
  // 2D wind direction. No need to be normalized
  ocean_param.wind_dir = Vector2(0.8f, 0.6f);
  // The bigger the wind speed, the larger scale of wave crest.
  // But the wave scale can be no larger than patch_length
  ocean_param.wind_speed = 600.0f;
  // Damp out the components opposite to wind direction.
  // The smaller the value, the higher wind dependency
  ocean_param.wind_dependency = 0.07f;
  // Control the scale of horizontal movement. Higher value creates
  // pointy crests.
  ocean_param.choppy_scale = 1.3f;

  g_pOceanSimulator.reset(new OceanSimulator(ocean_param, d3d_device,d3d_context));

  // Init D3D11 resources for rendering
  initRenderResource(ocean_param);

  // Update the simulation for the first time.
  g_pOceanSimulator->updateDisplacementMap(0, d3d_context);

}


