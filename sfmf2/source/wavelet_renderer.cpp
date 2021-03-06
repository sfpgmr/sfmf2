#include "stdafx.h"
#include "application.h"
#include "graphics.h"
#include "wavelet_renderer.h"


#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

using namespace sf;
using namespace DirectX;


struct simple_vertex
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 norm;
	DirectX::XMFLOAT2 tex;
};

struct cb_never_changes
{
	DirectX::XMMATRIX mView;
	DirectX::XMFLOAT4 vLightDir;
};

struct cb_change_on_resize
{
	DirectX::XMMATRIX mProjection;
};

struct cb_changes_every_frame
{
	DirectX::XMMATRIX mWorld;
	DirectX::XMFLOAT4 vLightColor;

	//    DirectX::XMFLOAT4 vMeshColor;
};

inline double power(const double re, const double im)
{
	return sqrt(re*re + im*im);
}

inline double han_window(const int i, const int size)
{
	return 0.5 - 0.5 * cos(2.0 * M_PI * i / size);
}

inline D2D1_COLOR_F hsv2rgb(double h, double s, double v,double alpha = 1.0)
{
  if (h < 0)
  {
    h += 360;
  }
  else if (h >= 360)
  {
    h -= 360;
  }

  int H_i = (int) floor(h / 60);
  double fl = (h / 60) - H_i;
  if (!(H_i & 1)) fl = 1 - fl; // if i is even
  double m = v * (1 - s);
  double n = v * (1 - s * fl);

  double r = 0, g = 0, b = 0;
  switch (H_i)
  {
  case 0: r = v; g = n; b = m; break;
  case 1: r = n; g = v; b = m; break;
  case 2: r = m; g = v; b = n; break;
  case 3: r = m; g = n; b = v; break;
  case 4: r = n; g = m; b = v; break;
  case 5: r = v; g = m; b = n; break;
  }

  return D2D1::ColorF(r,g,b,alpha);
}

const double sigma = 1.0;
const int octave_div = 12; // 音程単位
const int cent_div = 100; // セント単位

const double freq0 = 22.0; // 下限周波数
const int octave = 10; // 解析範囲のオクターブ
const int cent_interval = 25; // 解析周波数間隔(セント単位)

const int cent_max = cent_div * octave_div * octave; // 上限(セント単位)

void gabor_transform(double* wav, int b, int size, int Fs, double* wt)
{

  for (int y = 0; y < cent_max / cent_interval; y++)
  {
    double freq = freq0 * pow(2.0, double(y) * cent_interval / (cent_div * octave_div)); // 周波数
    double a = 1.0 / freq;
    double dt = a * sigma * sqrt(-2.0 * log(0.01)); // 窓幅(秒)
    int    dx = int(dt * Fs);                 // 窓幅(サンプル数)

    // 窓幅の範囲を積分
    double real_wt = 0;
    double imag_wt = 0;
    for (int m = -dx; m <= dx; m++)
    {
      int n = b + m; // 信号のサンプル位置
      if (n >= 0 && n < size)
      {
        double t = double(m) / Fs / a;
        double gauss = 1.0 / sqrt(2.0*M_PI*sigma*sigma) * exp(-t*t / (2.0*sigma*sigma)); // ガウス関数
        double omega_t = 2.0*M_PI*t; // ωt
        real_wt += double(wav[n]) * gauss * cos(omega_t);
        imag_wt += double(wav[n]) * gauss * sin(omega_t);
      }
    }
    wt[y] = 1.0 / sqrt(a) * sqrt(real_wt*real_wt + imag_wt*imag_wt);
  }
}

