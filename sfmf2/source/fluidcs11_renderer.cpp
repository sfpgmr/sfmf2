#include "stdafx.h"
#include "application.h"
#include "graphics.h"
#include "fluidcs11_renderer.h"
#include "fft4g.h"

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

using namespace sf;
using namespace DirectX;
namespace {
  struct particle
  {
    DirectX::XMFLOAT2 vPosition;
    DirectX::XMFLOAT2 vVelocity;
  };

  struct ParticleDensity
  {
    FLOAT fDensity;
  };

  struct ParticleForces
  {
    DirectX::XMFLOAT2 vAcceleration;
  };

  struct UINT2
  {
    UINT x;
    UINT y;
  };



  //--------------------------------------------------------------------------------------
  // Global variables
  //--------------------------------------------------------------------------------------

  // Compute Shader Constants
  // Grid cell key size for sorting, 8-bits for x and y
  const UINT NUM_GRID_INDICES = 65536;

  // Numthreads size for the simulation
  const UINT SIMULATION_BLOCK_SIZE = 256;

  // Numthreads size for the sort
  const UINT BITONIC_BLOCK_SIZE = 512;
  const UINT TRANSPOSE_BLOCK_SIZE = 16;

  // For this sample, only use power-of-2 numbers >= 8K and <= 64K
  // The algorithm can be extended to support any number of particles
  // But to keep the sample simple, we do not implement boundary conditions to handle it
  const UINT NUM_PARTICLES_8K = 8 * 1024;
  const UINT NUM_PARTICLES_16K = 16 * 1024;
  const UINT NUM_PARTICLES_32K = 32 * 1024;
  const UINT NUM_PARTICLES_64K = 64 * 1024;
  UINT g_iNumParticles = NUM_PARTICLES_32K;

  // particle Properties
  // These will control how the fluid behaves
  FLOAT g_fInitialParticleSpacing = 0.0045f;
  FLOAT g_fSmoothlen = 0.012f;
  FLOAT g_fPressureStiffness = 200.0f;
  FLOAT g_fRestDensity = 1000.0f;
  FLOAT g_fParticleMass = 0.0002f;
  FLOAT g_fViscosity = 0.1f;
  FLOAT g_fMaxAllowableTimeStep = 0.005f;
  FLOAT g_fParticleRenderSize = 0.003f;

  // Gravity Directions
  const float pw = 0.4f;
  const XMFLOAT2A GRAVITY_DOWN(0, -pw);
  const XMFLOAT2A GRAVITY_UP(0, pw);
  const XMFLOAT2A GRAVITY_LEFT(-pw, 0);
  const XMFLOAT2A GRAVITY_RIGHT(pw, 0);
  XMFLOAT2A g_vGravity = GRAVITY_DOWN;

  // Map Size
  // These values should not be larger than 256 * fSmoothlen
  // Since the map must be divided up into fSmoothlen sized grid cells
  // And the grid cell is used as a 16-bit sort key, 8-bits for x and y
  FLOAT g_fMapHeight = 1.2f;
  FLOAT g_fMapWidth = (4.0f / 3.0f) * g_fMapHeight;

  // Map Wall Collision Planes
  FLOAT g_fWallStiffness = 3000.0f;
  XMFLOAT3A g_vPlanes[4] = {
    XMFLOAT3A(1, 0, 0),
    XMFLOAT3A(0, 1, 0),
    XMFLOAT3A(-1, 0, g_fMapWidth),
    XMFLOAT3A(0, -1, g_fMapHeight)
  };

  // Simulation Algorithm
  enum eSimulationMode
  {
    SIM_MODE_SIMPLE,
    SIM_MODE_SHARED,
    SIM_MODE_GRID
  };

  eSimulationMode g_eSimMode = SIM_MODE_GRID;


  // Constant Buffer Layout
#pragma warning(push)
#pragma warning(disable:4324) // structure was padded due to __declspec(align())
#define _DECLSPEC_ALIGN_16_ __declspec(align(16))

  _DECLSPEC_ALIGN_16_ struct CBSimulationConstants
  {
    UINT iNumParticles;
    FLOAT fTimeStep;
    FLOAT fSmoothlen;
    FLOAT fPressureStiffness;
    FLOAT fRestDensity;
    FLOAT fDensityCoef;
    FLOAT fGradPressureCoef;
    FLOAT fLapViscosityCoef;
    FLOAT fWallStiffness;

    XMFLOAT2A vGravity;
    XMFLOAT4A vGridDim;

    XMFLOAT3A vPlanes[4];
  };

