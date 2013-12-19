#include "stdafx.h"
#define BOOST_ASSIGN_MAX_PARAMS 7
#include <boost/assign.hpp>
#include <boost/assign/ptr_list_of.hpp>
#include <boost/assign/ptr_list_inserter.hpp>
#include "renderer.h"


using namespace sf;
using namespace DirectX;

window_renderer::window_renderer(base_window& window) : window_(window)
{
  ZeroMemory(&mode_desc_, sizeof(mode_desc_));
  ZeroMemory(&swap_chain_desc_, sizeof(swap_chain_desc_));
  ZeroMemory(&swap_chain_fullscreen_desc_, sizeof(swap_chain_fullscreen_desc_));
  create_device_independent_resources();
  create_device();
  create_d3d_resources();
  create_dcomp_resources();
  // ALT+ENTERを禁止（フルスクリーンのみ）
  CHK(dxgi_factory_->MakeWindowAssociation((HWND) window_.raw_handle(), DXGI_MWA_NO_ALT_ENTER));
}

window_renderer::~window_renderer()
{
}

/////////////////////////////////////////////////////////////////////////////
// Direct X関係のコード
/////////////////////////////////////////////////////////////////////////////
// 汎用情報格納用
struct mode_info
{
  mode_info(const std::wstring& n, const std::wstring& d) : name(n), description(d) {}
  std::wstring name;
  std::wstring description;
};

// ディスプレイモード
struct display_mode
{
  display_mode(const std::wstring& n, const std::wstring& d) : name(n), description(d) {}
  std::wstring name;
  std::wstring description;
};

std::vector<mode_info> display_modes =
boost::assign::list_of<mode_info>
(L"DXGI_FORMAT_UNKNOWN", L"フォーマットが不明")
(L"DXGI_FORMAT_R32G32B32A32_TYPELESS", L"4 成分、128 ビット型なしフォーマット 1")
(L"DXGI_FORMAT_R32G32B32A32_FLOAT", L"4 成分、128 ビット浮動小数点フォーマット 1")
(L"DXGI_FORMAT_R32G32B32A32_UINT", L"4 成分、128 ビット符号なし整数フォーマット 1")
(L"DXGI_FORMAT_R32G32B32A32_SINT", L"4 成分、128 ビット符号付き整数フォーマット 1")
(L"DXGI_FORMAT_R32G32B32_TYPELESS", L"3 成分、96 ビット型なしフォーマット")
(L"DXGI_FORMAT_R32G32B32_FLOAT", L"3 成分、96 ビット浮動小数点フォーマット")
(L"DXGI_FORMAT_R32G32B32_UINT", L"3 成分、96 ビット符号なし整数フォーマット")
(L"DXGI_FORMAT_R32G32B32_SINT", L"3 成分、96 ビット符号付き整数フォーマット")
(L"DXGI_FORMAT_R16G16B16A16_TYPELESS", L"4 成分、64 ビット型なしフォーマット")
(L"DXGI_FORMAT_R16G16B16A16_FLOAT", L"4 成分、64 ビット浮動小数点フォーマット")
(L"DXGI_FORMAT_R16G16B16A16_UNORM", L"4 成分、64 ビット符号なし整数フォーマット")
(L"DXGI_FORMAT_R16G16B16A16_UINT", L"4 成分、64 ビット符号なし整数フォーマット")
(L"DXGI_FORMAT_R16G16B16A16_SNORM", L"4 成分、64 ビット符号付き整数フォーマット")
(L"DXGI_FORMAT_R16G16B16A16_SINT", L"4 成分、64 ビット符号付き整数フォーマット")
(L"DXGI_FORMAT_R32G32_TYPELESS", L"2 成分、64 ビット型なしフォーマット")
(L"DXGI_FORMAT_R32G32_FLOAT", L"2 成分、64 ビット浮動小数点フォーマット")
(L"DXGI_FORMAT_R32G32_UINT", L"2 成分、64 ビット符号なし整数フォーマット")
(L"DXGI_FORMAT_R32G32_SINT", L"2 成分、64 ビット符号付き整数フォーマット")
(L"DXGI_FORMAT_R32G8X24_TYPELESS", L"2 成分、64 ビット型なしフォーマット")
(L"DXGI_FORMAT_D32_FLOAT_S8X24_UINT", L"32 ビット浮動小数点成分、および 2 つの符号なし整数成分です (追加の 32 ビットを含む)。")
(L"DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS", L"32 ビット浮動小数点成分、および 2 つの型なし成分です (追加の 32 ビットを含む)。")
(L"DXGI_FORMAT_X32_TYPELESS_G8X24_UINT", L"32 ビット型なし成分、および 2 つの符号なし整数成分です (追加の 32 ビットを含む)。")
(L"DXGI_FORMAT_R10G10B10A2_TYPELESS", L"4 成分、32 ビット型なしフォーマット")
(L"DXGI_FORMAT_R10G10B10A2_UNORM", L"4 成分、32 ビット符号なし整数フォーマット")
(L"DXGI_FORMAT_R10G10B10A2_UINT", L"4 成分、32 ビット符号なし整数フォーマット")
(L"DXGI_FORMAT_R11G11B10_FLOAT", L"3 成分、32 ビット浮動小数点フォーマット")
(L"DXGI_FORMAT_R8G8B8A8_TYPELESS", L"3 成分、32 ビット型なしフォーマット")
(L"DXGI_FORMAT_R8G8B8A8_UNORM", L"4 成分、32 ビット符号なし整数フォーマット")
(L"DXGI_FORMAT_R8G8B8A8_UNORM_SRGB", L"4 成分、32 ビット符号なし正規化整数 sRGB フォーマット")
(L"DXGI_FORMAT_R8G8B8A8_UINT", L"4 成分、32 ビット符号なし整数フォーマット")
(L"DXGI_FORMAT_R8G8B8A8_SNORM", L"3 成分、32 ビット符号付き整数フォーマット")
(L"DXGI_FORMAT_R8G8B8A8_SINT", L"3 成分、32 ビット符号付き整数フォーマット")
(L"DXGI_FORMAT_R16G16_TYPELESS", L"2 成分、32 ビット型なしフォーマット")
(L"DXGI_FORMAT_R16G16_FLOAT", L"2 成分、32 ビット浮動小数点フォーマット")
(L"DXGI_FORMAT_R16G16_UNORM", L"2 成分、32 ビット符号なし整数フォーマット")
(L"DXGI_FORMAT_R16G16_UINT", L"2 成分、32 ビット符号なし整数フォーマット")
(L"DXGI_FORMAT_R16G16_SNORM", L"2 成分、32 ビット符号付き整数フォーマット")
(L"DXGI_FORMAT_R16G16_SINT", L"2 成分、32 ビット符号付き整数フォーマット")
(L"DXGI_FORMAT_R32_TYPELESS", L"1 成分、32 ビット型なしフォーマット")
(L"DXGI_FORMAT_D32_FLOAT", L"1 成分、32 ビット浮動小数点フォーマット")
(L"DXGI_FORMAT_R32_FLOAT", L"1 成分、32 ビット浮動小数点フォーマット")
(L"DXGI_FORMAT_R32_UINT", L"1 成分、32 ビット符号なし整数フォーマット")
(L"DXGI_FORMAT_R32_SINT", L"1 成分、32 ビット符号付き整数フォーマット")
(L"DXGI_FORMAT_R24G8_TYPELESS", L"2 成分、32 ビット型なしフォーマット")
(L"DXGI_FORMAT_D24_UNORM_S8_UINT", L"深度チャンネルに 24 ビット、ステンシル チャンネルに 8 ビットを使用する 32 ビット Z バッファー フォーマット")
(L"DXGI_FORMAT_R24_UNORM_X8_TYPELESS", L"1 成分、24 ビット符号なし正規化整数と追加の型なし 8 ビットを含む、32 ビット フォーマット")
(L"DXGI_FORMAT_X24_TYPELESS_G8_UINT", L"1 成分、24 ビット型なしフォーマットと追加の 8 ビット符号なし整数成分を含む、32 ビット フォーマット")
(L"DXGI_FORMAT_R8G8_TYPELESS", L"2 成分、16 ビット型なしフォーマット")
(L"DXGI_FORMAT_R8G8_UNORM", L"2 成分、16 ビット符号なし整数フォーマット")
(L"DXGI_FORMAT_R8G8_UINT", L"2 成分、16 ビット符号なし整数フォーマット")
(L"DXGI_FORMAT_R8G8_SNORM", L"2 成分、16 ビット符号付き整数フォーマット")
(L"DXGI_FORMAT_R8G8_SINT", L"2 成分、16 ビット符号付き整数フォーマット")
(L"DXGI_FORMAT_R16_TYPELESS", L"1 成分、16 ビット型なしフォーマット")
(L"DXGI_FORMAT_R16_FLOAT", L"1 成分、16 ビット浮動小数点フォーマット")
(L"DXGI_FORMAT_D16_UNORM", L"1 成分、16 ビット符号なし正規化整数フォーマット")
(L"DXGI_FORMAT_R16_UNORM", L"1 成分、16 ビット符号なし整数フォーマット")
(L"DXGI_FORMAT_R16_UINT", L"1 成分、16 ビット符号なし整数フォーマット")
(L"DXGI_FORMAT_R16_SNORM", L"1 成分、16 ビット符号付き整数フォーマット")
(L"DXGI_FORMAT_R16_SINT", L"1 成分、16 ビット符号付き整数フォーマット")
(L"DXGI_FORMAT_R8_TYPELESS", L"1 成分、8 ビット型なしフォーマット")
(L"DXGI_FORMAT_R8_UNORM", L"1 成分、8 ビット符号なし整数フォーマット")
(L"DXGI_FORMAT_R8_UINT", L"1 成分、8 ビット符号なし整数フォーマット")
(L"DXGI_FORMAT_R8_SNORM", L"1 成分、8 ビット符号付き整数フォーマット")
(L"DXGI_FORMAT_R8_SINT", L"1 成分、8 ビット符号付き整数フォーマット")
(L"DXGI_FORMAT_A8_UNORM", L"1 成分、8 ビット符号なし整数フォーマット")
(L"DXGI_FORMAT_R1_UNORM", L"1 成分、1 ビット符号なし正規化整数フォーマット 2.")
(L"DXGI_FORMAT_R9G9B9E5_SHAREDEXP", L"4 成分、32 ビット浮動小数点フォーマット 2.")
(L"DXGI_FORMAT_R8G8_B8G8_UNORM", L"4 成分、32 ビット符号なし正規化整数フォーマット 3")
(L"DXGI_FORMAT_G8R8_G8B8_UNORM", L"4 成分、32 ビット符号なし正規化整数フォーマット 3")
(L"DXGI_FORMAT_BC1_TYPELESS", L"4 成分、型なしブロック圧縮フォーマット")
(L"DXGI_FORMAT_BC1_UNORM", L"4 成分、ブロック圧縮フォーマット")
(L"DXGI_FORMAT_BC1_UNORM_SRGB", L"sRGB data用の 4 成分、ブロック圧縮フォーマット")
(L"DXGI_FORMAT_BC2_TYPELESS", L"4 成分、型なしブロック圧縮フォーマット")
(L"DXGI_FORMAT_BC2_UNORM", L"4 成分、ブロック圧縮フォーマット")
(L"DXGI_FORMAT_BC2_UNORM_SRGB", L"sRGB data用の 4 成分、ブロック圧縮フォーマット")
(L"DXGI_FORMAT_BC3_TYPELESS", L"4 成分、型なしブロック圧縮フォーマット")
(L"DXGI_FORMAT_BC3_UNORM", L"4 成分、ブロック圧縮フォーマット")
(L"DXGI_FORMAT_BC3_UNORM_SRGB", L"sRGB data用の 4 成分、ブロック圧縮フォーマット")
(L"DXGI_FORMAT_BC4_TYPELESS", L"1 成分、型なしブロック圧縮フォーマット")
(L"DXGI_FORMAT_BC4_UNORM", L"1 成分、ブロック圧縮フォーマット")
(L"DXGI_FORMAT_BC4_SNORM", L"1 成分、ブロック圧縮フォーマット")
(L"DXGI_FORMAT_BC5_TYPELESS", L"2 成分、型なしブロック圧縮フォーマット")
(L"DXGI_FORMAT_BC5_UNORM", L"2 成分、ブロック圧縮フォーマット")
(L"DXGI_FORMAT_BC5_SNORM", L"2 成分、ブロック圧縮フォーマット")
(L"DXGI_FORMAT_B5G6R5_UNORM", L"3 成分、16 ビット符号なし正規化整数フォーマット")
(L"DXGI_FORMAT_B5G5R5A1_UNORM", L"1 ビット アルファをサポートする 4 成分、16 ビット符号なし正規化整数フォーマット")
(L"DXGI_FORMAT_B8G8R8A8_UNORM", L"8 ビット アルファをサポートする 4 成分、16 ビット符号なし正規化整数フォーマット")
(L"DXGI_FORMAT_B8G8R8X8_UNORM", L"4 成分、16 ビット符号なし正規化整数フォーマット")
(L"DXGI_FORMAT_FORCE_UINT", L"コンパイル時に、この列挙型のサイズを 32 ビットにするために定義されています。このvalueを指定しない場合、一部のコンパイラでは列挙型を 32 ビット以外のサイズでコンパイル可能この定数が使用されることはありません。");

