#include "stdafx.h"
#include "application.h"
#include "graphics.h"
#include "fft_renderer.h"
#include "fft4g.h"


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
fft_renderer_base::fft_renderer_base(video_renderer_resources& res, fft_renderer_base::init_params_t& p) : text_(p.title), res_(res), time_(0)
{
	///////////////////////////////////////////////////////////////////
	// Direct3D���\�[�X�̐���
	///////////////////////////////////////////////////////////////////

	auto& d3d_context(res_.d3d_context);
	auto& d3d_device(graphics::instance()->d3d_device());

	{
		// �o�[�e�b�N�X�V�F�[�_�̃R���p�C��
		ID3DBlobPtr vsblob, vserrblob;
		DWORD compile_flag = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
		compile_flag |= D3DCOMPILE_DEBUG;
#endif

		std::wstring vspath(application::instance()->base_directory() + L"\\dxgi_test.fx");
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

		// �o�[�e�b�N�X�V�F�[�_�̐���
		CHK(d3d_device->CreateVertexShader(vsblob->GetBufferPointer(), vsblob->GetBufferSize(), NULL, &v_shader_));

		// ���͒��_���C�A�E�g�̒�`
		D3D11_INPUT_ELEMENT_DESC
			layout [] = {
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 } };
		;

		// ���͒��_���C�A�E�g�̐���
		CHK(d3d_device->CreateInputLayout(layout, ARRAYSIZE(layout), vsblob->GetBufferPointer(),
			vsblob->GetBufferSize(), &input_layout_));
		vsblob.Reset();
	}

	// ���̓��C�A�E�g�̐ݒ�
	d3d_context->IASetInputLayout(input_layout_.Get());

	// �s�N�Z���E�V�F�[�_�[�̃R���p�C��
	{
		ID3DBlobPtr psblob, pserror;
		DWORD compile_flag = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
		compile_flag |= D3DCOMPILE_DEBUG;
#endif
		std::wstring pspath(application::instance()->base_directory() + L"\\dxgi_test.fx");
		HRESULT hr = D3DCompileFromFile(pspath.c_str(), NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PS", "ps_5_0",
			compile_flag, 0, &psblob, &pserror);
		if (FAILED(hr))
		{
			if (pserror != NULL)
				OutputDebugStringA((char*) pserror->GetBufferPointer());
			safe_release(pserror);
			throw sf::win32_error_exception(hr);
		}

		// �s�N�Z���V�F�[�_�̍쐬
		CHK(d3d_device->CreatePixelShader(psblob->GetBufferPointer(), psblob->GetBufferSize(), NULL, &p_shader_));

		psblob.Reset();
	}

	// �o�[�e�b�N�X�o�b�t�@�̍쐬
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

	// ���_�o�b�t�@�̃Z�b�g
	uint32_t stride = sizeof(simple_vertex);
	uint32_t offset = 0;
	d3d_context->IASetVertexBuffers(0, 1, v_buffer_.GetAddressOf(), &stride, &offset);

	// �C���f�b�N�X�o�b�t�@�̐���
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

	// �C���f�b�N�X�o�b�t�@�̃Z�b�g
	d3d_context->IASetIndexBuffer(i_buffer_.Get(), DXGI_FORMAT_R16_UINT, 0);

	// �v���~�e�B�u�̌`�Ԃ��w�肷��
	d3d_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// �萔�o�b�t�@�𐶐�����B
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(cb_never_changes);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	CHK(d3d_device->CreateBuffer(&bd, NULL, &cb_never_changes_));

	bd.ByteWidth = sizeof(cb_change_on_resize);
	CHK(d3d_device->CreateBuffer(&bd, NULL, &cb_change_on_resize_));

	bd.ByteWidth = sizeof(cb_changes_every_frame);
	CHK(d3d_device->CreateBuffer(&bd, NULL, &cb_changes_every_frame_));

	// �e�N�X�`���̃��[�h
	ID3D11ResourcePtr ptr;
	std::wstring texpath(application::instance()->base_directory() + L"\\SF.dds");
	CHK(CreateDDSTextureFromFile(d3d_device.Get(), texpath.c_str(), &ptr, &shader_res_view_, NULL));

	// �T���v���X�e�[�g�̐���
	D3D11_SAMPLER_DESC sdesc = {};
	sdesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sdesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sdesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sdesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sdesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sdesc.MinLOD = 0;
	sdesc.MaxLOD = D3D11_FLOAT32_MAX;
	CHK(d3d_device->CreateSamplerState(&sdesc, &sampler_state_));

	// ���[���h���W�ϊ��s��̃Z�b�g�A�b�v
	mat_world_ = XMMatrixIdentity();

	//g_vMeshColor( 0.7f, 0.7f, 0.7f, 1.0f );
	// 
	init_view_matrix();

	// OM�X�e�[�W�ɓo�^����
	d3d_context->OMSetRenderTargets(1, res_.render_target_view.GetAddressOf(), res_.depth_view.Get());

	// �r���[�|�[�g�̐ݒ�
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
		D3D11_CULL_NONE,	//�|���S���̗��\�𖳂���
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

	// Direct 2D ���\�[�X�̍쐬

	res_.d2d_context->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &white_brush_);

	ZeroMemory(&text_metrics_, sizeof(DWRITE_TEXT_METRICS));

	// �f�o�C�X�Ɉˑ����郊�\�[�X���쐬���܂��B
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

	//  title(L".WAV�t�@�C������M4V�t�@�C���𐶐�����T���v��");


	//D2D1::Matrix3x2F mat2d = D2D1::Matrix3x2F::Rotation(90.0f);
	//mat2d.Invert();
	// mat2d._22 = - 1.0f;

	//res_.d2d_context->SetTransform(mat2d);

	//init_ = true;// ����������
	CHK(
		graphics::instance()->write_factory()->CreateTextLayout(
		text_.c_str(),
		(uint32) text_.length(),
		text_format_.Get(),
		res_.width, // ���̓e�L�X�g�̍ő啝�B
		40.0f, // ���̓e�L�X�g�̍ő卂���B
		&text_layout_
		)
		);
	CHK(text_layout_->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING));
	CHK(text_layout_->GetMetrics(&text_metrics_));

	// 1 Tick������̃T���v������2�ׂ̂��搔�Ɋۂ߂�
	length_ = 1;
	for (int i = 0; i < 32; ++i){
		length_ = length_ << 1;
		if (length_ > p.sample_length){
			length_ = length_ >> 2;
			break;
		}
	}

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