  _DECLSPEC_ALIGN_16_ struct CBRenderConstants
  {
    XMFLOAT4X4 mViewProjection;
    FLOAT fParticleSize;
  };

  _DECLSPEC_ALIGN_16_ struct SortCB
  {
    UINT iLevel;
    UINT iLevelMask;
    UINT iWidth;
    UINT iHeight;
  };
#pragma warning(pop)
}


fluidcs11_renderer_base::fluidcs11_renderer_base(video_renderer_resources& res, fluidcs11_renderer_base::init_params_t& p) : text_(p.title), res_(res), time_(0)
{
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

  std::wstring render_path(app_base_directory::instance()->shader_dir() + L"FluidRender.fx");
  // 頂点シェーダー
  create_shader_from_file(d3d_device, render_path, "ParticleVS", "vs_5_0", g_pParticleVS);
  // ジオメトリシェーダー
  create_shader_from_file(d3d_device, render_path, "ParticleGS", "gs_5_0", g_pParticleGS);
  // ピクセルシェーダー
  create_shader_from_file(d3d_device, render_path, "ParticlePS", "ps_5_0", g_pParticlePS);
  // Compute Shaders
  std::wstring cspath(app_base_directory::instance()->shader_dir() + L"FluidCS11.hlsl");
  const std::string cs_ver("cs_5_0");
  create_shader_from_file(d3d_device, cspath, "IntegrateCS", cs_ver, g_pIntegrateCS);
  create_shader_from_file(d3d_device, cspath, "DensityCS_Simple", cs_ver, g_pDensity_SimpleCS);
  create_shader_from_file(d3d_device, cspath, "ForceCS_Simple", cs_ver, g_pForce_SimpleCS);
  create_shader_from_file(d3d_device, cspath, "DensityCS_Shared", cs_ver, g_pDensity_SharedCS);
  create_shader_from_file(d3d_device, cspath, "ForceCS_Shared", cs_ver, g_pForce_SharedCS);
  create_shader_from_file(d3d_device, cspath, "DensityCS_Grid", cs_ver, g_pDensity_GridCS);
  create_shader_from_file(d3d_device, cspath, "ForceCS_Grid", cs_ver, g_pForce_GridCS);
  create_shader_from_file(d3d_device, cspath, "BuildGridCS", cs_ver, g_pBuildGridCS);
  create_shader_from_file(d3d_device, cspath, "ClearGridIndicesCS", cs_ver, g_pClearGridIndicesCS);
  create_shader_from_file(d3d_device, cspath, "BuildGridIndicesCS", cs_ver, g_pBuildGridIndicesCS);
  create_shader_from_file(d3d_device, cspath, "RearrangeParticlesCS", cs_ver, g_pRearrangeParticlesCS);

  std::wstring cssortpath(app_base_directory::instance()->shader_dir() + L"ComputeShaderSort11.hlsl");

  create_shader_from_file(d3d_device, cssortpath, "BitonicSort", cs_ver, g_pSortBitonic);
  create_shader_from_file(d3d_device, cssortpath, "MatrixTranspose", cs_ver, g_pSortTranspose);

  // Create the Simulation Buffers
  CreateSimulationBuffers();

  // Create Constant Buffers
  CHK(CreateConstantBuffer< CBSimulationConstants >(d3d_device.Get(), &g_pcbSimulationConstants));
  CHK(CreateConstantBuffer< CBRenderConstants >(d3d_device.Get(), &g_pcbRenderConstants));
  CHK(CreateConstantBuffer< SortCB >(d3d_device.Get(), &g_pSortCB));

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


}

