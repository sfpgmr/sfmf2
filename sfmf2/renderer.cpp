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
  // ALT+ENTER���֎~�i�t���X�N���[���̂݁j
  CHK(dxgi_factory_->MakeWindowAssociation((HWND) window_.raw_handle(), DXGI_MWA_NO_ALT_ENTER));
}

window_renderer::~window_renderer()
{
}

/////////////////////////////////////////////////////////////////////////////
// Direct X�֌W�̃R�[�h
/////////////////////////////////////////////////////////////////////////////
// �ėp���i�[�p
struct mode_info
{
  mode_info(const std::wstring& n, const std::wstring& d) : name(n), description(d) {}
  std::wstring name;
  std::wstring description;
};

// �f�B�X�v���C���[�h
struct display_mode
{
  display_mode(const std::wstring& n, const std::wstring& d) : name(n), description(d) {}
  std::wstring name;
  std::wstring description;
};

std::vector<mode_info> display_modes =
boost::assign::list_of<mode_info>
(L"DXGI_FORMAT_UNKNOWN", L"�t�H�[�}�b�g���s��")
(L"DXGI_FORMAT_R32G32B32A32_TYPELESS", L"4 �����A128 �r�b�g�^�Ȃ��t�H�[�}�b�g 1")
(L"DXGI_FORMAT_R32G32B32A32_FLOAT", L"4 �����A128 �r�b�g���������_�t�H�[�}�b�g 1")
(L"DXGI_FORMAT_R32G32B32A32_UINT", L"4 �����A128 �r�b�g�����Ȃ������t�H�[�}�b�g 1")
(L"DXGI_FORMAT_R32G32B32A32_SINT", L"4 �����A128 �r�b�g�����t�������t�H�[�}�b�g 1")
(L"DXGI_FORMAT_R32G32B32_TYPELESS", L"3 �����A96 �r�b�g�^�Ȃ��t�H�[�}�b�g")
(L"DXGI_FORMAT_R32G32B32_FLOAT", L"3 �����A96 �r�b�g���������_�t�H�[�}�b�g")
(L"DXGI_FORMAT_R32G32B32_UINT", L"3 �����A96 �r�b�g�����Ȃ������t�H�[�}�b�g")
(L"DXGI_FORMAT_R32G32B32_SINT", L"3 �����A96 �r�b�g�����t�������t�H�[�}�b�g")
(L"DXGI_FORMAT_R16G16B16A16_TYPELESS", L"4 �����A64 �r�b�g�^�Ȃ��t�H�[�}�b�g")
(L"DXGI_FORMAT_R16G16B16A16_FLOAT", L"4 �����A64 �r�b�g���������_�t�H�[�}�b�g")
(L"DXGI_FORMAT_R16G16B16A16_UNORM", L"4 �����A64 �r�b�g�����Ȃ������t�H�[�}�b�g")
(L"DXGI_FORMAT_R16G16B16A16_UINT", L"4 �����A64 �r�b�g�����Ȃ������t�H�[�}�b�g")
(L"DXGI_FORMAT_R16G16B16A16_SNORM", L"4 �����A64 �r�b�g�����t�������t�H�[�}�b�g")
(L"DXGI_FORMAT_R16G16B16A16_SINT", L"4 �����A64 �r�b�g�����t�������t�H�[�}�b�g")
(L"DXGI_FORMAT_R32G32_TYPELESS", L"2 �����A64 �r�b�g�^�Ȃ��t�H�[�}�b�g")
(L"DXGI_FORMAT_R32G32_FLOAT", L"2 �����A64 �r�b�g���������_�t�H�[�}�b�g")
(L"DXGI_FORMAT_R32G32_UINT", L"2 �����A64 �r�b�g�����Ȃ������t�H�[�}�b�g")
(L"DXGI_FORMAT_R32G32_SINT", L"2 �����A64 �r�b�g�����t�������t�H�[�}�b�g")
(L"DXGI_FORMAT_R32G8X24_TYPELESS", L"2 �����A64 �r�b�g�^�Ȃ��t�H�[�}�b�g")
(L"DXGI_FORMAT_D32_FLOAT_S8X24_UINT", L"32 �r�b�g���������_�����A����� 2 �̕����Ȃ����������ł� (�ǉ��� 32 �r�b�g���܂�)�B")
(L"DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS", L"32 �r�b�g���������_�����A����� 2 �̌^�Ȃ������ł� (�ǉ��� 32 �r�b�g���܂�)�B")
(L"DXGI_FORMAT_X32_TYPELESS_G8X24_UINT", L"32 �r�b�g�^�Ȃ������A����� 2 �̕����Ȃ����������ł� (�ǉ��� 32 �r�b�g���܂�)�B")
(L"DXGI_FORMAT_R10G10B10A2_TYPELESS", L"4 �����A32 �r�b�g�^�Ȃ��t�H�[�}�b�g")
(L"DXGI_FORMAT_R10G10B10A2_UNORM", L"4 �����A32 �r�b�g�����Ȃ������t�H�[�}�b�g")
(L"DXGI_FORMAT_R10G10B10A2_UINT", L"4 �����A32 �r�b�g�����Ȃ������t�H�[�}�b�g")
(L"DXGI_FORMAT_R11G11B10_FLOAT", L"3 �����A32 �r�b�g���������_�t�H�[�}�b�g")
(L"DXGI_FORMAT_R8G8B8A8_TYPELESS", L"3 �����A32 �r�b�g�^�Ȃ��t�H�[�}�b�g")
(L"DXGI_FORMAT_R8G8B8A8_UNORM", L"4 �����A32 �r�b�g�����Ȃ������t�H�[�}�b�g")
(L"DXGI_FORMAT_R8G8B8A8_UNORM_SRGB", L"4 �����A32 �r�b�g�����Ȃ����K������ sRGB �t�H�[�}�b�g")
(L"DXGI_FORMAT_R8G8B8A8_UINT", L"4 �����A32 �r�b�g�����Ȃ������t�H�[�}�b�g")
(L"DXGI_FORMAT_R8G8B8A8_SNORM", L"3 �����A32 �r�b�g�����t�������t�H�[�}�b�g")
(L"DXGI_FORMAT_R8G8B8A8_SINT", L"3 �����A32 �r�b�g�����t�������t�H�[�}�b�g")
(L"DXGI_FORMAT_R16G16_TYPELESS", L"2 �����A32 �r�b�g�^�Ȃ��t�H�[�}�b�g")
(L"DXGI_FORMAT_R16G16_FLOAT", L"2 �����A32 �r�b�g���������_�t�H�[�}�b�g")
(L"DXGI_FORMAT_R16G16_UNORM", L"2 �����A32 �r�b�g�����Ȃ������t�H�[�}�b�g")
(L"DXGI_FORMAT_R16G16_UINT", L"2 �����A32 �r�b�g�����Ȃ������t�H�[�}�b�g")
(L"DXGI_FORMAT_R16G16_SNORM", L"2 �����A32 �r�b�g�����t�������t�H�[�}�b�g")
(L"DXGI_FORMAT_R16G16_SINT", L"2 �����A32 �r�b�g�����t�������t�H�[�}�b�g")
(L"DXGI_FORMAT_R32_TYPELESS", L"1 �����A32 �r�b�g�^�Ȃ��t�H�[�}�b�g")
(L"DXGI_FORMAT_D32_FLOAT", L"1 �����A32 �r�b�g���������_�t�H�[�}�b�g")
(L"DXGI_FORMAT_R32_FLOAT", L"1 �����A32 �r�b�g���������_�t�H�[�}�b�g")
(L"DXGI_FORMAT_R32_UINT", L"1 �����A32 �r�b�g�����Ȃ������t�H�[�}�b�g")
(L"DXGI_FORMAT_R32_SINT", L"1 �����A32 �r�b�g�����t�������t�H�[�}�b�g")
(L"DXGI_FORMAT_R24G8_TYPELESS", L"2 �����A32 �r�b�g�^�Ȃ��t�H�[�}�b�g")
(L"DXGI_FORMAT_D24_UNORM_S8_UINT", L"�[�x�`�����l���� 24 �r�b�g�A�X�e���V�� �`�����l���� 8 �r�b�g���g�p���� 32 �r�b�g Z �o�b�t�@�[ �t�H�[�}�b�g")
(L"DXGI_FORMAT_R24_UNORM_X8_TYPELESS", L"1 �����A24 �r�b�g�����Ȃ����K�������ƒǉ��̌^�Ȃ� 8 �r�b�g���܂ށA32 �r�b�g �t�H�[�}�b�g")
(L"DXGI_FORMAT_X24_TYPELESS_G8_UINT", L"1 �����A24 �r�b�g�^�Ȃ��t�H�[�}�b�g�ƒǉ��� 8 �r�b�g�����Ȃ������������܂ށA32 �r�b�g �t�H�[�}�b�g")
(L"DXGI_FORMAT_R8G8_TYPELESS", L"2 �����A16 �r�b�g�^�Ȃ��t�H�[�}�b�g")
(L"DXGI_FORMAT_R8G8_UNORM", L"2 �����A16 �r�b�g�����Ȃ������t�H�[�}�b�g")
(L"DXGI_FORMAT_R8G8_UINT", L"2 �����A16 �r�b�g�����Ȃ������t�H�[�}�b�g")
(L"DXGI_FORMAT_R8G8_SNORM", L"2 �����A16 �r�b�g�����t�������t�H�[�}�b�g")
(L"DXGI_FORMAT_R8G8_SINT", L"2 �����A16 �r�b�g�����t�������t�H�[�}�b�g")
(L"DXGI_FORMAT_R16_TYPELESS", L"1 �����A16 �r�b�g�^�Ȃ��t�H�[�}�b�g")
(L"DXGI_FORMAT_R16_FLOAT", L"1 �����A16 �r�b�g���������_�t�H�[�}�b�g")
(L"DXGI_FORMAT_D16_UNORM", L"1 �����A16 �r�b�g�����Ȃ����K�������t�H�[�}�b�g")
(L"DXGI_FORMAT_R16_UNORM", L"1 �����A16 �r�b�g�����Ȃ������t�H�[�}�b�g")
(L"DXGI_FORMAT_R16_UINT", L"1 �����A16 �r�b�g�����Ȃ������t�H�[�}�b�g")
(L"DXGI_FORMAT_R16_SNORM", L"1 �����A16 �r�b�g�����t�������t�H�[�}�b�g")
(L"DXGI_FORMAT_R16_SINT", L"1 �����A16 �r�b�g�����t�������t�H�[�}�b�g")
(L"DXGI_FORMAT_R8_TYPELESS", L"1 �����A8 �r�b�g�^�Ȃ��t�H�[�}�b�g")
(L"DXGI_FORMAT_R8_UNORM", L"1 �����A8 �r�b�g�����Ȃ������t�H�[�}�b�g")
(L"DXGI_FORMAT_R8_UINT", L"1 �����A8 �r�b�g�����Ȃ������t�H�[�}�b�g")
(L"DXGI_FORMAT_R8_SNORM", L"1 �����A8 �r�b�g�����t�������t�H�[�}�b�g")
(L"DXGI_FORMAT_R8_SINT", L"1 �����A8 �r�b�g�����t�������t�H�[�}�b�g")
(L"DXGI_FORMAT_A8_UNORM", L"1 �����A8 �r�b�g�����Ȃ������t�H�[�}�b�g")
(L"DXGI_FORMAT_R1_UNORM", L"1 �����A1 �r�b�g�����Ȃ����K�������t�H�[�}�b�g 2.")
(L"DXGI_FORMAT_R9G9B9E5_SHAREDEXP", L"4 �����A32 �r�b�g���������_�t�H�[�}�b�g 2.")
(L"DXGI_FORMAT_R8G8_B8G8_UNORM", L"4 �����A32 �r�b�g�����Ȃ����K�������t�H�[�}�b�g 3")
(L"DXGI_FORMAT_G8R8_G8B8_UNORM", L"4 �����A32 �r�b�g�����Ȃ����K�������t�H�[�}�b�g 3")
(L"DXGI_FORMAT_BC1_TYPELESS", L"4 �����A�^�Ȃ��u���b�N���k�t�H�[�}�b�g")
(L"DXGI_FORMAT_BC1_UNORM", L"4 �����A�u���b�N���k�t�H�[�}�b�g")
(L"DXGI_FORMAT_BC1_UNORM_SRGB", L"sRGB data�p�� 4 �����A�u���b�N���k�t�H�[�}�b�g")
(L"DXGI_FORMAT_BC2_TYPELESS", L"4 �����A�^�Ȃ��u���b�N���k�t�H�[�}�b�g")
(L"DXGI_FORMAT_BC2_UNORM", L"4 �����A�u���b�N���k�t�H�[�}�b�g")
(L"DXGI_FORMAT_BC2_UNORM_SRGB", L"sRGB data�p�� 4 �����A�u���b�N���k�t�H�[�}�b�g")
(L"DXGI_FORMAT_BC3_TYPELESS", L"4 �����A�^�Ȃ��u���b�N���k�t�H�[�}�b�g")
(L"DXGI_FORMAT_BC3_UNORM", L"4 �����A�u���b�N���k�t�H�[�}�b�g")
(L"DXGI_FORMAT_BC3_UNORM_SRGB", L"sRGB data�p�� 4 �����A�u���b�N���k�t�H�[�}�b�g")
(L"DXGI_FORMAT_BC4_TYPELESS", L"1 �����A�^�Ȃ��u���b�N���k�t�H�[�}�b�g")
(L"DXGI_FORMAT_BC4_UNORM", L"1 �����A�u���b�N���k�t�H�[�}�b�g")
(L"DXGI_FORMAT_BC4_SNORM", L"1 �����A�u���b�N���k�t�H�[�}�b�g")
(L"DXGI_FORMAT_BC5_TYPELESS", L"2 �����A�^�Ȃ��u���b�N���k�t�H�[�}�b�g")
(L"DXGI_FORMAT_BC5_UNORM", L"2 �����A�u���b�N���k�t�H�[�}�b�g")
(L"DXGI_FORMAT_BC5_SNORM", L"2 �����A�u���b�N���k�t�H�[�}�b�g")
(L"DXGI_FORMAT_B5G6R5_UNORM", L"3 �����A16 �r�b�g�����Ȃ����K�������t�H�[�}�b�g")
(L"DXGI_FORMAT_B5G5R5A1_UNORM", L"1 �r�b�g �A���t�@���T�|�[�g���� 4 �����A16 �r�b�g�����Ȃ����K�������t�H�[�}�b�g")
(L"DXGI_FORMAT_B8G8R8A8_UNORM", L"8 �r�b�g �A���t�@���T�|�[�g���� 4 �����A16 �r�b�g�����Ȃ����K�������t�H�[�}�b�g")
(L"DXGI_FORMAT_B8G8R8X8_UNORM", L"4 �����A16 �r�b�g�����Ȃ����K�������t�H�[�}�b�g")
(L"DXGI_FORMAT_FORCE_UINT", L"�R���p�C�����ɁA���̗񋓌^�̃T�C�Y�� 32 �r�b�g�ɂ��邽�߂ɒ�`����Ă��܂��B����value���w�肵�Ȃ��ꍇ�A�ꕔ�̃R���p�C���ł͗񋓌^�� 32 �r�b�g�ȊO�̃T�C�Y�ŃR���p�C���\���̒萔���g�p����邱�Ƃ͂���܂���B");