void fft_renderer_base::init_view_matrix()
{
	auto& d3d_context(res_.d3d_context);

	// �r���[�s��̃Z�b�g�A�b�v
	XMVECTOR eye = XMVectorSet(0.0f, 3.0f, -6.0f, 0.0f);
	XMVECTOR at = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	mat_view_ = XMMatrixLookAtLH(eye, at, up);
	cb_never_changes cnc;
	cnc.mView = XMMatrixTranspose(mat_view_);
	//cnc.vLightColor = XMFLOAT4( 1.0f, 0.5f, 0.5f, 1.0f );
	cnc.vLightDir = XMFLOAT4(0.577f, 0.577f, -0.977f, 1.0f);
	// �萔�o�b�t�@�Ɋi�[
	d3d_context->UpdateSubresource(cb_never_changes_.Get(), 0, NULL, &cnc, 0, 0);

	// ���e�s��̃Z�b�g�A�b�v
	mat_projection_ = XMMatrixPerspectiveFovLH(XM_PIDIV4, (float) res_.width / (float) res_.height, 0.01f, 100.0f);
	cb_change_on_resize ccor;
	ccor.mProjection = XMMatrixTranspose(mat_projection_);
	// �萔�o�b�t�@�Ɋi�[
	d3d_context->UpdateSubresource(cb_change_on_resize_.Get(), 0, NULL, &ccor, 0, 0);
}