// スキャンライン情報

std::vector<mode_info> scanline_orders =
boost::assign::list_of<mode_info>
(L"DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED", L"走査線の順序が指定されていません。")
(L"DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE", L"イメージは先頭の走査線～最後の走査線から作成され、スキップされる走査線はありません。")
(L"DXGI_MODE_SCANLINE_ORDER_UPPER_FIELD_FIRST", L"イメージが上部のフィールドから作成されます。")
(L"DXGI_MODE_SCANLINE_ORDER_LOWER_FIELD_FIRST", L"イメージが下部のフィールドから作成されます。");

// スケーリングパラメータ
std::vector<mode_info> scalings = boost::assign::list_of<mode_info>
(L"DXGI_MODE_SCALING_UNSPECIFIED", L"スケーリングが指定されていません。")
(L"DXGI_MODE_SCALING_CENTERED", L"スケーリングなしを指定します。イメージはディスプレイの中央に配置されます。通常、このフラグは固定ドットピッチ ディスプレイ (LED ディスプレイなど) に使用します。")
(L"DXGI_MODE_SCALING_STRETCHED", L"拡大スケーリングを指定します。");

struct simple_vertex
{
  XMFLOAT3 pos;
  XMFLOAT3 norm;
  XMFLOAT2 tex;
};

struct cb_never_changes
{
  XMMATRIX mView;
  XMFLOAT4 vLightDir;
};

struct cb_change_on_resize
{
  XMMATRIX mProjection;
};

struct cb_changes_every_frame
{
  XMMATRIX mWorld;
  XMFLOAT4 vLightColor;

  //    XMFLOAT4 vMeshColor;
};


void  window_renderer::create_device_independent_resources()
{
  // DXGI Factory の 生成

  //if(!dxgi_factory_)
  //{
  //  IDXGIFactory1Ptr factory;
  //  CHK(CreateDXGIFactory1(__uuidof(IDXGIFactory1),reinterpret_cast<void**>(factory.GetAddressOf())));
  //  factory.As(&dxgi_factory_);

  //  get_dxgi_information();
  //}

  if (!d2d_factory_){
    ID2D1Factory1Ptr factory;
#if defined(DEBUG) || defined(_DEBUG)
    D2D1_FACTORY_OPTIONS options = {};
    options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
    CHK(D2D1CreateFactory(
      D2D1_FACTORY_TYPE_SINGLE_THREADED,
      __uuidof(ID2D1Factory1),
      &options,
      &factory
      ));
#else
    CHK(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &factory));
#endif

    CHK(factory.As(&d2d_factory_));

  }

  if (!write_factory_){
    IDWriteFactory1Ptr factory;
    CHK(::DWriteCreateFactory(
      DWRITE_FACTORY_TYPE_SHARED,
      __uuidof(IDWriteFactory1),
      reinterpret_cast<IUnknown**>(factory.GetAddressOf())
      ));
    factory.As(&write_factory_);
  }


}


void  window_renderer::create_device(){

  //calc_client_size();
  //init_ = false;

  // Feature Level配列のセットアップ
  std::vector<D3D_FEATURE_LEVEL> feature_levels = {
    D3D_FEATURE_LEVEL_11_1,
    D3D_FEATURE_LEVEL_11_0
  };

  //  boost::assign::list_of<D3D_FEATURE_LEVEL>
  //  (D3D_FEATURE_LEVEL_11_1)        // DirectX11.1対応GPU
  //  (D3D_FEATURE_LEVEL_11_0);        // DirectX11.0対応GPU
  ////        (D3D_FEATURE_LEVEL_10_0 );       // DirectX10対応GPU

  // OSが設定したFeature Levelを受け取る変数
  D3D_FEATURE_LEVEL level;

  // Direct3D デバイス&コンテキスト作成
  {
    ID3D11DevicePtr device;
    ID3D11DeviceContextPtr context;
    CHK(::D3D11CreateDevice(
      nullptr,
      D3D_DRIVER_TYPE_HARDWARE,
      NULL,
      D3D11_CREATE_DEVICE_DEBUG | D3D11_CREATE_DEVICE_BGRA_SUPPORT,
      &feature_levels[0],
      feature_levels.size(),
      D3D11_SDK_VERSION,
      &device,
      &level,
      &context));
    CHK(device.As(&d3d_device_));
    CHK(context.As(&d3d_context_));
  }

  // DXGIデバイスの取得
  CHK(d3d_device_.As(&dxgi_device_));

  // DXGIアダプタの取得
  {
    IDXGIAdapterPtr adp;
    CHK(dxgi_device_->GetAdapter(&adp));
    CHK(adp.As(&dxgi_adapter_));
  }

  // DXGI Outputの取得
  {
    IDXGIOutputPtr out;
    CHK(dxgi_adapter_->EnumOutputs(0, &out));
    CHK(out.As(&dxgi_output_));
  }

  // DXGI ファクトリーの取得
  CHK(dxgi_adapter_->GetParent(IID_PPV_ARGS(&dxgi_factory_)));

  // MSAA
  //DXGI_SAMPLE_DESC msaa;
  //for(int i = 0; i <= D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT; i++){
  //  UINT q;
  //  if SUCCEEDED(d3d_device_->CheckMultisampleQualityLevels(DXGI_FORMAT_D24_UNORM_S8_UINT, i, &q)){
  //    if(1 < q){
  //      msaa.Count = i;
  //      msaa.Quality = q - 1;
  //      break;
  //    }
  //  }
  //}

  // スワップチェインの作成
  create_swap_chain(window_.is_fullscreen());

  CHK(dxgi_device_->SetMaximumFrameLatency(1));

  // Direct2Dデバイスの作成
  CHK(d2d_factory_->CreateDevice(dxgi_device_.Get(), &d2d_device_));

  // Direct2Dデバイスコンテキストの作成
  {
    ID2D1DeviceContextPtr context;
    CHK(d2d_device_->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &context));
    context.As(&d2d_context_);
  }

  // Direct Composition
  {
    IDCompositionDevicePtr dcomp_device;
    CHK(DCompositionCreateDevice2(dxgi_device_.Get(), __uuidof(IDCompositionDevice), &dcomp_device));
    dcomp_device.As(&dcomp_device2_);
    dcomp_device2_.As(&dcomp_desktop_device_);
    CHK(dcomp_desktop_device_->CreateTargetForHwnd((HWND) window_.raw_handle(), TRUE, &dcomp_target_));

  }

  create_swapchain_dependent_resources();

  //get_dxgi_information();

}