wavelet_renderer_base::wavelet_renderer_base(video_renderer_resources& res, wavelet_renderer_base::init_params_t& p) : text_(p.title), res_(res), time_(0)
{
	///////////////////////////////////////////////////////////////////
	// Direct3Dリソースの生成
	///////////////////////////////////////////////////////////////////

	auto& d3d_context(res_.d3d_context);
	auto& d3d_device(graphics::instance()->d3d_device());

	{
		// バーテックスシェーダのコンパイル
		ID3DBlobPtr vsblob, vserrblob;
		DWORD compile_flag = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
		compile_flag |= D3DCOMPILE_DEBUG;
#endif

    std::wstring vspath(app_base_directory::instance()->shader_dir() + L"dxgi_test.fx");
		HRESULT hr = D3DCompileFromFile
			(
			vspath.c_str(), NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VS", "vs_5_0",
			compile_flag, 0, &vsblob, &vserrblob);
		if (FAILED(hr))
		{
			if (vserrblob != NULL)
				OutputDebugStringA((char*) vserrblob->GetBufferPointer());
			if (vserrblob) vserrblob.Reset();
			throw sf::win32_error_exception(hr);
		}

		// バーテックスシェーダの生成
		CHK(d3d_device->CreateVertexShader(vsblob->GetBufferPointer(), vsblob->GetBufferSize(), NULL, &v_shader_));

		// 入力頂点レイアウトの定義
		D3D11_INPUT_ELEMENT_DESC
			layout [] = {
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 } };
		;

		// 入力頂点レイアウトの生成
		CHK(d3d_device->CreateInputLayout(layout, ARRAYSIZE(layout), vsblob->GetBufferPointer(),
			vsblob->GetBufferSize(), &input_layout_));
		vsblob.Reset();
	}

	// 入力レイアウトの設定
	d3d_context->IASetInputLayout(input_layout_.Get());

	// ピクセル・シェーダーのコンパイル
	{
		ID3DBlobPtr psblob, pserror;
		DWORD compile_flag = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
		compile_flag |= D3DCOMPILE_DEBUG;
#endif
    std::wstring pspath(app_base_directory::instance()->shader_dir() + L"dxgi_test.fx");
		HRESULT hr = D3DCompileFromFile(pspath.c_str(), NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PS", "ps_5_0",
			compile_flag, 0, &psblob, &pserror);
		if (FAILED(hr))
		{
			if (pserror != NULL)
				OutputDebugStringA((char*) pserror->GetBufferPointer());
			safe_release(pserror);
			throw sf::win32_error_exception(hr);
		}

		// ピクセルシェーダの作成
		CHK(d3d_device->CreatePixelShader(psblob->GetBufferPointer(), psblob->GetBufferSize(), NULL, &p_shader_));

		psblob.Reset();
	}

	// バーテックスバッファの作成
	// Create vertex buffer
	simple_vertex vertices [] =
	{
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) },

		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 1.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) }
	};

	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(simple_vertex) * ARRAYSIZE(vertices);
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA init_data = {};
	init_data.pSysMem = vertices;
	CHK(d3d_device->CreateBuffer(&bd, &init_data, &v_buffer_));

	// 頂点バッファのセット
	uint32_t stride = sizeof(simple_vertex);
	uint32_t offset = 0;
	d3d_context->IASetVertexBuffers(0, 1, v_buffer_.GetAddressOf(), &stride, &offset);

	// インデックスバッファの生成
	WORD indices [] =
	{
		3, 1, 0,
		2, 1, 3,

		6, 4, 5,
		7, 4, 6,

		11, 9, 8,
		10, 9, 11,

		14, 12, 13,
		15, 12, 14,

		19, 17, 16,
		18, 17, 19,

		22, 20, 21,
		23, 20, 22
	};


	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * ARRAYSIZE(indices);
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	init_data.pSysMem = indices;
	CHK(d3d_device->CreateBuffer(&bd, &init_data, &i_buffer_));

	// インデックスバッファのセット
	d3d_context->IASetIndexBuffer(i_buffer_.Get(), DXGI_FORMAT_R16_UINT, 0);

	// プリミティブの形態を指定する
	d3d_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 定数バッファを生成する。
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(cb_never_changes);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	CHK(d3d_device->CreateBuffer(&bd, NULL, &cb_never_changes_));

	bd.ByteWidth = sizeof(cb_change_on_resize);
	CHK(d3d_device->CreateBuffer(&bd, NULL, &cb_change_on_resize_));

	bd.ByteWidth = sizeof(cb_changes_every_frame);
	CHK(d3d_device->CreateBuffer(&bd, NULL, &cb_changes_every_frame_));

	// テクスチャのロード
	ID3D11ResourcePtr ptr;
  std::wstring texpath(app_base_directory::instance()->shader_dir() + L"SF.dds");
	CHK(CreateDDSTextureFromFile(d3d_device.Get(), texpath.c_str(), &ptr, &shader_res_view_, NULL));

	// サンプルステートの生成
	D3D11_SAMPLER_DESC sdesc = {};
	sdesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sdesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sdesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sdesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sdesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sdesc.MinLOD = 0;
	sdesc.MaxLOD = D3D11_FLOAT32_MAX;
	CHK(d3d_device->CreateSamplerState(&sdesc, &sampler_state_));

	// ワールド座標変換行列のセットアップ
	mat_world_ = XMMatrixIdentity();

	//g_vMeshColor( 0.7f, 0.7f, 0.7f, 1.0f );
	// 
	init_view_matrix();

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

  length_ = p.sample_length;
 
  wt_.reset(new double[cent_max / cent_interval]);
  wt_1_.reset(new double[cent_max / cent_interval]);

}