void fft_renderer_base::render(LONGLONG t, int samplepos, audio_samples_t& audio_samples)
{

  auto& d3d_context(res_.d3d_context);

  // �F�̕ύX
  mesh_color_.x = 1.0f;
  mesh_color_.y = 1.0f;
  mesh_color_.z = 1.0f;

  // �萔�X�V

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
  // �l�p�`
  d3d_context->VSSetShader(v_shader_.Get(), NULL, 0);
  d3d_context->VSSetConstantBuffers(0, 1, cb_never_changes_.GetAddressOf());
  d3d_context->VSSetConstantBuffers(1, 1, cb_change_on_resize_.GetAddressOf());
  d3d_context->VSSetConstantBuffers(2, 1, cb_changes_every_frame_.GetAddressOf());
  d3d_context->PSSetShader(p_shader_.Get(), NULL, 0);
  d3d_context->PSSetConstantBuffers(2, 1, cb_changes_every_frame_.GetAddressOf());
  d3d_context->PSSetShaderResources(0, 1, shader_res_view_.GetAddressOf());
  d3d_context->PSSetSamplers(0, 1, sampler_state_.GetAddressOf());

  // ���_�o�b�t�@�̃Z�b�g
  uint32_t stride = sizeof(simple_vertex);
  uint32_t offset = 0;
  d3d_context->IASetVertexBuffers(0, 1, v_buffer_.GetAddressOf(), &stride, &offset);
  // �C���f�b�N�X�o�b�t�@�̃Z�b�g
  d3d_context->IASetIndexBuffer(i_buffer_.Get(), DXGI_FORMAT_R16_UINT, 0);
  // �v���~�e�B�u�̌`�Ԃ��w�肷��
  d3d_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


  d3d_context->DrawIndexed(36, 0, 0);
  */

  ID2D1DeviceContext* context = res_.d2d_context.Get();

  context->SetTarget(res_.video_bitmap.Get());
  context->SaveDrawingState(state_.Get());
  context->BeginDraw();

  //// �E�����ɔz�u
  //D2D1::Matrix3x2F screenTranslation = D2D1::Matrix3x2F::Translation(
  //  0.0f, logicalSize.Height);
  ////		logicalSize.Height - m_textMetrics.layoutHeight
  ////	);

  ////D2D1::Matrix3x2F screenTranslation = D2D1::Matrix3x2F::Translation(0.0f,0.0f);
  //screenTranslation._22 = screenTranslation._22 * -1.0f;

  //context->SetTransform(screenTranslation * m_deviceResources->GetOrientationTransform2D());



  // FFT�̎��s
  int size = audio_samples[0].size();
  int start = samplepos;
  int end = samplepos + length_;
  for (int i = start,j = 0; i < end; ++i,++j)
  {
    if (i < size)
    {
      // �n�j���O��
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
  rdft(length_, 1, a_.get(), ip_.get(), w_.get()); // ���U�t�[���G�ϊ�
  rdft(length_, 1, a_1_.get(), ip_1_.get(), w_1_.get()); // ���U�t�[���G�ϊ�

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

  // �g�`�f�[�^��\������

  //{
  //  const float delta = res_.width / (float) length;

  //  for (float i = 0; i < res_.width; i += delta){
  //    int pos = (int) i;
  //    if (pos >= length) break;
  //    float left = ((float) wave_data[pos]) / 32768.0f * 150.0f + 180.0f;
  //    float right = ((float) wave_data[pos + 1]) / 32768.0f * 150.0f + 540.0f;
  //    context->DrawLine(D2D1::Point2F(i, 180.0f), D2D1::Point2F(i, left), white_brush_.Get());
  //    context->DrawLine(D2D1::Point2F(i, 540.0f), D2D1::Point2F(i, right), white_brush_.Get());
  //  }
  //}

  // 

  {

//    const float delta = res_.width / (float) (length_ / 6.0f);
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

      white_brush_->SetColor(hsv2rgb(350.0 + l * 140. / -min_db, 0.99, 0.99, 0.99 + l / -min_db));
      context->FillRectangle(D2D1::RectF(i, left_top, i + 1, 350.0f),/*::Point2F(i, 250.0f), D2D1::Point2F(i, left), */ white_brush_.Get());
      white_brush_->SetColor(hsv2rgb(350.0 + r * 140. / -min_db, 0.99, 0.99, 0.99 + r / -min_db));
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

  //{
  //  const float delta = res_.width / (float) (length_ / 2.0f);
  //  const int li = length_ / 2;

  //  int pos = 0;
  //  for (float i = 0; i < res_.width; i += delta){
  //    if (pos >= li) break;
  //    double l = ((log_[pos] - 20.0) < -40.0 ? -40.0 : (log_[pos] - 20.0));
  //    double r = ((log_1_[pos] - 20.0) < -40.0 ? -40.0 : (log_1_[pos] - 20.0));

  //    float left_top = -l * 5. + 150.0;
  //    float right_top = -r * 5. + 500.0;

  //    white_brush_->SetColor(hsv2rgb(340.0 + l * 120 / 60, 0.99, 0.99, 0.99 + l / 60.));
  //    context->FillRectangle(D2D1::RectF(i, left_top, i + delta, 350.0f),/*::Point2F(i, 250.0f), D2D1::Point2F(i, left), */ white_brush_.Get());
  //    white_brush_->SetColor(hsv2rgb(340.0 + r * 120 / 60, 0.99, 0.99, 0.99 + r / 60.));
  //    context->FillRectangle(D2D1::RectF(i, right_top, i + delta, 700.0f),/*::Point2F(i, 250.0f), D2D1::Point2F(i, left), */ white_brush_.Get());
  //    //      context->DrawLine(D2D1::Point2F(i, 550.0f), D2D1::Point2F(i, right), white_brush_.Get());
  //    ++pos;
  //  }
  //}


  //{
  //  const float delta = res_.width / (float) (length_ / 3.5f);
  //  const int li = length_ / 3.5f;

  //  int pos = 0;
  //  for (float i = 0; i < res_.width; i += delta){
  //    if (pos >= li) break;
  //    double l = (log_[pos] < -60.0 ? -60.0 : log_[pos]);
  //    double r = (log_1_[pos] < -60.0 ? -60.0 : log_1_[pos]);

  //    float left_top = l * -200.0 / 60.0 + 150.0;
  //    float right_top = r * -200.0 / 60.0 + 500.0;

  //    white_brush_->SetColor(hsv2rgb(340.0 + l * 120 / 60, 0.99, 0.99, 0.99 + l / 60.));
  //    context->FillRectangle(D2D1::RectF(i, left_top, i + delta, 350.0f),/*::Point2F(i, 250.0f), D2D1::Point2F(i, left), */ white_brush_.Get());
  //    white_brush_->SetColor(hsv2rgb(340.0 + r * 120 / 60, 0.99, 0.99, 0.99 + r / 60.));
  //    context->FillRectangle(D2D1::RectF(i, right_top, i + delta, 700.0f),/*::Point2F(i, 250.0f), D2D1::Point2F(i, left), */ white_brush_.Get());
  //    //      context->DrawLine(D2D1::Point2F(i, 550.0f), D2D1::Point2F(i, right), white_brush_.Get());
  //    ++pos;
  //  }
  //}
//  white_brush_->SetOpacity(0.5f);
  white_brush_->SetColor(D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.5f));
  context->DrawTextLayout(
    D2D1::Point2F(0.f, 0.f),
    text_layout_.Get(),
    white_brush_.Get()
    );

  // D2DERR_RECREATE_TARGET �������Ŗ������܂��B���̃G���[�́A�f�o�C�X������ꂽ���Ƃ������܂��B
  // ����́APresent �ɑ΂��鎟��̌Ăяo�����ɏ�������܂��B


  HRESULT hr = context->EndDraw();
  if (hr != D2DERR_RECREATE_TARGET)
  {
    CHK(hr);
  }

  context->RestoreDrawingState(state_.Get());
}
void fft_renderer_base::render(LONGLONG time, INT16* wave_data, int length)
{

	auto& d3d_context(res_.d3d_context);

	// �F�̕ύX
	mesh_color_.x = 1.0f;
	mesh_color_.y = 1.0f;
	mesh_color_.z = 1.0f;

	// �萔�X�V

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
	// �l�p�`
	d3d_context->VSSetShader(v_shader_.Get(), NULL, 0);
	d3d_context->VSSetConstantBuffers(0, 1, cb_never_changes_.GetAddressOf());
	d3d_context->VSSetConstantBuffers(1, 1, cb_change_on_resize_.GetAddressOf());
	d3d_context->VSSetConstantBuffers(2, 1, cb_changes_every_frame_.GetAddressOf());
	d3d_context->PSSetShader(p_shader_.Get(), NULL, 0);
	d3d_context->PSSetConstantBuffers(2, 1, cb_changes_every_frame_.GetAddressOf());
	d3d_context->PSSetShaderResources(0, 1, shader_res_view_.GetAddressOf());
	d3d_context->PSSetSamplers(0, 1, sampler_state_.GetAddressOf());

	// ���_�o�b�t�@�̃Z�b�g
	uint32_t stride = sizeof(simple_vertex);
	uint32_t offset = 0;
	d3d_context->IASetVertexBuffers(0, 1, v_buffer_.GetAddressOf(), &stride, &offset);
	// �C���f�b�N�X�o�b�t�@�̃Z�b�g
	d3d_context->IASetIndexBuffer(i_buffer_.Get(), DXGI_FORMAT_R16_UINT, 0);
	// �v���~�e�B�u�̌`�Ԃ��w�肷��
	d3d_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	d3d_context->DrawIndexed(36, 0, 0);
  */

	ID2D1DeviceContext* context = res_.d2d_context.Get();

	context->SetTarget(res_.video_bitmap.Get());
	context->SaveDrawingState(state_.Get());
	context->BeginDraw();

	//// �E�����ɔz�u
	//D2D1::Matrix3x2F screenTranslation = D2D1::Matrix3x2F::Translation(
	//  0.0f, logicalSize.Height);
	////		logicalSize.Height - m_textMetrics.layoutHeight
	////	);

	////D2D1::Matrix3x2F screenTranslation = D2D1::Matrix3x2F::Translation(0.0f,0.0f);
	//screenTranslation._22 = screenTranslation._22 * -1.0f;

	//context->SetTransform(screenTranslation * m_deviceResources->GetOrientationTransform2D());

	context->DrawTextLayout(
		D2D1::Point2F(0.f, 0.f),
		text_layout_.Get(),
		white_brush_.Get()
		);

	// FFT�̎��s
	int l = length / 2;
	for (int i = 0; i < length_; ++i)
	{
		if (i < l)
		{
			// �n�j���O��
			a_[i] = han_window(i, length_) * (double(wave_data[i * 2]) / 32768.0);
      a_1_[i] = han_window(i, length_) * (double(wave_data[i * 2 + 1]) / 32768.0);
			//wavdata[i] = double(*p_wav);
			//DOUT(boost::wformat(L"%f\t%f\t%d\n") % a_[i] % (double(wave_data[i * 2]) / 32768.0) % wave_data[i * 2]);
		}
		else {
			a_[i] = 0;
		}
	}
  ip_[0] = ip_1_[0] = 0;
  w_[0] = w_1_[0] = 0;
	rdft(length_, 1, a_.get(), ip_.get(), w_.get()); // ���U�t�[���G�ϊ�
  rdft(length_, 1, a_1_.get(), ip_1_.get(), w_1_.get()); // ���U�t�[���G�ϊ�

	log_[0] = log10(a_[0]) / length_;
  log_1_[0] = log10(a_1_[0]) / length_;
  log_[length_ / 2] = log10(a_[1]) / length_;
  log_1_[length_ / 2] = log10(a_1_[1]) / length_;
  for (int i = 1, end = length_ / 2; i < end; ++i)
	{
/*		log_[i] = log(power(a_[i * 2], a_[i * 2 + 1])) / length_ * 5000;
    log_1_[i] = log(power(a_1_[i * 2], a_1_[i * 2 + 1])) / length_ * 5000;*/
  log_[i] = 20 * log10(power(a_[i * 2], a_[i * 2 + 1])) /*/ length_ * 25000*/;
  log_1_[i] = 20 * log10(power(a_1_[i * 2], a_1_[i * 2 + 1]))/* / length_ * 25000*/;
 //   DOUT(boost::wformat(L"%f\t%f\t%f\n") % a_[i * 2] % a_[i * 2 + 1] % (log_[i]));
		//TRACE(L"%f\t%f\t%e\n", wavdata[i*2], wavdata[i*2+1], Adft_log[i]);
	}

	// �g�`�f�[�^��\������

  //{
  //  const float delta = res_.width / (float) length;

  //  for (float i = 0; i < res_.width; i += delta){
  //    int pos = (int) i;
  //    if (pos >= length) break;
  //    float left = ((float) wave_data[pos]) / 32768.0f * 150.0f + 180.0f;
  //    float right = ((float) wave_data[pos + 1]) / 32768.0f * 150.0f + 540.0f;
  //    context->DrawLine(D2D1::Point2F(i, 180.0f), D2D1::Point2F(i, left), white_brush_.Get());
  //    context->DrawLine(D2D1::Point2F(i, 540.0f), D2D1::Point2F(i, right), white_brush_.Get());
  //  }
  //}

  // 

  {
    const float delta = res_.width / (float) (length_ / 3.5f );
    const int li = length_ / 3.5f;

    int pos = 0;
    for (float i = 0; i < res_.width; i += delta){
      if (pos >= li) break;
      double l = (log_[pos] < -60.0 ? -60.0 : log_[pos]);
      double r = (log_1_[pos] < -60.0 ? -60.0 : log_1_[pos]);

      float left_top = l * -200.0 / 60.0 + 150.0;
      float right_top = r * -200.0 / 60.0 + 500.0;

      white_brush_->SetColor(hsv2rgb(340.0 + l * 120 / 60, 0.99, 0.99,0.99 + l / 60.));
      context->FillRectangle(D2D1::RectF(i,left_top,i + delta,350.0f),/*::Point2F(i, 250.0f), D2D1::Point2F(i, left), */ white_brush_.Get());
      white_brush_->SetColor(hsv2rgb(340.0 + r * 120 / 60, 0.99, 0.99,0.99 + r / 60.));
      context->FillRectangle(D2D1::RectF(i, right_top, i + delta, 700.0f),/*::Point2F(i, 250.0f), D2D1::Point2F(i, left), */ white_brush_.Get());
      //      context->DrawLine(D2D1::Point2F(i, 550.0f), D2D1::Point2F(i, right), white_brush_.Get());
      ++pos;
    }
  }

	// D2DERR_RECREATE_TARGET �������Ŗ������܂��B���̃G���[�́A�f�o�C�X������ꂽ���Ƃ������܂��B
	// ����́APresent �ɑ΂��鎟��̌Ăяo�����ɏ�������܂��B
	HRESULT hr = context->EndDraw();
	if (hr != D2DERR_RECREATE_TARGET)
	{
		CHK(hr);
	}

	context->RestoreDrawingState(state_.Get());
}

fft_renderer_base::~fft_renderer_base()
{
}