// �X�L�������C�����

std::vector<mode_info> scanline_orders =
boost::assign::list_of<mode_info>
(L"DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED", L"�������̏������w�肳��Ă��܂���B")
(L"DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE", L"�C���[�W�͐擪�̑������`�Ō�̑���������쐬����A�X�L�b�v����鑖�����͂���܂���B")
(L"DXGI_MODE_SCANLINE_ORDER_UPPER_FIELD_FIRST", L"�C���[�W���㕔�̃t�B�[���h����쐬����܂��B")
(L"DXGI_MODE_SCANLINE_ORDER_LOWER_FIELD_FIRST", L"�C���[�W�������̃t�B�[���h����쐬����܂��B");

// �X�P�[�����O�p�����[�^
std::vector<mode_info> scalings = boost::assign::list_of<mode_info>
(L"DXGI_MODE_SCALING_UNSPECIFIED", L"�X�P�[�����O���w�肳��Ă��܂���B")
(L"DXGI_MODE_SCALING_CENTERED", L"�X�P�[�����O�Ȃ����w�肵�܂��B�C���[�W�̓f�B�X�v���C�̒����ɔz�u����܂��B�ʏ�A���̃t���O�͌Œ�h�b�g�s�b�` �f�B�X�v���C (LED �f�B�X�v���C�Ȃ�) �Ɏg�p���܂��B")
(L"DXGI_MODE_SCALING_STRETCHED", L"�g��X�P�[�����O���w�肵�܂��B");

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
  // DXGI Factory �� ����

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

  // Feature Level�z��̃Z�b�g�A�b�v
  std::vector<D3D_FEATURE_LEVEL> feature_levels = {
    D3D_FEATURE_LEVEL_11_1,
    D3D_FEATURE_LEVEL_11_0
  };

  //  boost::assign::list_of<D3D_FEATURE_LEVEL>
  //  (D3D_FEATURE_LEVEL_11_1)        // DirectX11.1�Ή�GPU
  //  (D3D_FEATURE_LEVEL_11_0);        // DirectX11.0�Ή�GPU
  ////        (D3D_FEATURE_LEVEL_10_0 );       // DirectX10�Ή�GPU

  // OS���ݒ肵��Feature Level���󂯎��ϐ�
  D3D_FEATURE_LEVEL level;

  // Direct3D �f�o�C�X&�R���e�L�X�g�쐬
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

  // DXGI�f�o�C�X�̎擾
  CHK(d3d_device_.As(&dxgi_device_));

  // DXGI�A�_�v�^�̎擾
  {
    IDXGIAdapterPtr adp;
    CHK(dxgi_device_->GetAdapter(&adp));
    CHK(adp.As(&dxgi_adapter_));
  }

  // DXGI Output�̎擾
  {
    IDXGIOutputPtr out;
    CHK(dxgi_adapter_->EnumOutputs(0, &out));
    CHK(out.As(&dxgi_output_));
  }

  // DXGI �t�@�N�g���[�̎擾
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

  // �X���b�v�`�F�C���̍쐬
  create_swap_chain(window_.is_fullscreen());

  CHK(dxgi_device_->SetMaximumFrameLatency(1));

  // Direct2D�f�o�C�X�̍쐬
  CHK(d2d_factory_->CreateDevice(dxgi_device_.Get(), &d2d_device_));

  // Direct2D�f�o�C�X�R���e�L�X�g�̍쐬
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
  // Direct3D���\�[�X�̐���
  ///////////////////////////////////////////////////////////////////

  {
    // �o�[�e�b�N�X�V�F�[�_�̃R���p�C��
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

    // �o�[�e�b�N�X�V�F�[�_�̐���
    CHK(d3d_device_->CreateVertexShader(vsblob->GetBufferPointer(), vsblob->GetBufferSize(), NULL, &v_shader_));

    // ���͒��_���C�A�E�g�̒�`
    D3D11_INPUT_ELEMENT_DESC
      layout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 } };
    ;

    // ���͒��_���C�A�E�g�̐���
    CHK(d3d_device_->CreateInputLayout(layout, ARRAYSIZE(layout), vsblob->GetBufferPointer(),
      vsblob->GetBufferSize(), &input_layout_));
    vsblob.Reset();
  }

  // ���̓��C�A�E�g�̐ݒ�
  d3d_context_->IASetInputLayout(input_layout_.Get());

  // �s�N�Z���E�V�F�[�_�[�̃R���p�C��
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

    // �s�N�Z���V�F�[�_�̍쐬
    CHK(d3d_device_->CreatePixelShader(psblob->GetBufferPointer(), psblob->GetBufferSize(), NULL, &p_shader_));

    psblob.Reset();
  }

  // �o�[�e�b�N�X�o�b�t�@�̍쐬
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

  // ���_�o�b�t�@�̃Z�b�g
  uint32_t stride = sizeof(simple_vertex);
  uint32_t offset = 0;
  d3d_context_->IASetVertexBuffers(0, 1, v_buffer_.GetAddressOf(), &stride, &offset);

  // �C���f�b�N�X�o�b�t�@�̐���
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

  // �C���f�b�N�X�o�b�t�@�̃Z�b�g
  d3d_context_->IASetIndexBuffer(i_buffer_.Get(), DXGI_FORMAT_R16_UINT, 0);

  // �v���~�e�B�u�̌`�Ԃ��w�肷��
  d3d_context_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

  // �萔�o�b�t�@�𐶐�����B
  bd.Usage = D3D11_USAGE_DEFAULT;
  bd.ByteWidth = sizeof(cb_never_changes);
  bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  bd.CPUAccessFlags = 0;
  CHK(d3d_device_->CreateBuffer(&bd, NULL, &cb_never_changes_));

  bd.ByteWidth = sizeof(cb_change_on_resize);
  CHK(d3d_device_->CreateBuffer(&bd, NULL, &cb_change_on_resize_));

  bd.ByteWidth = sizeof(cb_changes_every_frame);
  CHK(d3d_device_->CreateBuffer(&bd, NULL, &cb_changes_every_frame_));

  // �e�N�X�`���̃��[�h
  ID3D11ResourcePtr ptr;
  CHK(CreateDDSTextureFromFile(d3d_device_.Get(), L"SF.dds", &ptr, &shader_res_view_, NULL));

  // �T���v���X�e�[�g�̐���
  D3D11_SAMPLER_DESC sdesc = {};
  sdesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
  sdesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
  sdesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
  sdesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
  sdesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
  sdesc.MinLOD = 0;
  sdesc.MaxLOD = D3D11_FLOAT32_MAX;
  CHK(d3d_device_->CreateSamplerState(&sdesc, &sampler_state_));

  // ���[���h���W�ϊ��s��̃Z�b�g�A�b�v
  mat_world_ = XMMatrixIdentity();

  //g_vMeshColor( 0.7f, 0.7f, 0.7f, 1.0f );
  // 
  init_view_matrix();

  //init_ = true;// ����������

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
  // Text Format�̍쐬
  CHK(write_factory_->CreateTextFormat(
    L"���C���I",                // Font family name.
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

  // 2D Transform �̃Z�b�g�A�b�v
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
    // Opacity�̃A�j���[�V����
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

    // 3D�ϊ��̃A�j���[�V����

    v->SetEffect(effect.Get());
  }

  dcomp_device2_->Commit();

  d2d_context_->SetTarget(backup.Get());

}