void fluidcs11_renderer_base::CreateSimulationBuffers()
{
  auto& d3d_device(graphics::instance()->d3d_device());

  // Destroy the old buffers in case the number of particles has changed
  g_pParticles.Reset();
  g_pParticlesSRV.Reset();
  g_pParticlesUAV.Reset();

  g_pSortedParticles.Reset();
  g_pSortedParticlesSRV.Reset();
  g_pSortedParticlesUAV.Reset();

  g_pParticleForces.Reset();
  g_pParticleForcesSRV.Reset();
  g_pParticleForcesUAV.Reset();

  g_pParticleDensity.Reset();
  g_pParticleDensitySRV.Reset();
  g_pParticleDensityUAV.Reset();

  g_pGridSRV.Reset();
  g_pGridUAV.Reset();
  g_pGrid.Reset();

  g_pGridPingPongSRV.Reset();
  g_pGridPingPongUAV.Reset();
  g_pGridPingPong.Reset();

  g_pGridIndicesSRV.Reset();
  g_pGridIndicesUAV.Reset();
  g_pGridIndices.Reset();

  // Create the initial particle positions
  // This is only used to populate the GPU buffers on creation
  const UINT iStartingWidth = (UINT) sqrt((FLOAT) g_iNumParticles);
  std::unique_ptr<particle []> particles;
  particles.reset(new particle[g_iNumParticles]);
  ZeroMemory(particles.get(), sizeof(particle) * g_iNumParticles);
  for (UINT i = 0; i < g_iNumParticles; i++)
  {
    // Arrange the particles in a nice square
    UINT x = i % iStartingWidth;
    UINT y = i / iStartingWidth;
    particles[i].vPosition = XMFLOAT2(g_fInitialParticleSpacing * (FLOAT) x, g_fInitialParticleSpacing * (FLOAT) y);
  }

  // Create Structured Buffers
  CHK(CreateStructuredBuffer< particle >(d3d_device, g_iNumParticles, g_pParticles, g_pParticlesSRV, g_pParticlesUAV, particles.get()));
  CHK(CreateStructuredBuffer< particle >(d3d_device, g_iNumParticles, g_pSortedParticles, g_pSortedParticlesSRV, g_pSortedParticlesUAV, particles.get()));
  CHK(CreateStructuredBuffer< ParticleForces >(d3d_device, g_iNumParticles, g_pParticleForces, g_pParticleForcesSRV, g_pParticleForcesUAV));
  CHK(CreateStructuredBuffer< ParticleDensity >(d3d_device, g_iNumParticles, g_pParticleDensity, g_pParticleDensitySRV, g_pParticleDensityUAV));
  CHK(CreateStructuredBuffer< UINT >(d3d_device, g_iNumParticles, g_pGrid, g_pGridSRV, g_pGridUAV));
  CHK(CreateStructuredBuffer< UINT >(d3d_device, g_iNumParticles, g_pGridPingPong, g_pGridPingPongSRV, g_pGridPingPongUAV));
  CHK(CreateStructuredBuffer< UINT2 >(d3d_device, NUM_GRID_INDICES, g_pGridIndices, g_pGridIndicesSRV, g_pGridIndicesUAV));

}

void fluidcs11_renderer_base::init_view_matrix()
{
	auto& d3d_context(res_.d3d_context);

	//// ビュー行列のセットアップ
	//XMVECTOR eye = XMVectorSet(0.0f, 3.0f, -6.0f, 0.0f);
	//XMVECTOR at = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	//XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	//mat_view_ = XMMatrixLookAtLH(eye, at, up);
	//cb_never_changes cnc;
	//cnc.mView = XMMatrixTranspose(mat_view_);
	////cnc.vLightColor = XMFLOAT4( 1.0f, 0.5f, 0.5f, 1.0f );
	//cnc.vLightDir = XMFLOAT4(0.577f, 0.577f, -0.977f, 1.0f);
	//// 定数バッファに格納
	//d3d_context->UpdateSubresource(cb_never_changes_.Get(), 0, NULL, &cnc, 0, 0);

	//// 投影行列のセットアップ
	//mat_projection_ = XMMatrixPerspectiveFovLH(XM_PIDIV4, (float) res_.width / (float) res_.height, 0.01f, 100.0f);
	//cb_change_on_resize ccor;
	//ccor.mProjection = XMMatrixTranspose(mat_projection_);
	//// 定数バッファに格納
	//d3d_context->UpdateSubresource(cb_change_on_resize_.Get(), 0, NULL, &ccor, 0, 0);
}