void window_renderer::create_d3d_resources()
{
  ///////////////////////////////////////////////////////////////////
  // Direct3Dリソースの生成
  ///////////////////////////////////////////////////////////////////

  {
    // バーテックスシェーダのコンパイル
    ID3DBlobPtr vsblob, vserrblob;
    DWORD compile_flag = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
    compile_flag |= D3DCOMPILE_DEBUG;
#endif

    HRESULT hr = D3DCompileFromFile
      (
      L"dxgi_test.fx", NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VS", "vs_5_0",
      compile_flag, 0, &vsblob, &vserrblob);
    if (FAILED(hr))
    {
      if (vserrblob != NULL)
        OutputDebugStringA((char*) vserrblob->GetBufferPointer());
      if (vserrblob) vserrblob.Reset();
      throw sf::win32_error_exception(hr);
    }

    // バーテックスシェーダの生成
    CHK(d3d_device_->CreateVertexShader(vsblob->GetBufferPointer(), vsblob->GetBufferSize(), NULL, &v_shader_));

    // 入力頂点レイアウトの定義
    D3D11_INPUT_ELEMENT_DESC
      layout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 } };
    ;

    // 入力頂点レイアウトの生成
    CHK(d3d_device_->CreateInputLayout(layout, ARRAYSIZE(layout), vsblob->GetBufferPointer(),
      vsblob->GetBufferSize(), &input_layout_));
    vsblob.Reset();
  }

  // 入力レイアウトの設定
  d3d_context_->IASetInputLayout(input_layout_.Get());

  // ピクセル・シェーダーのコンパイル
  {
    ID3DBlobPtr psblob, pserror;
    DWORD compile_flag = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
    compile_flag |= D3DCOMPILE_DEBUG;
#endif
    HRESULT hr = D3DCompileFromFile(L"dxgi_test.fx", NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PS", "ps_5_0",
      compile_flag, 0, &psblob, &pserror);
    if (FAILED(hr))
    {
      if (pserror != NULL)
        OutputDebugStringA((char*) pserror->GetBufferPointer());
      safe_release(pserror);
      throw sf::win32_error_exception(hr);
    }

    // ピクセルシェーダの作成
    CHK(d3d_device_->CreatePixelShader(psblob->GetBufferPointer(), psblob->GetBufferSize(), NULL, &p_shader_));

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
  CHK(d3d_device_->CreateBuffer(&bd, &init_data, &v_buffer_));

  // 頂点バッファのセット
  uint32_t stride = sizeof(simple_vertex);
  uint32_t offset = 0;
  d3d_context_->IASetVertexBuffers(0, 1, v_buffer_.GetAddressOf(), &stride, &offset);

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
  CHK(d3d_device_->CreateBuffer(&bd, &init_data, &i_buffer_));

  // インデックスバッファのセット
  d3d_context_->IASetIndexBuffer(i_buffer_.Get(), DXGI_FORMAT_R16_UINT, 0);

  // プリミティブの形態を指定する
  d3d_context_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

  // 定数バッファを生成する。
  bd.Usage = D3D11_USAGE_DEFAULT;
  bd.ByteWidth = sizeof(cb_never_changes);
  bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  bd.CPUAccessFlags = 0;
  CHK(d3d_device_->CreateBuffer(&bd, NULL, &cb_never_changes_));

  bd.ByteWidth = sizeof(cb_change_on_resize);
  CHK(d3d_device_->CreateBuffer(&bd, NULL, &cb_change_on_resize_));

  bd.ByteWidth = sizeof(cb_changes_every_frame);
  CHK(d3d_device_->CreateBuffer(&bd, NULL, &cb_changes_every_frame_));

  // テクスチャのロード
  ID3D11ResourcePtr ptr;
  CHK(CreateDDSTextureFromFile(d3d_device_.Get(), L"SF.dds", &ptr, &shader_res_view_, NULL));

  // サンプルステートの生成
  D3D11_SAMPLER_DESC sdesc = {};
  sdesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
  sdesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
  sdesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
  sdesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
  sdesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
  sdesc.MinLOD = 0;
  sdesc.MaxLOD = D3D11_FLOAT32_MAX;
  CHK(d3d_device_->CreateSamplerState(&sdesc, &sampler_state_));

  // ワールド座標変換行列のセットアップ
  mat_world_ = XMMatrixIdentity();

  //g_vMeshColor( 0.7f, 0.7f, 0.7f, 1.0f );
  // 
  init_view_matrix();

  //init_ = true;// 初期化完了

}