void window_renderer::create_swap_chain(bool fullscreen)
{
  // �X���b�v�`�F�[���̍쐬

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
  // DXGI�T�[�t�F�[�X����Direct2D�`��p�r�b�g�}�b�v���쐬
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

  // Direct2D�`��^�[�Q�b�g�̐ݒ�
  d2d_context_->SetTarget(d2d1_target_bitmap_.Get());

}

// Direc2D�@�`��^�[�Q�b�g�̐ݒ�

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
//    // �A�_�v�^�f�o�C�X���̎擾
//    //LARGE_INTEGER version;
//    CHK(dxgi_factory_->EnumAdapters1(0,&dxgi_adapter_));
//    //CHK(dxgi_adapter_->CheckInterfaceSupport( __uuidof(ID3D10Device),&version));
//
//
//    // D3DDevice�̍쐬
//
//    std::vector<D3D_FEATURE_LEVEL> feature_levels = 
//      boost::assign::list_of<D3D_FEATURE_LEVEL>
//      (D3D_FEATURE_LEVEL_11_0 )        // DirectX11�Ή�GPU
//      (D3D_FEATURE_LEVEL_10_1)        // DirectX10.1�Ή�GPU
//      (D3D_FEATURE_LEVEL_10_0 );       // DirectX10�Ή�GPU
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
//    // �\�����[�h
//    DXGI_MODE_DESC desired_desc = {};// , actual_desc_ = {};
//    // �e�F8�r�b�g�ŕ������Ȃ����K����
//    desired_desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
//    desired_desc.Height = height_;// ����
//    desired_desc.Width = width_;// ��
//    desired_desc.Scaling = DXGI_MODE_SCALING_CENTERED;// �X�P�[�����O�Ȃ�
//    // ���t���b�V�����[�g��60Hz��v������
//
//    desired_desc.RefreshRate.Numerator = 60000;
//    desired_desc.RefreshRate.Denominator = 1000;
//    // �߂����[�h������
//    CHK(output_->FindClosestMatchingMode(&desired_desc,&actual_desc_,d3d_device_.Get()));
//
//    //// �X���b�v�`�F�[���̍쐬
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
//    // �o�b�N�o�b�t�@�̍쐬
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
//    // �X���b�v�`�F�[���ˑ����\�[�X�̍쐬
//    
//    create_swapchain_dependent_resources();
//
//    {
//      // �o�[�e�b�N�X�V�F�[�_�̃R���p�C��
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
//      // �o�[�e�b�N�X�V�F�[�_�̐���
//      CHK(d3d_device_->CreateVertexShader( vsblob->GetBufferPointer(), vsblob->GetBufferSize(), NULL, &v_shader_ ));
//
//      // ���͒��_���C�A�E�g�̒�`
//      D3D11_INPUT_ELEMENT_DESC
//        layout[] = {
//          { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//          { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//          { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }};
//          ;
//
//          // ���͒��_���C�A�E�g�̐���
//          CHK(d3d_device_->CreateInputLayout( layout, ARRAYSIZE(layout), vsblob->GetBufferPointer(),
//            vsblob->GetBufferSize(), &input_layout_ ));
//          vsblob.Reset();
//    }
//
//    // ���̓��C�A�E�g�̐ݒ�
//    d3d_context_->IASetInputLayout( input_layout_.Get() );
//
//    // �s�N�Z���E�V�F�[�_�[�̃R���p�C��
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
//      // �s�N�Z���V�F�[�_�̍쐬
//      CHK(d3d_device_->CreatePixelShader( psblob->GetBufferPointer(), psblob->GetBufferSize(), NULL, &p_shader_ ));
//
//      psblob.Reset();
//    }
//
//    // �o�[�e�b�N�X�o�b�t�@�̍쐬
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
//    // ���_�o�b�t�@�̃Z�b�g
//    uint32_t stride = sizeof( simple_vertex );
//    uint32_t offset = 0;
//    d3d_context_->IASetVertexBuffers( 0, 1, v_buffer_.GetAddressOf(), &stride, &offset );
//
//    // �C���f�b�N�X�o�b�t�@�̐���
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
//    // �C���f�b�N�X�o�b�t�@�̃Z�b�g
//    d3d_context_->IASetIndexBuffer( i_buffer_.Get(), DXGI_FORMAT_R16_UINT, 0 );
//
//    // �v���~�e�B�u�̌`�Ԃ��w�肷��
//    d3d_context_->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
//
//    // �萔�o�b�t�@�𐶐�����B
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
//    // �e�N�X�`���̃��[�h
//	ID3D11ResourcePtr ptr;
//    CHK(CreateDDSTextureFromFile( d3d_device_.Get(), L"SF.dds", &ptr, &shader_res_view_, NULL ));
////    CHK(CreateDDSTextureFromFile( d3d_device_, L"SF.dds", NULL, NULL, &shader_res_view_, NULL ));
//
//    // �T���v���X�e�[�g�̐���
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
//    // ���[���h���W�ϊ��s��̃Z�b�g�A�b�v
//    mat_world_ = XMMatrixIdentity();
//
//    //g_vMeshColor( 0.7f, 0.7f, 0.7f, 1.0f );
//    // 
//    init_view_matrix();
//
//
//
//    // ���I�e�N�X�`���̐���
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
//      //// �[�x�o�b�t�@�̍쐬
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
//    init_ = true;// ����������
//  }