void fluidcs11_renderer_base::render(LONGLONG t, int samplepos, audio_samples_t& audio_samples)
{

  auto& d3d_context(res_.d3d_context);

  // OMステージに登録する
  d3d_context->OMSetRenderTargets(1, res_.render_target_view.GetAddressOf(), res_.depth_view.Get());

  float color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
  d3d_context->ClearRenderTargetView(res_.render_target_view.Get(), color);
  d3d_context->ClearDepthStencilView(res_.depth_view.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

  static float time_backup = 0.0f;
  float elapsed_time = 0.0f;

  if (time_backup == 0.0f){
    time_backup = (double) t / (double) MFCLOCK_FREQUENCY_HNS;
    elapsed_time = time_backup;
  } else {
    float tc = (double) t / (double) MFCLOCK_FREQUENCY_HNS;
    elapsed_time = tc - time_backup;
    time_backup = tc;
  }

  while (elapsed_time > 0.0f)
  {
    SimulateFluid(elapsed_time);
    elapsed_time -= g_fMaxAllowableTimeStep;
  }
  RenderFluid(elapsed_time);


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

  {
    /*******/
    const double delta = (144.0 - 36.0) / res_.width;
    const float delta2 = res_.width / (144.0 - 36.0);
    const int li = length_ / 2;

    int pos = 0,pos_bkp = 0;
    double note = 36;
    double min_db = -40.0;
    const double  step = 44100. / (double(length_) / 2.);
    for (float i = 0; i < res_.width; ++i){
      pos =  (int)(pow(2, (note - 69.0) / 12.) * 440. / step);
      if (pos >= length_ / 2) break;

      double l = (((log_[pos] - 20.0) < min_db) ? min_db : (log_[pos] - 20.0));
      double r = (((log_1_[pos] - 20.0)  < min_db) ? min_db : (log_1_[pos] - 20.0));

      float left_top = (-l) * 5  + 150.0;
      float right_top = (-r) * 5 + 500.0;

//      white_brush_->SetColor(hsv2rgb(270.0 + l * 140. / -min_db, 0.99, 0.99, (0.99 + l / -min_db)*0.5));
      white_brush_->SetColor(D2D1::ColorF(1.0f, 1.0f, 1.0f, (0.99 + l / -min_db)*0.7));
      context->FillRectangle(D2D1::RectF(i, left_top, i + 1, 350.0f),/*::Point2F(i, 250.0f), D2D1::Point2F(i, left), */ white_brush_.Get());
      white_brush_->SetColor(D2D1::ColorF(1.0f, 1.0f, 1.0f, (0.99 + r / -min_db)*0.7));
//      white_brush_->SetColor(hsv2rgb(270.0 + r * 140. / -min_db, 0.99, 0.99, (0.99 + r / -min_db)*0.5));
      context->FillRectangle(D2D1::RectF(i, right_top, i + 1, 700.0f),/*::Point2F(i, 250.0f), D2D1::Point2F(i, left), */ white_brush_.Get());
      //      context->DrawLine(D2D1::Point2F(i, 550.0f), D2D1::Point2F(i, right), white_brush_.Get());
      note += delta;
    }
    //white_brush_->SetColor(D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.5f));
    //for (float i = 0; i < res_.width; i+= delta2)
    //{
    //  context->DrawLine(D2D1::Point2F(i, 0.0f), D2D1::Point2F(i, res_.height), white_brush_.Get());
    //}
  }

 
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

void fluidcs11_renderer_base::render(LONGLONG time, INT16* wave_data, int length)
{
  auto& d3d_context(res_.d3d_context);
}

void fluidcs11_renderer_base::GPUSort(ID3D11UnorderedAccessViewPtr& inUAV, ID3D11ShaderResourceViewPtr& inSRV,
  ID3D11UnorderedAccessViewPtr& tempUAV, ID3D11ShaderResourceViewPtr& tempSRV){
  auto& context(res_.d3d_context);

  context->CSSetConstantBuffers(0, 1, g_pSortCB.GetAddressOf());

  const UINT NUM_ELEMENTS = g_iNumParticles;
  const UINT MATRIX_WIDTH = BITONIC_BLOCK_SIZE;
  const UINT MATRIX_HEIGHT = NUM_ELEMENTS / BITONIC_BLOCK_SIZE;

  // Sort the data
  // First sort the rows for the levels <= to the block size
  for (UINT level = 2; level <= BITONIC_BLOCK_SIZE; level <<= 1)
  {
    SortCB constants = { level, level, MATRIX_HEIGHT, MATRIX_WIDTH };
    context->UpdateSubresource(g_pSortCB.Get(), 0, NULL, &constants, 0, 0);

    // Sort the row data
    UINT UAVInitialCounts = 0;
    context->CSSetUnorderedAccessViews(0, 1, inUAV.GetAddressOf(), &UAVInitialCounts);
    context->CSSetShader(g_pSortBitonic.Get(), NULL, 0);
    context->Dispatch(NUM_ELEMENTS / BITONIC_BLOCK_SIZE, 1, 1);
  }

  // Then sort the rows and columns for the levels > than the block size
  // Transpose. Sort the Columns. Transpose. Sort the Rows.
  for (UINT level = (BITONIC_BLOCK_SIZE << 1); level <= NUM_ELEMENTS; level <<= 1)
  {
    SortCB constants1 = { (level / BITONIC_BLOCK_SIZE), (level & ~NUM_ELEMENTS) / BITONIC_BLOCK_SIZE, MATRIX_WIDTH, MATRIX_HEIGHT };
    context->UpdateSubresource(g_pSortCB.Get(), 0, NULL, &constants1, 0, 0);

    // Transpose the data from buffer 1 into buffer 2
    ID3D11ShaderResourceView* pViewNULL = NULL;
    UINT UAVInitialCounts = 0;
    context->CSSetShaderResources(0, 1, &pViewNULL);
    context->CSSetUnorderedAccessViews(0, 1, tempUAV.GetAddressOf(), &UAVInitialCounts);
    context->CSSetShaderResources(0, 1, inSRV.GetAddressOf());
    context->CSSetShader(g_pSortTranspose.Get(), NULL, 0);
    context->Dispatch(MATRIX_WIDTH / TRANSPOSE_BLOCK_SIZE, MATRIX_HEIGHT / TRANSPOSE_BLOCK_SIZE, 1);

    // Sort the transposed column data
    context->CSSetShader(g_pSortBitonic.Get(), NULL, 0);
    context->Dispatch(NUM_ELEMENTS / BITONIC_BLOCK_SIZE, 1, 1);

    SortCB constants2 = { BITONIC_BLOCK_SIZE, level, MATRIX_HEIGHT, MATRIX_WIDTH };
    context->UpdateSubresource(g_pSortCB.Get(), 0, NULL, &constants2, 0, 0);

    // Transpose the data from buffer 2 back into buffer 1
    context->CSSetShaderResources(0, 1, &pViewNULL);
    context->CSSetUnorderedAccessViews(0, 1, inUAV.GetAddressOf(), &UAVInitialCounts);
    context->CSSetShaderResources(0, 1, tempSRV.GetAddressOf());
    context->CSSetShader(g_pSortTranspose.Get(), NULL, 0);
    context->Dispatch(MATRIX_HEIGHT / TRANSPOSE_BLOCK_SIZE, MATRIX_WIDTH / TRANSPOSE_BLOCK_SIZE, 1);

    // Sort the row data
    context->CSSetShader(g_pSortBitonic.Get(), NULL, 0);
    context->Dispatch(NUM_ELEMENTS / BITONIC_BLOCK_SIZE, 1, 1);
  }
}

void fluidcs11_renderer_base::SimulateFluid_Simple(){
  auto& context(res_.d3d_context);
  UINT UAVInitialCounts = 0;

  // Setup
  context->CSSetConstantBuffers(0, 1, g_pcbSimulationConstants.GetAddressOf());
  context->CSSetShaderResources(0, 1, g_pParticlesSRV.GetAddressOf());

  // Density
  context->CSSetUnorderedAccessViews(0, 1, g_pParticleDensityUAV.GetAddressOf(), &UAVInitialCounts);
  context->CSSetShader(g_pDensity_SimpleCS.Get(), NULL, 0);
  context->Dispatch(g_iNumParticles / SIMULATION_BLOCK_SIZE, 1, 1);

  // Force
  context->CSSetUnorderedAccessViews(0, 1, g_pParticleForcesUAV.GetAddressOf(), &UAVInitialCounts);
  context->CSSetShaderResources(1, 1, g_pParticleDensitySRV.GetAddressOf());
  context->CSSetShader(g_pForce_SimpleCS.Get(), NULL, 0);
  context->Dispatch(g_iNumParticles / SIMULATION_BLOCK_SIZE, 1, 1);

  // Integrate
  context->CopyResource(g_pSortedParticles.Get(), g_pParticles.Get());
  context->CSSetShaderResources(0, 1, g_pSortedParticlesSRV.GetAddressOf());
  context->CSSetUnorderedAccessViews(0, 1, g_pParticlesUAV.GetAddressOf(), &UAVInitialCounts);
  context->CSSetShaderResources(2, 1, g_pParticleForcesSRV.GetAddressOf());
  context->CSSetShader(g_pIntegrateCS.Get(), NULL, 0);
  context->Dispatch(g_iNumParticles / SIMULATION_BLOCK_SIZE, 1, 1);
}

void fluidcs11_renderer_base::SimulateFluid_Shared(){
  auto& context(res_.d3d_context);
  UINT UAVInitialCounts = 0;

  // Setup
  context->CSSetConstantBuffers(0, 1, g_pcbSimulationConstants.GetAddressOf());
  context->CSSetShaderResources(0, 1, g_pParticlesSRV.GetAddressOf());

  // Density
  context->CSSetUnorderedAccessViews(0, 1, g_pParticleDensityUAV.GetAddressOf(), &UAVInitialCounts);
  context->CSSetShader(g_pDensity_SharedCS.Get(), NULL, 0);
  context->Dispatch(g_iNumParticles / SIMULATION_BLOCK_SIZE, 1, 1);

  // Force
  context->CSSetUnorderedAccessViews(0, 1, g_pParticleForcesUAV.GetAddressOf(), &UAVInitialCounts);
  context->CSSetShaderResources(1, 1, g_pParticleDensitySRV.GetAddressOf());
  context->CSSetShader(g_pForce_SharedCS.Get(), NULL, 0);
  context->Dispatch(g_iNumParticles / SIMULATION_BLOCK_SIZE, 1, 1);

  // Integrate
  context->CopyResource(g_pSortedParticles.Get(), g_pParticles.Get());
  context->CSSetShaderResources(0, 1, g_pSortedParticlesSRV.GetAddressOf());
  context->CSSetUnorderedAccessViews(0, 1, g_pParticlesUAV.GetAddressOf(), &UAVInitialCounts);
  context->CSSetShaderResources(2, 1, g_pParticleForcesSRV.GetAddressOf());
  context->CSSetShader(g_pIntegrateCS.Get(), NULL, 0);
  context->Dispatch(g_iNumParticles / SIMULATION_BLOCK_SIZE, 1, 1);
}

void fluidcs11_renderer_base::SimulateFluid_Grid(){

  auto& context(res_.d3d_context);

  UINT UAVInitialCounts = 0;

  // Setup
  context->CSSetConstantBuffers(0, 1, g_pcbSimulationConstants.GetAddressOf());
  context->CSSetUnorderedAccessViews(0, 1, g_pGridUAV.GetAddressOf(), &UAVInitialCounts);
  context->CSSetShaderResources(0, 1, g_pParticlesSRV.GetAddressOf());

  // Build Grid
  context->CSSetShader(g_pBuildGridCS.Get(), NULL, 0);
  context->Dispatch(g_iNumParticles / SIMULATION_BLOCK_SIZE, 1, 1);

  // Sort Grid
  GPUSort(g_pGridUAV, g_pGridSRV, g_pGridPingPongUAV, g_pGridPingPongSRV);

  // Setup
  context->CSSetConstantBuffers(0, 1, g_pcbSimulationConstants.GetAddressOf());
  context->CSSetUnorderedAccessViews(0, 1, g_pGridIndicesUAV.GetAddressOf(), &UAVInitialCounts);
  context->CSSetShaderResources(3, 1, g_pGridSRV.GetAddressOf());

  // Build Grid Indices
  context->CSSetShader(g_pClearGridIndicesCS.Get(), NULL, 0);
  context->Dispatch(NUM_GRID_INDICES / SIMULATION_BLOCK_SIZE, 1, 1);
  context->CSSetShader(g_pBuildGridIndicesCS.Get(), NULL, 0);
  context->Dispatch(g_iNumParticles / SIMULATION_BLOCK_SIZE, 1, 1);

  // Setup
  context->CSSetUnorderedAccessViews(0, 1, g_pSortedParticlesUAV.GetAddressOf(), &UAVInitialCounts);
  context->CSSetShaderResources(0, 1, g_pParticlesSRV.GetAddressOf());
  context->CSSetShaderResources(3, 1, g_pGridSRV.GetAddressOf());

  // Rearrange
  context->CSSetShader(g_pRearrangeParticlesCS.Get(), NULL, 0);
  context->Dispatch(g_iNumParticles / SIMULATION_BLOCK_SIZE, 1, 1);

  // Setup
  context->CSSetUnorderedAccessViews(0, 1, &g_pNullUAV, &UAVInitialCounts);
  context->CSSetShaderResources(0, 1, &g_pNullSRV);
  context->CSSetShaderResources(0, 1, g_pSortedParticlesSRV.GetAddressOf());
  context->CSSetShaderResources(3, 1, g_pGridSRV.GetAddressOf());
  context->CSSetShaderResources(4, 1, g_pGridIndicesSRV.GetAddressOf());

  // Density
  context->CSSetUnorderedAccessViews(0, 1, g_pParticleDensityUAV.GetAddressOf(), &UAVInitialCounts);
  context->CSSetShader(g_pDensity_GridCS.Get(), NULL, 0);
  context->Dispatch(g_iNumParticles / SIMULATION_BLOCK_SIZE, 1, 1);

  // Force
  context->CSSetUnorderedAccessViews(0, 1, g_pParticleForcesUAV.GetAddressOf(), &UAVInitialCounts);
  context->CSSetShaderResources(1, 1, g_pParticleDensitySRV.GetAddressOf());
  context->CSSetShader(g_pForce_GridCS.Get(), NULL, 0);
  context->Dispatch(g_iNumParticles / SIMULATION_BLOCK_SIZE, 1, 1);

  // Integrate
  context->CSSetUnorderedAccessViews(0, 1, g_pParticlesUAV.GetAddressOf(), &UAVInitialCounts);
  context->CSSetShaderResources(2, 1, g_pParticleForcesSRV.GetAddressOf());
  context->CSSetShader(g_pIntegrateCS.Get(), NULL, 0);
  context->Dispatch(g_iNumParticles / SIMULATION_BLOCK_SIZE, 1, 1);
}

void fluidcs11_renderer_base::SimulateFluid(float fElapsedTime){
  auto& context(res_.d3d_context);
  UINT UAVInitialCounts = 0;

  // Update per-frame variables
  CBSimulationConstants pData = {};

  // Simulation Constants
  pData.iNumParticles = g_iNumParticles;
  // Clamp the time step when the simulation runs slowly to prevent numerical explosion
  pData.fTimeStep = min(g_fMaxAllowableTimeStep, fElapsedTime);
  pData.fSmoothlen = g_fSmoothlen;
  pData.fPressureStiffness = g_fPressureStiffness;
  pData.fRestDensity = g_fRestDensity;
  pData.fDensityCoef = g_fParticleMass * 315.0f / (64.0f * XM_PI * pow(g_fSmoothlen, 9));
  pData.fGradPressureCoef = g_fParticleMass * -45.0f / (XM_PI * pow(g_fSmoothlen, 6));
  pData.fLapViscosityCoef = g_fParticleMass * g_fViscosity * 45.0f / (XM_PI * pow(g_fSmoothlen, 6));

  pData.vGravity = g_vGravity;

  // Cells are spaced the size of the smoothing length search radius
  // That way we only need to search the 8 adjacent cells + current cell
  pData.vGridDim.x = 1.0f / g_fSmoothlen;
  pData.vGridDim.y = 1.0f / g_fSmoothlen;
  pData.vGridDim.z = 0;
  pData.vGridDim.w = 0;

  // Collision information for the map
  pData.fWallStiffness = g_fWallStiffness;
  pData.vPlanes[0] = g_vPlanes[0];
  pData.vPlanes[1] = g_vPlanes[1];
  pData.vPlanes[2] = g_vPlanes[2];
  pData.vPlanes[3] = g_vPlanes[3];

  context->UpdateSubresource(g_pcbSimulationConstants.Get(), 0, NULL, &pData, 0, 0);

  switch (g_eSimMode) {
    // Simple N^2 Algorithm
  case SIM_MODE_SIMPLE:
    SimulateFluid_Simple();
    break;

    // Optimized N^2 Algorithm using Shared Memory
  case SIM_MODE_SHARED:
    SimulateFluid_Shared();
    break;

    // Optimized Grid + Sort Algorithm
  case SIM_MODE_GRID:
    SimulateFluid_Grid();
    break;
  }

  // Unset
  context->CSSetUnorderedAccessViews(0, 1, &g_pNullUAV, &UAVInitialCounts);
  context->CSSetShaderResources(0, 1, &g_pNullSRV);
  context->CSSetShaderResources(1, 1, &g_pNullSRV);
  context->CSSetShaderResources(2, 1, &g_pNullSRV);
  context->CSSetShaderResources(3, 1, &g_pNullSRV);
  context->CSSetShaderResources(4, 1, &g_pNullSRV);
}

void fluidcs11_renderer_base::RenderFluid(float fElapsedTime)
{
  auto& context(res_.d3d_context);
  // Simple orthographic projection to display the entire map
  XMMATRIX mView = XMMatrixTranslation(-g_fMapWidth / 2.0f, -g_fMapHeight / 2.0f, 0);
  XMMATRIX mProjection = XMMatrixOrthographicLH(g_fMapWidth, g_fMapHeight, 0, 1);
  XMMATRIX mViewProjection = mView * mProjection;

  // Update Constants
  CBRenderConstants pData = {};

  XMStoreFloat4x4(&pData.mViewProjection, XMMatrixTranspose(mViewProjection));
  pData.fParticleSize = g_fParticleRenderSize;

  context->UpdateSubresource(g_pcbRenderConstants.Get(), 0, NULL, &pData, 0, 0);

  // Set the shaders
  context->VSSetShader(g_pParticleVS.Get(), NULL, 0);
  context->GSSetShader(g_pParticleGS.Get(), NULL, 0);
  context->PSSetShader(g_pParticlePS.Get(), NULL, 0);

  // Set the constant buffers
  context->VSSetConstantBuffers(0, 1, g_pcbRenderConstants.GetAddressOf());
  context->GSSetConstantBuffers(0, 1, g_pcbRenderConstants.GetAddressOf());
  context->PSSetConstantBuffers(0, 1, g_pcbRenderConstants.GetAddressOf());

  // Setup the particles buffer and IA
  context->VSSetShaderResources(0, 1, g_pParticlesSRV.GetAddressOf());
  context->VSSetShaderResources(1, 1, g_pParticleDensitySRV.GetAddressOf());
  context->IASetVertexBuffers(0, 1, &g_pNullBuffer, &g_iNullUINT, &g_iNullUINT);
  context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

  // Draw the mesh
  context->Draw(g_iNumParticles, 0);

  // Unset the particles buffer
  context->VSSetShaderResources(0, 1, &g_pNullSRV);
  context->VSSetShaderResources(1, 1, &g_pNullSRV);
}


fluidcs11_renderer_base::~fluidcs11_renderer_base()
{
  g_pcbSimulationConstants.Reset();
  g_pcbRenderConstants.Reset();
  g_pSortCB.Reset();

  g_pParticleVS.Reset();
  g_pParticleGS.Reset();
  g_pParticlePS.Reset();

  g_pIntegrateCS.Reset();
  g_pDensity_SimpleCS.Reset();
  g_pForce_SimpleCS.Reset();
  g_pDensity_SharedCS.Reset();
  g_pForce_SharedCS.Reset();
  g_pDensity_GridCS.Reset();
  g_pForce_GridCS.Reset();
  g_pBuildGridCS.Reset();
  g_pClearGridIndicesCS.Reset();
  g_pBuildGridIndicesCS.Reset();
  g_pRearrangeParticlesCS.Reset();
  g_pSortBitonic.Reset();
  g_pSortTranspose.Reset();

  g_pParticles.Reset();
  g_pParticlesSRV.Reset();
  g_pParticlesUAV.Reset();

  g_pSortedParticles.Reset();
  g_pSortedParticlesSRV.Reset();
  g_pSortedParticlesUAV.Reset();

  g_pParticleForces.Reset();
  g_pParticleForcesSRV.Reset();
  g_pParticleForcesUAV.Reset();

  g_pParticleDensity.Reset();
  g_pParticleDensitySRV.Reset();
  g_pParticleDensityUAV.Reset();

  g_pGridSRV.Reset();
  g_pGridUAV.Reset();
  g_pGrid.Reset();

  g_pGridPingPongSRV.Reset();
  g_pGridPingPongUAV.Reset();
  g_pGridPingPong.Reset();

  g_pGridIndicesSRV.Reset();
  g_pGridIndicesUAV.Reset();
  g_pGridIndices.Reset();
}