void window_renderer::create_dcomp_resources()
{
  IDCompositionSurfacePtr dcomp_surf;
  RECT surf_rect = { 0, 0, 200, 200 };
  CHK(dcomp_device2_->CreateSurface(surf_rect.right, surf_rect.bottom, swap_chain_desc_.Format, DXGI_ALPHA_MODE_PREMULTIPLIED, &dcomp_surf));
  IDXGISurfacePtr dxgi_surf;
  POINT offset;

  CHK(dcomp_surf->BeginDraw(&surf_rect, IID_PPV_ARGS(&dxgi_surf), &offset));

  D2D1_BITMAP_PROPERTIES1 bitmap_prop = D2D1::BitmapProperties1(
    D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
    D2D1::PixelFormat(swap_chain_desc_.Format, D2D1_ALPHA_MODE_PREMULTIPLIED),
    window_.dpi().dpix(),
    window_.dpi().dpiy());

  ID2D1ImagePtr backup;
  ID2D1Bitmap1Ptr d2dtarget_bitmap;
  CHK(d2d_context_->CreateBitmapFromDxgiSurface(dxgi_surf.Get(), &bitmap_prop, &d2dtarget_bitmap));

  d2d_context_->GetTarget(&backup);
  d2d_context_->SetTarget(d2dtarget_bitmap.Get());

  ID2D1SolidColorBrushPtr brush, tbrush;
  float alpha = 1.0f;
  CHK(d2d_context_->CreateSolidColorBrush(D2D1::ColorF(1.0f, 0.0f, 0.0f, alpha), &brush));
  CHK(d2d_context_->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f), &tbrush));
  IDWriteTextFormatPtr format;
  // Text Formatの作成
  CHK(write_factory_->CreateTextFormat(
    L"メイリオ",                // Font family name.
    NULL,                       // Font collection (NULL sets it to use the system font collection).
    DWRITE_FONT_WEIGHT_REGULAR,
    DWRITE_FONT_STYLE_NORMAL,
    DWRITE_FONT_STRETCH_NORMAL,
    12.000f,
    L"ja-jp",
    &format));

  d2d_context_->BeginDraw();

  float w = surf_rect.right / 2.0f, h = surf_rect.bottom / 2.0f;
  std::wstring t = L"DirectComposition1";

  d2d_context_->FillRectangle(D2D1::RectF(0.0f, 0.0f, w, h), brush.Get());
  d2d_context_->DrawTextW(t.c_str(), t.size(), format.Get(), D2D1::RectF(0.0f, 0.0f, w, h), tbrush.Get());

  brush->SetColor(D2D1::ColorF(0.0f, 1.0f, 0.0f, alpha));
  d2d_context_->FillRectangle(D2D1::RectF(w, 0.0f, w + w, h), brush.Get());
  t = L"DirectComposition2";
  d2d_context_->DrawTextW(t.c_str(), t.size(), format.Get(), D2D1::RectF(w, 0.0f, w + w, h), tbrush.Get());
  brush->SetColor(D2D1::ColorF(0.0f, 0.0f, 1.0f, alpha));
  t = L"DirectComposition4";
  d2d_context_->FillRectangle(D2D1::RectF(w, h, w + w, h + h), brush.Get());
  d2d_context_->DrawTextW(t.c_str(), t.size(), format.Get(), D2D1::RectF(w, h, w + w, h + h), tbrush.Get());
  brush->SetColor(D2D1::ColorF(1.0f, 0.0f, 1.0f, alpha));
  d2d_context_->FillRectangle(D2D1::RectF(0, h, w, h + h), brush.Get());
  t = L"DirectComposition3";
  d2d_context_->DrawTextW(t.c_str(), t.size(), format.Get(), D2D1::RectF(0, h, w, h + h), tbrush.Get());

  d2d_context_->EndDraw();
  dcomp_surf->EndDraw();
  brush.Reset();


  IDCompositionVisual2Ptr v, v1, v2, v3, v4;
  CHK(dcomp_device2_->CreateVisual(&v));
  CHK(v->SetContent(dcomp_surf.Get()));

  //      v->SetOffsetX(width_ / 2.0f);
  //      v->SetOffsetY(height_ / 5.0f);

  dcomp_target_->SetRoot(v.Get());

  CHK(dcomp_device2_->CreateVisual(&v1));
  CHK(v1->SetContent(dcomp_surf.Get()));
  CHK(dcomp_device2_->CreateVisual(&v2));
  CHK(v2->SetContent(dcomp_surf.Get()));
  CHK(dcomp_device2_->CreateVisual(&v3));
  CHK(v3->SetContent(dcomp_surf.Get()));
  CHK(dcomp_device2_->CreateVisual(&v4));
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
  dcomp_device2_->CreateRectangleClip(&clip);
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

  v->AddVisual(v1.Get(), FALSE, nullptr);
  v->AddVisual(v2.Get(), FALSE, nullptr);
  v->AddVisual(v3.Get(), FALSE, nullptr);
  v->AddVisual(v4.Get(), FALSE, nullptr);

  // 2D Transform のセットアップ
  {
    IDCompositionTransform* transforms[3];

    IDCompositionTransformPtr transform_group;

    CHK(dcomp_device2_->CreateRotateTransform(&rot_));
    CHK(dcomp_device2_->CreateRotateTransform(&rot_child_));
    CHK(dcomp_device2_->CreateScaleTransform(&scale_));
    CHK(dcomp_device2_->CreateTranslateTransform(&trans_));


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

    CHK(dcomp_device2_->CreateTransformGroup(transforms, 3, &transform_group));
    v->SetTransform(transform_group.Get());
    v1->SetTransform(rot_child_.Get());
    v2->SetTransform(rot_child_.Get());
    v3->SetTransform(rot_child_.Get());
    v4->SetTransform(rot_child_.Get());
  }

  {
    // Opacityのアニメーション
    IDCompositionAnimationPtr anim;
    CHK(dcomp_device2_->CreateAnimation(&anim));
    anim->AddCubic(0.0f, 0.0f, 1.0f / 4.0f, 0.0f, 0.0f);
    anim->AddCubic(4.0f, 1.0f, -1.0f / 4.0f, 0.0f, 0.0f);
    anim->AddRepeat(8.0f, 8.0f);
    //anim->End(10.0f,0.0f);

    IDCompositionEffectGroupPtr effect;
    dcomp_device2_->CreateEffectGroup(&effect);
    effect->SetOpacity(anim.Get());

    IDCompositionAnimationPtr anim3d;
    CHK(dcomp_device2_->CreateAnimation(&anim3d));
    anim3d->AddCubic(0.0f, 0.0f, 360.0f / 8.0f, 0.0f, 0.0f);
    anim3d->AddRepeat(8.0f, 8.0f);

    IDCompositionRotateTransform3DPtr rot3d;
    dcomp_device2_->CreateRotateTransform3D(&rot3d);
    rot3d->SetAngle(anim3d.Get());
    rot3d->SetAxisZ(0.0f);
    rot3d->SetAxisY(0.0f);
    rot3d->SetAxisX(1.0f);
    rot3d->SetCenterX(w);
    rot3d->SetCenterY(w);

    //   rot3d->SetAxisX(1.0f);

    effect->SetTransform3D(rot3d.Get());

    // 3D変換のアニメーション

    v->SetEffect(effect.Get());
  }

  dcomp_device2_->Commit();

  d2d_context_->SetTarget(backup.Get());

}

void window_renderer::create_swap_chain(bool fullscreen)
{
  // スワップチェーンの作成

  //RECT rect;
  //::GetWindowRect(hwnd, &rect);
  swap_chain_desc_.Width = window_.width();
  swap_chain_desc_.Height = window_.height();
  swap_chain_desc_.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
  swap_chain_desc_.Scaling = DXGI_SCALING_NONE;
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

  CHK(dxgi_factory_->CreateSwapChainForHwnd(d3d_device_.Get(),(HWND)window_.raw_handle(), &swap_chain_desc_, &swap_chain_fullscreen_desc_, dxgi_output_.Get(), &dxgi_swap_chain_));
}


void  window_renderer::create_d2d_render_target()
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

  CHK(d2d_context_->CreateBitmapFromDxgiSurface(
    dxgi_back_buffer_.Get(), &bitmap_properties, &d2d1_target_bitmap_));

  // Direct2D描画ターゲットの設定
  d2d_context_->SetTarget(d2d1_target_bitmap_.Get());

}

// Direc2D　描画ターゲットの設定

void  window_renderer::discard_d2d_render_target()
{
  d2d_context_->SetTarget(nullptr);
  d2d1_target_bitmap_.Reset();
}