void wavelet_renderer_base::init_view_matrix()
{
	auto& d3d_context(res_.d3d_context);

	// ビュー行列のセットアップ
	XMVECTOR eye = XMVectorSet(0.0f, 3.0f, -6.0f, 0.0f);
	XMVECTOR at = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	mat_view_ = XMMatrixLookAtLH(eye, at, up);
	cb_never_changes cnc;
	cnc.mView = XMMatrixTranspose(mat_view_);
	//cnc.vLightColor = XMFLOAT4( 1.0f, 0.5f, 0.5f, 1.0f );
	cnc.vLightDir = XMFLOAT4(0.577f, 0.577f, -0.977f, 1.0f);
	// 定数バッファに格納
	d3d_context->UpdateSubresource(cb_never_changes_.Get(), 0, NULL, &cnc, 0, 0);

	// 投影行列のセットアップ
	mat_projection_ = XMMatrixPerspectiveFovLH(XM_PIDIV4, (float) res_.width / (float) res_.height, 0.01f, 100.0f);
	cb_change_on_resize ccor;
	ccor.mProjection = XMMatrixTranspose(mat_projection_);
	// 定数バッファに格納
	d3d_context->UpdateSubresource(cb_change_on_resize_.Get(), 0, NULL, &ccor, 0, 0);
}

void wavelet_renderer_base::render(LONGLONG t, int samplepos, audio_samples_t& audio_samples)
{

  auto& d3d_context(res_.d3d_context);

  // 色の変更
  mesh_color_.x = 1.0f;
  mesh_color_.y = 1.0f;
  mesh_color_.z = 1.0f;

  // 定数更新

  cb_changes_every_frame cb;
  static float rad = 0.0f;
  rad += 0.1f;
  mat_world_ = XMMatrixRotationY(rad);
  cb.mWorld = XMMatrixTranspose(mat_world_);
  cb.vLightColor = mesh_color_;
  d3d_context->UpdateSubresource(cb_changes_every_frame_.Get(), 0, NULL, &cb, 0, 0);
  d3d_context->OMSetRenderTargets(1, res_.render_target_view.GetAddressOf(), res_.depth_view.Get());
  //  float color[4] = { 1.0f, (2.0f - sinf(rad)) * 0.5f , 1.0f, 1.0f };
  float color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
  d3d_context->ClearRenderTargetView(res_.render_target_view.Get(), color);
  d3d_context->ClearDepthStencilView(res_.depth_view.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
  /*
  // 四角形
  d3d_context->VSSetShader(v_shader_.Get(), NULL, 0);
  d3d_context->VSSetConstantBuffers(0, 1, cb_never_changes_.GetAddressOf());
  d3d_context->VSSetConstantBuffers(1, 1, cb_change_on_resize_.GetAddressOf());
  d3d_context->VSSetConstantBuffers(2, 1, cb_changes_every_frame_.GetAddressOf());
  d3d_context->PSSetShader(p_shader_.Get(), NULL, 0);
  d3d_context->PSSetConstantBuffers(2, 1, cb_changes_every_frame_.GetAddressOf());
  d3d_context->PSSetShaderResources(0, 1, shader_res_view_.GetAddressOf());
  d3d_context->PSSetSamplers(0, 1, sampler_state_.GetAddressOf());

  // 頂点バッファのセット
  uint32_t stride = sizeof(simple_vertex);
  uint32_t offset = 0;
  d3d_context->IASetVertexBuffers(0, 1, v_buffer_.GetAddressOf(), &stride, &offset);
  // インデックスバッファのセット
  d3d_context->IASetIndexBuffer(i_buffer_.Get(), DXGI_FORMAT_R16_UINT, 0);
  // プリミティブの形態を指定する
  d3d_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


  d3d_context->DrawIndexed(36, 0, 0);
  */

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

//  {
//
////    const float delta = res_.width / (float) (length_ / 6.0f);
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
//      white_brush_->SetColor(hsv2rgb(350.0 + l * 140. / -min_db, 0.99, 0.99, 0.99 + l / -min_db));
//      context->FillRectangle(D2D1::RectF(i, left_top, i + 1, 350.0f),/*::Point2F(i, 250.0f), D2D1::Point2F(i, left), */ white_brush_.Get());
//      white_brush_->SetColor(hsv2rgb(350.0 + r * 140. / -min_db, 0.99, 0.99, 0.99 + r / -min_db));
//      context->FillRectangle(D2D1::RectF(i, right_top, i + 1, 700.0f),/*::Point2F(i, 250.0f), D2D1::Point2F(i, left), */ white_brush_.Get());
//      //      context->DrawLine(D2D1::Point2F(i, 550.0f), D2D1::Point2F(i, right), white_brush_.Get());
//      note += delta;
//    }
//    //white_brush_->SetColor(D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.5f));
//    //for (float i = 0; i < res_.width; i+= delta2)
//    //{
//    //  context->DrawLine(D2D1::Point2F(i, 0.0f), D2D1::Point2F(i, res_.height), white_brush_.Get());
//    //}
//  }

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

void wavelet_renderer_base::render(LONGLONG time, INT16* wave_data, int length){}


wavelet_renderer_base::~wavelet_renderer_base()
{
}