void  window_renderer::init_view_matrix()
{
  // �r���[�s��̃Z�b�g�A�b�v
  ////��{value�ݒ�
  //float aspect = (float) width_ / height_;	    //�A�X�y�N�g��(������1�Ƃ����Ƃ��̕�)
  //float depth = 1.0f;										//���s��Z
  //float fovy  = (float)atan(1.0f / depth) * 2.0f;					//�����Z=0�Ńf�o�C�X�̕��ƍ����ɍ��킹��

  //XMVECTOR eye = { 0.0f, 0.0f, -depth, 0.0f };
  //XMVECTOR at = { 0.0f, 0.0f, 0.0f, 0.0f};
  //XMVECTOR up = { 0.0f, 1.0f, 0.0f, 0.0f };
  //mat_view_ = XMMatrixLookAtLH( eye, at, up );
  //cb_never_changes cnc;
  //cnc.mView = XMMatrixTranspose( mat_view_ );
  ////cnc.vLightColor = XMFLOAT4( 1.0f, 0.5f, 0.5f, 1.0f );
  //cnc.vLightDir =  XMFLOAT4(0.577f, 0.577f, -0.977f, 1.0f);
  //// �萔�o�b�t�@�Ɋi�[
  //d3d_context_->UpdateSubresource( cb_never_changes_.Get(), 0, NULL, &cnc, 0, 0 );

  //// ���e�s��̃Z�b�g�A�b�v

  ////mat_projection_ = XMMatrixPerspectiveFovLH( XM_PIDIV4, /*(rc.right - rc.left)/(rc.bottom - rc.top)*/width_ / height_, 0.01f, 100.0f );
  ////mat_projection_ = XMMatrixPerspectiveFovLH( fovy, aspect, 0.01f, 100.0f );
  //mat_projection_ = XMMatrixPerspectiveFovLH( fovy, 1.0, 0.0001f, 100.0f );
  //cb_change_on_resize ccor;
  //ccor.mProjection = XMMatrixTranspose( mat_projection_ );
  //// �萔�o�b�t�@�Ɋi�[
  //d3d_context_->UpdateSubresource( cb_change_on_resize_.Get(), 0, NULL, &ccor, 0, 0 );
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
  d3d_context_->UpdateSubresource(cb_never_changes_.Get(), 0, NULL, &cnc, 0, 0);

  // ���e�s��̃Z�b�g�A�b�v
  mat_projection_ = XMMatrixPerspectiveFovLH(XM_PIDIV4, window_.width() / window_.height(), 0.01f, 100.0f);
  cb_change_on_resize ccor;
  ccor.mProjection = XMMatrixTranspose(mat_projection_);
  // �萔�o�b�t�@�Ɋi�[
  d3d_context_->UpdateSubresource(cb_change_on_resize_.Get(), 0, NULL, &ccor, 0, 0);


}