//   
//  void  renderer::create_device(){
//    calc_client_size();
//    HRESULT hr = S_OK;
//    init_ = false;
//    RECT rc;
//    GetWindowRect(hwnd_,&rc);
//
//    // アダプタデバイス情報の取得
//    //LARGE_INTEGER version;
//    CHK(dxgi_factory_->EnumAdapters1(0,&dxgi_adapter_));
//    //CHK(dxgi_adapter_->CheckInterfaceSupport( __uuidof(ID3D10Device),&version));
//
//
//    // D3DDeviceの作成
//
//    std::vector<D3D_FEATURE_LEVEL> feature_levels = 
//      boost::assign::list_of<D3D_FEATURE_LEVEL>
//      (D3D_FEATURE_LEVEL_11_0 )        // DirectX11対応GPU
//      (D3D_FEATURE_LEVEL_10_1)        // DirectX10.1対応GPU
//      (D3D_FEATURE_LEVEL_10_0 );       // DirectX10対応GPU
//
//    D3D_FEATURE_LEVEL level;
//    CHK(::D3D11CreateDevice(
//      dxgi_adapter_.Get(),
//      D3D_DRIVER_TYPE_UNKNOWN ,
//      NULL,
//      D3D11_CREATE_DEVICE_DEBUG,
//      &feature_levels[0],
//      feature_levels.size(),
//      D3D11_SDK_VERSION,
//      &d3d_device_,
//      &level,
//      &d3d_context_));
//
//    CHK(dxgi_adapter_->EnumOutputs(0,&output_));
//
//    // MSAA
//    DXGI_SAMPLE_DESC msaa;
//    for(int i = 0; i <= D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT; i++){
//      UINT q;
//      if SUCCEEDED(d3d_device_->CheckMultisampleQualityLevels(DXGI_FORMAT_D24_UNORM_S8_UINT, i, &q)){
//        if(1 < q){
//          msaa.Count = i;
//          msaa.Quality = q - 1;
//          break;
//        }
//      }
//    }
//
//    // 表示モード
//    DXGI_MODE_DESC desired_desc = {};// , actual_desc_ = {};
//    // 各色8ビットで符号化なし正規化数
//    desired_desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
//    desired_desc.Height = height_;// 高さ
//    desired_desc.Width = width_;// 幅
//    desired_desc.Scaling = DXGI_MODE_SCALING_CENTERED;// スケーリングなし
//    // リフレッシュレートを60Hzを要求する
//
//    desired_desc.RefreshRate.Numerator = 60000;
//    desired_desc.RefreshRate.Denominator = 1000;
//    // 近いモードを検索
//    CHK(output_->FindClosestMatchingMode(&desired_desc,&actual_desc_,d3d_device_.Get()));
//
//    //// スワップチェーンの作成
//    //{
//    //  DXGI_SWAP_CHAIN_DESC desc = {};
//
//    //  desc.BufferDesc = actual_desc_;
//    //  desc.SampleDesc.Count	= 1;
//    //  desc.BufferUsage			= DXGI_USAGE_RENDER_TARGET_OUTPUT;
//    //  desc.BufferCount			= 1;
//    //  //      desc.SampleDesc = msaa;
//    //  desc.OutputWindow		= hwnd_;
//    //  //desc.SwapEffect			= DXGI_SWAP_EFFECT_DISCARD;
//    //  desc.Windowed			= TRUE;
//    //  desc.Flags = DXGI_SWAP_CHAIN_FLAG_GDI_COMPATIBLE;
//
//    //  CHK(dxgi_factory_->CreateSwapChain(d3d_device_,&desc,&dxgi_swap_chain_));
//
//    //}
//
//    // バックバッファの作成
//  
//    D3D11_TEXTURE2D_DESC desc = {0};
//    desc.Width = actual_desc_.Width;
//    desc.Height = actual_desc_.Height;
//    desc.Format = actual_desc_.Format;
//    desc.MipLevels = 1;
//    desc.SampleDesc.Count = 1;
//    desc.SampleDesc.Quality = 0;
//    desc.ArraySize = 1;
//    desc.Usage = D3D11_USAGE_DEFAULT;
//    desc.BindFlags = D3D11_BIND_RENDER_TARGET;
//    desc.MiscFlags = D3D11_RESOURCE_MISC_GDI_COMPATIBLE;
//    CHK(d3d_device_->CreateTexture2D(&desc,NULL,&back_buffer_));
//
//    // スワップチェーン依存リソースの作成
//    
//    create_swapchain_dependent_resources();
//
//    {
//      // バーテックスシェーダのコンパイル
//      ID3DBlobPtr vsblob,vserrblob;
//      DWORD compile_flag = D3DCOMPILE_ENABLE_STRICTNESS;
//#if defined( DEBUG ) || defined( _DEBUG )
//      compile_flag |= D3DCOMPILE_DEBUG;
//#endif
//
//	  HRESULT hr = D3DCompileFromFile
//		  (
//			L"dxgi_test.fx", NULL,D3D_COMPILE_STANDARD_FILE_INCLUDE , "VS", "vs_5_0", 
//        compile_flag, 0, &vsblob, &vserrblob );
//      if( FAILED( hr ) )
//      {
//        if( vserrblob != NULL )
//          OutputDebugStringA( (char*)vserrblob->GetBufferPointer() );
//        if( vserrblob ) vserrblob.Reset();
//        throw sf::win32_error_exception(hr);
//      }
//
//      // バーテックスシェーダの生成
//      CHK(d3d_device_->CreateVertexShader( vsblob->GetBufferPointer(), vsblob->GetBufferSize(), NULL, &v_shader_ ));
//
//      // 入力頂点レイアウトの定義
//      D3D11_INPUT_ELEMENT_DESC
//        layout[] = {
//          { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//          { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//          { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }};
//          ;
//
//          // 入力頂点レイアウトの生成
//          CHK(d3d_device_->CreateInputLayout( layout, ARRAYSIZE(layout), vsblob->GetBufferPointer(),
//            vsblob->GetBufferSize(), &input_layout_ ));
//          vsblob.Reset();
//    }
//
//    // 入力レイアウトの設定
//    d3d_context_->IASetInputLayout( input_layout_.Get() );
//
//    // ピクセル・シェーダーのコンパイル
//    {
//      ID3DBlobPtr psblob,pserror;
//      DWORD compile_flag = D3DCOMPILE_ENABLE_STRICTNESS;
//#if defined( DEBUG ) || defined( _DEBUG )
//      compile_flag |= D3DCOMPILE_DEBUG;
//#endif
//      HRESULT hr = D3DCompileFromFile( L"dxgi_test.fx", NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PS", "ps_5_0", 
//        compile_flag, 0,  &psblob, &pserror);
//      if( FAILED( hr ) )
//      {
//        if( pserror != NULL )
//          OutputDebugStringA( (char*)pserror->GetBufferPointer() );
//        safe_release(pserror);
//        throw sf::win32_error_exception(hr);
//      }
//
//      // ピクセルシェーダの作成
//      CHK(d3d_device_->CreatePixelShader( psblob->GetBufferPointer(), psblob->GetBufferSize(), NULL, &p_shader_ ));
//
//      psblob.Reset();
//    }
//
//    // バーテックスバッファの作成
//    // Create vertex buffer
//    simple_vertex vertices[] =
//    {
//      { XMFLOAT3( 0.0f, 0.0f,0.0f ),XMFLOAT3( 0.0f, 0.0f, -1.0f ) , XMFLOAT2( 0.0f, 0.0f ) },
//      { XMFLOAT3( 640.0f, 0.0f, 0.0f ),XMFLOAT3( 0.0f, 0.0f, -1.0f ), XMFLOAT2( 1.0f, 0.0f ) },
//      { XMFLOAT3( 0.0f, 480.0f, 0.0f ),XMFLOAT3( 0.0f, 0.0f, -1.0f ) , XMFLOAT2( 0.0f, 1.0f ) },
//      { XMFLOAT3( 640.0f, 480.0f, 0.0f ),XMFLOAT3( 0.0f, 0.0f, -1.0f ), XMFLOAT2( 1.0f, 1.0f ) }
//
//      //{ XMFLOAT3( -1.0f, -1.0f, 2.0f ),XMFLOAT3( 0.0f, 0.0f, -1.0f ) , XMFLOAT2( 1.0f, 1.0f ) },
//      //{ XMFLOAT3( 1.0f, -1.0f, 2.0f ),XMFLOAT3( 0.0f, 0.0f, -1.0f ), XMFLOAT2( 0.0f, 1.0f ) },
//      //{ XMFLOAT3( -1.0f, 1.0f, 2.0f ),XMFLOAT3( 0.0f, 0.0f, -1.0f ) , XMFLOAT2( 1.0f, 0.0f ) },
//      //{ XMFLOAT3( 1.0f, 1.0f, 2.0f ),XMFLOAT3( 0.0f, 0.0f, -1.0f ), XMFLOAT2( 0.0f, 0.0f ) }
//      // ---------------------
//      //{ XMFLOAT3( -1.0f, 1.0f, -1.0f ), XMFLOAT3( 0.0f, 1.0f, 0.0f ),XMFLOAT2( 0.0f, 0.0f ) },
//      //{ XMFLOAT3( 1.0f, 1.0f, -1.0f ),XMFLOAT3( 0.0f, 1.0f, 0.0f ), XMFLOAT2( 1.0f, 0.0f ) },
//      //{ XMFLOAT3( 1.0f, 1.0f, 1.0f ),XMFLOAT3( 0.0f, 1.0f, 0.0f ), XMFLOAT2( 1.0f, 1.0f ) },
//      //{ XMFLOAT3( -1.0f, 1.0f, 1.0f ), XMFLOAT3( 0.0f, 1.0f, 0.0f ) ,XMFLOAT2( 0.0f, 1.0f ) },
//
//      //{ XMFLOAT3( -1.0f, -1.0f, -1.0f ),XMFLOAT3( 0.0f, -1.0f, 0.0f ), XMFLOAT2( 0.0f, 0.0f ) },
//      //{ XMFLOAT3( 1.0f, -1.0f, -1.0f ),  XMFLOAT3( 0.0f, -1.0f, 0.0f ) ,XMFLOAT2( 1.0f, 0.0f ) },
//      //{ XMFLOAT3( 1.0f, -1.0f, 1.0f ), XMFLOAT3( 0.0f, -1.0f, 0.0f ) , XMFLOAT2( 1.0f, 1.0f ) },
//      //{ XMFLOAT3( -1.0f, -1.0f, 1.0f ), XMFLOAT3( 0.0f, -1.0f, 0.0f ),XMFLOAT2( 0.0f, 1.0f ) },
//
//      //{ XMFLOAT3( -1.0f, -1.0f, 1.0f ),XMFLOAT3( -1.0f, 0.0f, 0.0f ) , XMFLOAT2( 0.0f, 0.0f ) },
//      //{ XMFLOAT3( -1.0f, -1.0f, -1.0f ),XMFLOAT3( -1.0f, 0.0f, 0.0f ), XMFLOAT2( 1.0f, 0.0f ) },
//      //{ XMFLOAT3( -1.0f, 1.0f, -1.0f ), XMFLOAT3( -1.0f, 0.0f, 0.0f ),XMFLOAT2( 1.0f, 1.0f ) },
//      //{ XMFLOAT3( -1.0f, 1.0f, 1.0f ),XMFLOAT3( -1.0f, 0.0f, 0.0f ), XMFLOAT2( 0.0f, 1.0f ) },
//
//      //{ XMFLOAT3( 1.0f, -1.0f, 1.0f ),XMFLOAT3( 1.0f, 0.0f, 0.0f ), XMFLOAT2( 0.0f, 0.0f ) },
//      //{ XMFLOAT3( 1.0f, -1.0f, -1.0f ),XMFLOAT3( 1.0f, 0.0f, 0.0f ), XMFLOAT2( 1.0f, 0.0f ) },
//      //{ XMFLOAT3( 1.0f, 1.0f, -1.0f ),XMFLOAT3( 1.0f, 0.0f, 0.0f ), XMFLOAT2( 1.0f, 1.0f ) },
//      //{ XMFLOAT3( 1.0f, 1.0f, 1.0f ),XMFLOAT3( 1.0f, 0.0f, 0.0f ), XMFLOAT2( 0.0f, 1.0f ) },
//
//      //{ XMFLOAT3( -1.0f, -1.0f, -1.0f ),XMFLOAT3( 0.0f, 0.0f, -1.0f ) , XMFLOAT2( 0.0f, 0.0f ) },
//      //{ XMFLOAT3( 1.0f, -1.0f, -1.0f ),XMFLOAT3( 0.0f, 0.0f, -1.0f ), XMFLOAT2( 1.0f, 0.0f ) },
//      //{ XMFLOAT3( 1.0f, 1.0f, -1.0f ),XMFLOAT3( 0.0f, 0.0f, -1.0f ), XMFLOAT2( 1.0f, 1.0f ) },
//      //{ XMFLOAT3( -1.0f, 1.0f, -1.0f ),XMFLOAT3( 0.0f, 0.0f, -1.0f ) , XMFLOAT2( 0.0f, 1.0f ) },
//
//      //{ XMFLOAT3( -1.0f, -1.0f, 1.0f ), XMFLOAT3( 0.0f, 0.0f, 1.0f ) , XMFLOAT2( 0.0f, 0.0f ) },
//      //{ XMFLOAT3( 1.0f, -1.0f, 1.0f ), XMFLOAT3( 0.0f, 0.0f, 1.0f ) , XMFLOAT2( 1.0f, 0.0f ) },
//      //{ XMFLOAT3( 1.0f, 1.0f, 1.0f ),XMFLOAT3( 0.0f, 0.0f, 1.0f ), XMFLOAT2( 1.0f, 1.0f ) },
//      //{ XMFLOAT3( -1.0f, 1.0f, 1.0f ), XMFLOAT3( 0.0f, 0.0f, 1.0f ),XMFLOAT2( 0.0f, 1.0f ) }
//    };
//    //std::vector<simple_vertex> vertices = boost::assign::list_of<simple_vertex>
//    //
//    //    ( XMFLOAT3( -1.0f, 1.0f, -1.0f ), XMFLOAT2( 0.0f, 0.0f ) )
//    //    ( XMFLOAT3( 1.0f, 1.0f, -1.0f ), XMFLOAT2( 1.0f, 0.0f ) )
//    //    ( XMFLOAT3( 1.0f, 1.0f, 1.0f ), XMFLOAT2( 1.0f, 1.0f ) )
//    //    ( XMFLOAT3( -1.0f, 1.0f, 1.0f ), XMFLOAT2( 0.0f, 1.0f ) )
//
//    //    ( XMFLOAT3( -1.0f, -1.0f, -1.0f ), XMFLOAT2( 0.0f, 0.0f ) )
//    //    ( XMFLOAT3( 1.0f, -1.0f, -1.0f ), XMFLOAT2( 1.0f, 0.0f ) )
//    //    ( XMFLOAT3( 1.0f, -1.0f, 1.0f ), XMFLOAT2( 1.0f, 1.0f ) )
//    //    ( XMFLOAT3( -1.0f, -1.0f, 1.0f ), XMFLOAT2( 0.0f, 1.0f ) )
//
//    //    ( XMFLOAT3( -1.0f, -1.0f, 1.0f ), XMFLOAT2( 0.0f, 0.0f ) )
//    //    ( XMFLOAT3( -1.0f, -1.0f, -1.0f ), XMFLOAT2( 1.0f, 0.0f ) )
//    //    ( XMFLOAT3( -1.0f, 1.0f, -1.0f ), XMFLOAT2( 1.0f, 1.0f ) )
//    //    ( XMFLOAT3( -1.0f, 1.0f, 1.0f ), XMFLOAT2( 0.0f, 1.0f ) )
//
//    //    ( XMFLOAT3( 1.0f, -1.0f, 1.0f ), XMFLOAT2( 0.0f, 0.0f ) )
//    //    ( XMFLOAT3( 1.0f, -1.0f, -1.0f ), XMFLOAT2( 1.0f, 0.0f ) )
//    //    ( XMFLOAT3( 1.0f, 1.0f, -1.0f ), XMFLOAT2( 1.0f, 1.0f ) )
//    //    ( XMFLOAT3( 1.0f, 1.0f, 1.0f ), XMFLOAT2( 0.0f, 1.0f ) )
//
//    //    ( XMFLOAT3( -1.0f, -1.0f, -1.0f ), XMFLOAT2( 0.0f, 0.0f ) )
//    //    ( XMFLOAT3( 1.0f, -1.0f, -1.0f ), XMFLOAT2( 1.0f, 0.0f ) )
//    //    ( XMFLOAT3( 1.0f, 1.0f, -1.0f ), XMFLOAT2( 1.0f, 1.0f ) )
//    //    ( XMFLOAT3( -1.0f, 1.0f, -1.0f ), XMFLOAT2( 0.0f, 1.0f ) )
//
//    //    ( XMFLOAT3( -1.0f, -1.0f, 1.0f ), XMFLOAT2( 0.0f, 0.0f ) )
//    //    ( XMFLOAT3( 1.0f, -1.0f, 1.0f ), XMFLOAT2( 1.0f, 0.0f ) )
//    //    ( XMFLOAT3( 1.0f, 1.0f, 1.0f ), XMFLOAT2( 1.0f, 1.0f ) )
//    //    ( XMFLOAT3( -1.0f, 1.0f, 1.0f ), XMFLOAT2( 0.0f, 1.0f ) );
//
//    D3D11_BUFFER_DESC bd = {};
//    bd.Usage = D3D11_USAGE_DEFAULT;
//    bd.ByteWidth = sizeof( simple_vertex ) * 4;
//    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//    bd.CPUAccessFlags = 0;
//
//    D3D11_SUBRESOURCE_DATA init_data = {};
//    init_data.pSysMem = vertices;
//    CHK(d3d_device_->CreateBuffer( &bd, &init_data, &v_buffer_ ));
//
//    // 頂点バッファのセット
//    uint32_t stride = sizeof( simple_vertex );
//    uint32_t offset = 0;
//    d3d_context_->IASetVertexBuffers( 0, 1, v_buffer_.GetAddressOf(), &stride, &offset );
//
//    // インデックスバッファの生成
//    WORD indices[] =
//    {
//      0,1,2,
//      2,3,1
//      //3,1,0,
//      //2,1,3,
//      //6,4,5,
//      //7,4,6,
//      //11,9,8,
//      //10,9,11,
//      //14,12,13,
//      //15,12,14,
//      //19,17,16,
//      //18,17,19,
//      //22,20,21,
//      //23,20,22
//    };
//
//    bd.Usage = D3D11_USAGE_DEFAULT;
//    bd.ByteWidth = sizeof( WORD ) * 6;
//    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
//    bd.CPUAccessFlags = 0;
//    init_data.pSysMem = indices;
//    CHK(d3d_device_->CreateBuffer( &bd, &init_data, &i_buffer_ ));
//
//    // インデックスバッファのセット
//    d3d_context_->IASetIndexBuffer( i_buffer_.Get(), DXGI_FORMAT_R16_UINT, 0 );
//
//    // プリミティブの形態を指定する
//    d3d_context_->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
//
//    // 定数バッファを生成する。
//    bd.Usage = D3D11_USAGE_DEFAULT;
//    bd.ByteWidth = sizeof(cb_never_changes);
//    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
//    bd.CPUAccessFlags = 0;
//    CHK(d3d_device_->CreateBuffer( &bd, NULL, &cb_never_changes_ ));
//
//    bd.ByteWidth = sizeof(cb_change_on_resize);
//    CHK(d3d_device_->CreateBuffer( &bd, NULL, &cb_change_on_resize_ ));
//
//    bd.ByteWidth = sizeof(cb_changes_every_frame);
//    CHK(d3d_device_->CreateBuffer( &bd, NULL, &cb_changes_every_frame_ ));
//
//    // テクスチャのロード
//	ID3D11ResourcePtr ptr;
//    CHK(CreateDDSTextureFromFile( d3d_device_.Get(), L"SF.dds", &ptr, &shader_res_view_, NULL ));
////    CHK(CreateDDSTextureFromFile( d3d_device_, L"SF.dds", NULL, NULL, &shader_res_view_, NULL ));
//
//    // サンプルステートの生成
//    D3D11_SAMPLER_DESC sdesc = {};
//    sdesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
//    sdesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
//    sdesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
//    sdesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
//    sdesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
//    sdesc.MinLOD = 0;
//    sdesc.MaxLOD = D3D11_FLOAT32_MAX;
//    CHK(d3d_device_->CreateSamplerState( &sdesc, &sampler_state_ ));
//
//    // ワールド座標変換行列のセットアップ
//    mat_world_ = XMMatrixIdentity();
//
//    //g_vMeshColor( 0.7f, 0.7f, 0.7f, 1.0f );
//    // 
//    init_view_matrix();
//
//
//
//    // 動的テクスチャの生成
//    {
//      //D3D11_TEXTURE2D_DESC desc = {0};
//      //desc.Width = 256;
//      //desc.Height = 256;
//      //desc.Format = actual_desc_.Format;
//      //desc.MipLevels = 1;
//      //desc.SampleDesc.Count = 1;
//      //desc.SampleDesc.Quality = 0;
//      //desc.ArraySize = 1;
//      //desc.Usage = D3D11_USAGE_DEFAULT;
//      //desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
//      //// desc.MiscFlags = D3D11_RESOURCE_MISC_GDI_COMPATIBLE;
//      //CHK(d3d_device_->CreateTexture2D(&desc,NULL,&cube_texture_));
//      //CHK(d3d_device_->CreateRenderTargetView(cube_texture_,0,&cube_view_));
//
//      //// 深度バッファの作成
//      //D3D11_TEXTURE2D_DESC depth = {} ;
//      //depth.Width = desc.Width;//rc.right - rc.left;client_width_;
//      //depth.Height = desc.Height;//rc.bottom - rc.top;client_height_;
//      //depth.MipLevels = 1;
//      //depth.ArraySize = 1;
//      //depth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
//      //depth.SampleDesc.Count = 1;
//      //depth.SampleDesc.Quality = 0;
//      //depth.Usage = D3D11_USAGE_DEFAULT;
//      //depth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
//      //depth.CPUAccessFlags = 0;
//      //depth.MiscFlags = 0;
//      //CHK(d3d_device_->CreateTexture2D( &depth, NULL, &cube_depth_texture_ ));
//
//      //D3D11_DEPTH_STENCIL_VIEW_DESC dsv = {};
//      //dsv.Format = depth.Format;
//      //dsv.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
//      //dsv.Texture2D.MipSlice = 0;
//      //CHK(d3d_device_->CreateDepthStencilView( cube_depth_texture_, &dsv, &cube_depth_view_ ));
//      //CHK(d3d_device_->CreateShaderResourceView(cube_texture_,0,&cube_shader_res_view_));
//
//      //D3D11_SAMPLER_DESC sdesc = {};
//      //sdesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
//      //sdesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
//      //sdesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
//      //sdesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
//      //sdesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
//      //sdesc.MinLOD = 0;
//      //sdesc.MaxLOD = D3D11_FLOAT32_MAX;
//      //CHK(d3d_device_->CreateSamplerState( &sdesc, &cube_sampler_state_ ));
//      //cube_mat_projection_ = XMMatrixPerspectiveFovLH( XM_PIDIV4, /*(rc.right - rc.left)/(rc.bottom - rc.top)*/256 / 256, 0.01f, 100.0f );
//
//    }
//    // 
//
//    init_ = true;// 初期化完了
//  }



