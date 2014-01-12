#include "stdafx.h"
#include "application.h"
#include "graphics.h"
#include "test_renderer.h"


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

test_renderer_base::test_renderer_base(video_renderer_resources& res) : res_(res), time_(0)
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

    // バーテックスシェーダの生成
    CHK(d3d_device->CreateVertexShader(vsblob->GetBufferPointer(), vsblob->GetBufferSize(), NULL, &v_shader_));

    // 入力頂点レイアウトの定義
    D3D11_INPUT_ELEMENT_DESC
      layout[] = {
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

    // ピクセルシェーダの作成
    CHK(d3d_device->CreatePixelShader(psblob->GetBufferPointer(), psblob->GetBufferSize(), NULL, &p_shader_));

    psblob.Reset();
  }

  // バーテックスバッファの作成
  // Create vertex buffer
  simple_vertex vertices[] =
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
  WORD indices[] =
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
  std::wstring texpath(application::instance()->base_directory() + L"\\SF.dds");
  CHK(CreateDDSTextureFromFile(d3d_device.Get(),texpath.c_str(), &ptr, &shader_res_view_, NULL));

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

  //init_ = true;// 初期化完了
}

void test_renderer_base::init_view_matrix()
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
  mat_projection_ = XMMatrixPerspectiveFovLH(XM_PIDIV4, res_.width / res_.height, 0.01f, 100.0f);
  cb_change_on_resize ccor;
  ccor.mProjection = XMMatrixTranspose(mat_projection_);
  // 定数バッファに格納
  d3d_context->UpdateSubresource(cb_change_on_resize_.Get(), 0, NULL, &ccor, 0, 0);

}

void test_renderer_base::render(LONGLONG time)
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
  float color[4] = { 1.0f, 0.0f, 1.0f, 1.0f };
  d3d_context->ClearRenderTargetView(res_.render_target_view.Get(), color);

  // 四角形
  d3d_context->VSSetShader(v_shader_.Get(), NULL, 0);
  d3d_context->VSSetConstantBuffers(0, 1, cb_never_changes_.GetAddressOf());
  d3d_context->VSSetConstantBuffers(1, 1, cb_change_on_resize_.GetAddressOf());
  d3d_context->VSSetConstantBuffers(2, 1, cb_changes_every_frame_.GetAddressOf());
  d3d_context->PSSetShader(p_shader_.Get(), NULL, 0);
  d3d_context->PSSetConstantBuffers(2, 1, cb_changes_every_frame_.GetAddressOf());
  d3d_context->PSSetShaderResources(0, 1, shader_res_view_.GetAddressOf());
  d3d_context->PSSetSamplers(0, 1, sampler_state_.GetAddressOf());

  d3d_context->DrawIndexed(36, 0, 0);

}

test_renderer_base::~test_renderer_base() 
{
}


