#include "stdafx.h"
#include "graphics.h"
#define BOOST_ASSIGN_MAX_PARAMS 7
#include <boost/assign.hpp>
#include <boost/assign/ptr_list_of.hpp>
#include <boost/assign/ptr_list_inserter.hpp>

using namespace sf;


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

graphics::graphics()
{
  init();
}

graphics::~graphics()
{
  discard_device();
  discard_device_independant_resources();
}

void graphics::init()
{
  create_device_independent_resources();
  create_device();
}

void  graphics::create_device_independent_resources()
{
  // DXGI Factory の 生成

  //if(!dxgi_factory_)
  //{
  //  IDXGIFactory1Ptr factory;
  //  CHK(CreateDXGIFactory1(__uuidof(IDXGIFactory1),reinterpret_cast<void**>(factory.GetAddressOf())));
  //  factory.As(&dxgi_factory_);

  //  get_dxgi_information();
  //}

  // Direc2D Factoryの生成
  if (!d2d_factory_){
    ID2D1Factory1Ptr factory;
    D2D1_FACTORY_OPTIONS options = {};
#if defined(DEBUG) || defined(_DEBUG)
    options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif
    CHK(D2D1CreateFactory(
      D2D1_FACTORY_TYPE_SINGLE_THREADED,
      __uuidof(ID2D1Factory1),
      &options,
      &factory
      ));
//   CHK(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &factory));
    CHK(factory.As(&d2d_factory_));

  }

  // DirectWrite Factoryの生成

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

void  graphics::create_device(){

  // Feature Level配列のセットアップ
  std::vector<D3D_FEATURE_LEVEL> feature_levels = {
    D3D_FEATURE_LEVEL_11_1,
    D3D_FEATURE_LEVEL_11_0
  };

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

  CHK(dxgi_device_->SetMaximumFrameLatency(1));

  // Direct2Dデバイスの作成
  CHK(d2d_factory_->CreateDevice(dxgi_device_.Get(), &d2d_device_));

  // Direct2Dデバイスコンテキストの作成
  {
    ID2D1DeviceContextPtr context;
    CHK(d2d_device_->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &context));
    context.As(&d2d_context_);
  }

  // Direct Composition デバイスの作成
  {
    IDCompositionDevicePtr dcomp_device;
    CHK(DCompositionCreateDevice2(dxgi_device_.Get(), __uuidof(IDCompositionDevice), &dcomp_device));
    dcomp_device.As(&dcomp_device2_);
    dcomp_device2_.As(&dcomp_desktop_device_);
    //CHK(dcomp_desktop_device_->CreateTargetForHwnd((HWND) window_.raw_handle(), TRUE, &dcomp_target_));
  }
}

void graphics::discard_device()
{

  before_device_discarded_(*this);

  d2d_context_.Reset();
  d2d_device_.Reset();

  dxgi_output_.Reset();
  dxgi_device_.Reset();

  dcomp_device2_.Reset();
  dcomp_desktop_device_.Reset();

  d3d_context_.Reset();
  d3d_device_.Reset();

  dxgi_adapter_.Reset();
  dxgi_factory_.Reset();

  after_device_discarded_(*this);

}

void  graphics::discard_device_independant_resources(){
  d2d_factory_.Reset();
  write_factory_.Reset();
}

void  graphics::get_dxgi_information()
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

    DOUT(boost::wformat(L"%s \n") % desc.Description);
    DOUT(boost::wformat(L"%d \n") % desc.DedicatedVideoMemory);
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