void  window_renderer::init_view_matrix()
{
  // ビュー行列のセットアップ
  ////基本value設定
  //float aspect = (float) width_ / height_;	    //アスペクト比(高さを1としたときの幅)
  //float depth = 1.0f;										//奥行きZ
  //float fovy  = (float)atan(1.0f / depth) * 2.0f;					//視野をZ=0でデバイスの幅と高さに合わせる

  //XMVECTOR eye = { 0.0f, 0.0f, -depth, 0.0f };
  //XMVECTOR at = { 0.0f, 0.0f, 0.0f, 0.0f};
  //XMVECTOR up = { 0.0f, 1.0f, 0.0f, 0.0f };
  //mat_view_ = XMMatrixLookAtLH( eye, at, up );
  //cb_never_changes cnc;
  //cnc.mView = XMMatrixTranspose( mat_view_ );
  ////cnc.vLightColor = XMFLOAT4( 1.0f, 0.5f, 0.5f, 1.0f );
  //cnc.vLightDir =  XMFLOAT4(0.577f, 0.577f, -0.977f, 1.0f);
  //// 定数バッファに格納
  //d3d_context_->UpdateSubresource( cb_never_changes_.Get(), 0, NULL, &cnc, 0, 0 );

  //// 投影行列のセットアップ

  ////mat_projection_ = XMMatrixPerspectiveFovLH( XM_PIDIV4, /*(rc.right - rc.left)/(rc.bottom - rc.top)*/width_ / height_, 0.01f, 100.0f );
  ////mat_projection_ = XMMatrixPerspectiveFovLH( fovy, aspect, 0.01f, 100.0f );
  //mat_projection_ = XMMatrixPerspectiveFovLH( fovy, 1.0, 0.0001f, 100.0f );
  //cb_change_on_resize ccor;
  //ccor.mProjection = XMMatrixTranspose( mat_projection_ );
  //// 定数バッファに格納
  //d3d_context_->UpdateSubresource( cb_change_on_resize_.Get(), 0, NULL, &ccor, 0, 0 );
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
  d3d_context_->UpdateSubresource(cb_never_changes_.Get(), 0, NULL, &cnc, 0, 0);

  // 投影行列のセットアップ
  mat_projection_ = XMMatrixPerspectiveFovLH(XM_PIDIV4, window_.width() / window_.height(), 0.01f, 100.0f);
  cb_change_on_resize ccor;
  ccor.mProjection = XMMatrixTranspose(mat_projection_);
  // 定数バッファに格納
  d3d_context_->UpdateSubresource(cb_change_on_resize_.Get(), 0, NULL, &ccor, 0, 0);


}