void  window_renderer::create_swapchain_dependent_resources()
{

  create_d2d_render_target();

  // Direct 3D���\�[�X�̍쐬 /////////////////////////////////////////

  CHK(dxgi_swap_chain_->GetBuffer(0, IID_PPV_ARGS(&back_buffer_)));

  // �r���[�̍쐬
  CHK(d3d_device_->CreateRenderTargetView(back_buffer_.Get(), 0, &d3d_render_target_view_));
  D3D11_TEXTURE2D_DESC desc = {};
  back_buffer_->GetDesc(&desc);

  // �[�x�o�b�t�@�̍쐬
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

  // OM�X�e�[�W�ɓo�^����
  d3d_context_->OMSetRenderTargets(1, d3d_render_target_view_.GetAddressOf(), depth_view_.Get());

  // �r���[�|�[�g�̐ݒ�
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

// �X���b�v�`�F�C�������X�g�A����

void  window_renderer::restore_swapchain_and_dependent_resources()
{
  // �t���X�N���[�����炢������E�B���h�E���[�h�ɖ߂�
  if (window_.is_fullscreen())
  {
    CHK(dxgi_swap_chain_->GetFullscreenState(FALSE, nullptr));
  }

  // �X���b�v�`�F�C���Ɉˑ����Ă��郊�\�[�X���������
  discard_swapchain_dependent_resources();

  CHK(dxgi_swap_chain_->ResizeBuffers(2, swap_chain_desc_.Width, swap_chain_desc_.Height, swap_chain_desc_.Format, swap_chain_desc_.Flags));

  // �t���X�N���[���̏ꍇ�A���Ƃɖ߂��B
  if (window_.is_fullscreen()){
    BOOL f = window_.is_fullscreen() ? TRUE : FALSE;
    CHK(dxgi_swap_chain_->GetFullscreenState(&f, nullptr));
  }

  // �X���b�v�`�F�C���Ɉˑ����Ă��郊�\�[�X���č쐬����
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

    // �`��^�[�Q�b�g�̃N���A
    d3d_context_->ClearRenderTargetView(d3d_render_target_view_.Get(), color);
    // �[�x�o�b�t�@�̃N���A
    d3d_context_->ClearDepthStencilView(depth_view_.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

    d2d_context_->BeginDraw();
    //d2d_context_->Clear();


    //thunk_proc_ = (WNDPROC)thunk_.getCode();
    D2D_RECT_F layout_rect_ = D2D1::RectF(0.0f, 100.0f, 400.0f, 100.0f);
    // Text Format�̍쐬
    //CHK(write_factory_->CreateTextFormat(
    //  L"�l�r �S�V�b�N",                // Font family name.
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
    //std::wstring m((boost::wformat(L"�`�`�a�a�b�b�c�c�d�d�e�eTEST�\��%08d�@ ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789�A�C�E�G�I����������") % count).str());
    //d2d_context_->DrawTextW(
    //  m.c_str(),
    //  m.size(),
    //  write_text_format_.Get(),
    //  layout_rect_,
    //  brush.Get());

    d2d_context_->EndDraw();


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
    d3d_context_->UpdateSubresource(cb_changes_every_frame_.Get(), 0, NULL, &cb, 0, 0);
    d3d_context_->OMSetRenderTargets(1, d3d_render_target_view_.GetAddressOf(), depth_view_.Get());

    // �l�p�`
    d3d_context_->VSSetShader(v_shader_.Get(), NULL, 0);
    d3d_context_->VSSetConstantBuffers(0, 1, cb_never_changes_.GetAddressOf());
    d3d_context_->VSSetConstantBuffers(1, 1, cb_change_on_resize_.GetAddressOf());
    d3d_context_->VSSetConstantBuffers(2, 1, cb_changes_every_frame_.GetAddressOf());
    d3d_context_->PSSetShader(p_shader_.Get(), NULL, 0);
    d3d_context_->PSSetConstantBuffers(2, 1, cb_changes_every_frame_.GetAddressOf());
    d3d_context_->PSSetShaderResources(0, 1, shader_res_view_.GetAddressOf());
    d3d_context_->PSSetSamplers(0, 1, sampler_state_.GetAddressOf());

    d3d_context_->DrawIndexed(36, 0, 0);


    // �t���b�v

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

  // �`��^�[�Q�b�g�̃N���A
  d3d_context_->ClearRenderTargetView(d3d_render_target_view_.Get(),color);
  // �[�x�o�b�t�@�̃N���A
  d3d_context_->ClearDepthStencilView(depth_view_.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0 );

  // �F�̕ύX
  mesh_color_.x = 1.0f;
  mesh_color_.y = 1.0f;
  mesh_color_.z = 1.0f;

  // �萔�X�V

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

  // �l�p�`
  d3d_context_->VSSetShader( v_shader_.Get(), NULL, 0 );
  d3d_context_->VSSetConstantBuffers( 0, 1, cb_never_changes_.GetAddressOf() );
  d3d_context_->VSSetConstantBuffers( 1, 1, cb_change_on_resize_.GetAddressOf() );
  d3d_context_->VSSetConstantBuffers( 2, 1, cb_changes_every_frame_.GetAddressOf() );
  d3d_context_->PSSetShader( p_shader_.Get(), NULL, 0 );
  d3d_context_->PSSetConstantBuffers( 2, 1, cb_changes_every_frame_.GetAddressOf() );
  d3d_context_->PSSetShaderResources( 0, 1, shader_res_view_.GetAddressOf() );
  d3d_context_->PSSetSamplers( 0, 1, sampler_state_.GetAddressOf() );

  d3d_context_->DrawIndexed( 6, 0, 0 );

  // ��ʂɓ]��
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
  //    blend.SourceConstantAlpha = 128; // �s�����x�i���C���[�h�E�B���h�E�S�̂̃A���t�@value�j
  //    blend.AlphaFormat = AC_SRC_ALPHA;
  // �f�o�C�X�R���e�L�X�g�ɂ����郌�C���̈ʒu
  POINT po;
  po.x = po.y = 0;
  BOOL err;
  err = BitBlt(ddc.get(),0,0,width_,height_,sdc,0,0,SRCCOPY);
  //      err = AlphaBlend(ddc.get(),0,0,width_,height_,sdc,0,0,width_,height_,blend);
  //err = UpdateLayeredWindow(hwnd_, ddc.get(), &wnd_pos, &wnd_size, sdc, &po, RGB(255,0,0), &blend, ULW_ALPHA | ULW_COLORKEY );
  BOOST_ASSERT(err == TRUE);
  surface->ReleaseDC( NULL);
  surface.Reset();
  // OM�X�e�[�W�ɓo�^����
  d3d_context_->OMSetRenderTargets( 1, d3d_render_target_view_.GetAddressOf(), depth_view_.Get() );
  }
  */
}