void  window_renderer::create_swapchain_dependent_resources()
{

  create_d2d_render_target();

  // Direct 3Dリソースの作成 /////////////////////////////////////////

  CHK(dxgi_swap_chain_->GetBuffer(0, IID_PPV_ARGS(&back_buffer_)));

  // ビューの作成
  CHK(d3d_device_->CreateRenderTargetView(back_buffer_.Get(), 0, &d3d_render_target_view_));
  D3D11_TEXTURE2D_DESC desc = {};
  back_buffer_->GetDesc(&desc);

  // 深度バッファの作成
  D3D11_TEXTURE2D_DESC depth = {};
  depth.Width = desc.Width;//rc.right - rc.left;client_width_;
  depth.Height = desc.Height;//rc.bottom - rc.top;client_height_;
  depth.MipLevels = 1;
  depth.ArraySize = 1;
  depth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
  depth.SampleDesc.Count = 1;
  depth.SampleDesc.Quality = 0;
  depth.Usage = D3D11_USAGE_DEFAULT;
  depth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
  depth.CPUAccessFlags = 0;
  depth.MiscFlags = 0;
  CHK(d3d_device_->CreateTexture2D(&depth, NULL, &d3d_depth_texture_));

  D3D11_DEPTH_STENCIL_VIEW_DESC dsv = {};
  dsv.Format = depth.Format;
  dsv.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
  dsv.Texture2D.MipSlice = 0;
  CHK(d3d_device_->CreateDepthStencilView(d3d_depth_texture_.Get(), &dsv, &depth_view_));

  // OMステージに登録する
  d3d_context_->OMSetRenderTargets(1, d3d_render_target_view_.GetAddressOf(), depth_view_.Get());

  // ビューポートの設定
  D3D11_VIEWPORT vp;
  vp.Width = depth.Width;//client_width_;
  vp.Height = depth.Height;//client_height_;
  vp.MinDepth = 0.0f;
  vp.MaxDepth = 1.0f;
  vp.TopLeftX = 0;
  vp.TopLeftY = 0;
  d3d_context_->RSSetViewports(1, &vp);

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
  d3d_device_->CreateRasterizerState(&hRasterizerDesc, &hpRasterizerState);
  d3d_context_->RSSetState(hpRasterizerState);
}


void  window_renderer::discard_swapchain_dependent_resources()
{

  if (window_.is_fullscreen()){
    dxgi_swap_chain_->SetFullscreenState(FALSE, nullptr);
  }

  discard_d2d_render_target();

  dxgi_back_buffer_.Reset();

  depth_view_.Reset();
  d3d_depth_texture_.Reset();
  d3d_render_target_view_.Reset();
  back_buffer_.Reset();

}


void  window_renderer::discard_device()
{
  //safe_release(sampler_state_);
  //safe_release(shader_res_view_);
  //safe_release(cb_changes_every_frame_);
  //safe_release(cb_change_on_resize_);
  //safe_release(cb_never_changes_);
  //safe_release(i_buffer_);
  //safe_release(v_buffer_);
  //safe_release(p_shader_);
  //safe_release(input_layout_);
  //safe_release(v_shader_);

  discard_swapchain_dependent_resources();
  dxgi_swap_chain_.Reset();

  //safe_release(cube_sampler_state_);
  //safe_release(cube_shader_res_view_);
  //safe_release(cube_view_);
  //safe_release(cube_depth_view_);
  //safe_release(cube_texture_);
  //safe_release(cube_depth_texture_);
  //safe_release(render_target_);
  //    safe_release(dxgi_swap_chain_);

  d2d_context_.Reset();
  d2d_device_.Reset();

  dxgi_output_.Reset();
  dxgi_device_.Reset();


  d3d_context_.Reset();
  d3d_device_.Reset();
  dxgi_adapter_.Reset();
  dxgi_factory_.Reset();
}


void  window_renderer::discard_device_independant_resources(){
  d2d_factory_.Reset();
  write_factory_.Reset();
}

// スワップチェインをリストアする

void  window_renderer::restore_swapchain_and_dependent_resources()
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



void  window_renderer::get_dxgi_information()
{
  int i = 0;

  while (1){
    IDXGIAdapter1Ptr adapter;
    HRESULT hr = dxgi_factory_->EnumAdapters1(i, &adapter);
    if (hr == DXGI_ERROR_NOT_FOUND)
    {
      break;
    }
    DXGI_ADAPTER_DESC1 desc;
    adapter->GetDesc1(&desc);
    //adapter->CheckInterfaceSupport();

    debug_out(boost::wformat(L"%s \n") % desc.Description);
    debug_out(boost::wformat(L"%d \n") % desc.DedicatedVideoMemory);
    IDXGIDevice1Ptr device;

    uint32_t oi = 0;


    while (1)
    {
      IDXGIOutputPtr output;
      if (adapter->EnumOutputs(oi, &output) == DXGI_ERROR_NOT_FOUND){
        break;
      }
      DXGI_OUTPUT_DESC output_desc;
      output->GetDesc(&output_desc);
      UINT num = 0;
      DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
      UINT flags = DXGI_ENUM_MODES_INTERLACED | DXGI_ENUM_MODES_SCALING;

      output->GetDisplayModeList(format, flags, &num, 0);
      boost::shared_array<DXGI_MODE_DESC> disp_modes(new DXGI_MODE_DESC[num]);
      output->GetDisplayModeList(format, flags, &num, &disp_modes[0]);
      //output->GetFrameStatistics
      for (uint32_t mode_index = 0; mode_index < num; ++mode_index)
      {
        DXGI_MODE_DESC& mode(disp_modes[mode_index]);
        ::OutputDebugStringW((boost::wformat(L"Format: %s %s \n Width: %d Height: %d RefleshRate: %d/%d Scaling:%s %s \n Scanline: %s %s \n")
          % display_modes[mode.Format].name % display_modes[mode.Format].description
          %  mode.Width % mode.Height
          %  mode.RefreshRate.Numerator % mode.RefreshRate.Denominator
          %  scalings[mode.Scaling].name %  scalings[mode.Scaling].description
          %  scanline_orders[mode.ScanlineOrdering].name
          %  scanline_orders[mode.ScanlineOrdering].description).str().c_str());
      }
      //        output->
      OutputDebugStringW((boost::wformat(L"%s \n") % output_desc.DeviceName).str().c_str());
      oi++;
    }

    adapter.Reset();
    ++i;
  }
}


void window_renderer::render()
{
  if (window_.is_activate()){

    static float rot = 0.0f;
    float color[4] = { 0.0f, 0.0f, 0.0f, 0.5f };

    // 描画ターゲットのクリア
    d3d_context_->ClearRenderTargetView(d3d_render_target_view_.Get(), color);
    // 深度バッファのクリア
    d3d_context_->ClearDepthStencilView(depth_view_.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

    d2d_context_->BeginDraw();
    //d2d_context_->Clear();


    //thunk_proc_ = (WNDPROC)thunk_.getCode();
    D2D_RECT_F layout_rect_ = D2D1::RectF(0.0f, 100.0f, 400.0f, 100.0f);
    // Text Formatの作成
    //CHK(write_factory_->CreateTextFormat(
    //  L"ＭＳ ゴシック",                // Font family name.
    //  NULL,                       // Font collection (NULL sets it to use the system font collection).
    //  DWRITE_FONT_WEIGHT_REGULAR,
    //  DWRITE_FONT_STYLE_NORMAL,
    //  DWRITE_FONT_STRETCH_NORMAL,
    //  16.000f,
    //  L"ja-jp",
    //  &write_text_format_
    //  ));

    d2d_context_->SetTransform(D2D1::Matrix3x2F::Identity());
    ID2D1SolidColorBrushPtr brush, line_brush;
    d2d_context_->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::OrangeRed), &brush);
    d2d_context_->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.5f), &line_brush);

    {
      D2D_POINT_2F start, end;
      for (float i = 0; i < window_.width() + 1.0f; i += 16.0f)
      {
        start.x = end.x = i;
        end.y = window_.height();
        start.y = 0.0f;
        d2d_context_->DrawLine(start, end, line_brush.Get(), 0.5f);
      }

      for (float i = 0; i < window_.height() + 1.0f; i += 16.0f)
      {
        start.y = end.y = i;
        end.x = window_.width();
        start.x = 0.0f;
        d2d_context_->DrawLine(start, end, line_brush.Get(), 0.5f);
      }

    }

    static int count;
    count++;
    //std::wstring m((boost::wformat(L"ＡＡＢＢＣＣＤＤＥＥＦＦTEST表示%08d　 ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789アイウエオあいうえお") % count).str());
    //d2d_context_->DrawTextW(
    //  m.c_str(),
    //  m.size(),
    //  write_text_format_.Get(),
    //  layout_rect_,
    //  brush.Get());

    d2d_context_->EndDraw();


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
    d3d_context_->UpdateSubresource(cb_changes_every_frame_.Get(), 0, NULL, &cb, 0, 0);
    d3d_context_->OMSetRenderTargets(1, d3d_render_target_view_.GetAddressOf(), depth_view_.Get());

    // 四角形
    d3d_context_->VSSetShader(v_shader_.Get(), NULL, 0);
    d3d_context_->VSSetConstantBuffers(0, 1, cb_never_changes_.GetAddressOf());
    d3d_context_->VSSetConstantBuffers(1, 1, cb_change_on_resize_.GetAddressOf());
    d3d_context_->VSSetConstantBuffers(2, 1, cb_changes_every_frame_.GetAddressOf());
    d3d_context_->PSSetShader(p_shader_.Get(), NULL, 0);
    d3d_context_->PSSetConstantBuffers(2, 1, cb_changes_every_frame_.GetAddressOf());
    d3d_context_->PSSetShaderResources(0, 1, shader_res_view_.GetAddressOf());
    d3d_context_->PSSetSamplers(0, 1, sampler_state_.GetAddressOf());

    d3d_context_->DrawIndexed(36, 0, 0);


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

  /*
  if(init_)
  {
  static float rot = 0.0f;

  float color[4] = { 0.0f, 0.0f, 0.0f, 0.5f };

  // 描画ターゲットのクリア
  d3d_context_->ClearRenderTargetView(d3d_render_target_view_.Get(),color);
  // 深度バッファのクリア
  d3d_context_->ClearDepthStencilView(depth_view_.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0 );

  // 色の変更
  mesh_color_.x = 1.0f;
  mesh_color_.y = 1.0f;
  mesh_color_.z = 1.0f;

  // 定数更新

  cb_changes_every_frame cb;

  mat_world_ = XMMatrixIdentity();
  //      mat_world_._11 = 2.0f / width_;
  mat_world_.r[0].m128_f32[0] = 2.0f / width_;
  //      mat_world_._22 = 2.0f  / height_ * -1.0f;
  mat_world_.r[1].m128_f32[1] = 2.0f  / height_ * -1.0f;
  //      mat_world_._41 = -1.0f;
  mat_world_.r[3].m128_f32[0] = -1.0f;
  //      mat_world_._42 = 1.0f;
  mat_world_.r[3].m128_f32[1] = 1.0f;
  mat_world_ *= XMMatrixRotationX(rot);
  rot += 0.005f;
  cb.mWorld =  XMMatrixTranspose(mat_world_);
  cb.vLightColor = mesh_color_;
  d3d_context_->UpdateSubresource( cb_changes_every_frame_.Get(), 0, NULL, &cb, 0, 0 );

  // 四角形
  d3d_context_->VSSetShader( v_shader_.Get(), NULL, 0 );
  d3d_context_->VSSetConstantBuffers( 0, 1, cb_never_changes_.GetAddressOf() );
  d3d_context_->VSSetConstantBuffers( 1, 1, cb_change_on_resize_.GetAddressOf() );
  d3d_context_->VSSetConstantBuffers( 2, 1, cb_changes_every_frame_.GetAddressOf() );
  d3d_context_->PSSetShader( p_shader_.Get(), NULL, 0 );
  d3d_context_->PSSetConstantBuffers( 2, 1, cb_changes_every_frame_.GetAddressOf() );
  d3d_context_->PSSetShaderResources( 0, 1, shader_res_view_.GetAddressOf() );
  d3d_context_->PSSetSamplers( 0, 1, sampler_state_.GetAddressOf() );

  d3d_context_->DrawIndexed( 6, 0, 0 );

  // 画面に転送
  IDXGISurface1Ptr surface;
  CHK(back_buffer_.As<IDXGISurface1>(&surface));
  HDC sdc;
  CHK(surface->GetDC( FALSE, &sdc ));

  //get_dc ddc(hwnd_);
  get_window_dc ddc(hwnd_);
  //      RECT rc;
  //      GetWindowRect(hwnd_,&rc);
  //      POINT wnd_pos = {rc.left,rc.top};
  //      SIZE  wnd_size = {width_,height_};
  //
  //BLENDFUNCTION blend;
  //    blend.BlendOp = AC_SRC_OVER;
  //    blend.BlendFlags = 0;
  //    blend.SourceConstantAlpha = 128; // 不透明度（レイヤードウィンドウ全体のアルファvalue）
  //    blend.AlphaFormat = AC_SRC_ALPHA;
  // デバイスコンテキストにおけるレイヤの位置
  POINT po;
  po.x = po.y = 0;
  BOOL err;
  err = BitBlt(ddc.get(),0,0,width_,height_,sdc,0,0,SRCCOPY);
  //      err = AlphaBlend(ddc.get(),0,0,width_,height_,sdc,0,0,width_,height_,blend);
  //err = UpdateLayeredWindow(hwnd_, ddc.get(), &wnd_pos, &wnd_size, sdc, &po, RGB(255,0,0), &blend, ULW_ALPHA | ULW_COLORKEY );
  BOOST_ASSERT(err == TRUE);
  surface->ReleaseDC( NULL);
  surface.Reset();
  // OMステージに登録する
  d3d_context_->OMSetRenderTargets( 1, d3d_render_target_view_.GetAddressOf(), depth_view_.Get() );
  }
  */
}


