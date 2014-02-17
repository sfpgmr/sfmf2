// Copyright (c) 2011 NVIDIA Corporation. All rights reserved.
//
// TO  THE MAXIMUM  EXTENT PERMITTED  BY APPLICABLE  LAW, THIS SOFTWARE  IS PROVIDED
// *AS IS*  AND NVIDIA AND  ITS SUPPLIERS DISCLAIM  ALL WARRANTIES,  EITHER  EXPRESS
// OR IMPLIED, INCLUDING, BUT NOT LIMITED  TO, NONINFRINGEMENT,IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL  NVIDIA 
// OR ITS SUPPLIERS BE  LIABLE  FOR  ANY  DIRECT, SPECIAL,  INCIDENTAL,  INDIRECT,  OR  
// CONSEQUENTIAL DAMAGES WHATSOEVER (INCLUDING, WITHOUT LIMITATION,  DAMAGES FOR LOSS 
// OF BUSINESS PROFITS, BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR ANY 
// OTHER PECUNIARY LOSS) ARISING OUT OF THE  USE OF OR INABILITY  TO USE THIS SOFTWARE, 
// EVEN IF NVIDIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
//
// Please direct any bugs or questions to SDKFeedback@nvidia.com

#include "stdafx.h"

//#include "DXUT.h"
//#include "SDKmisc.h"

#include "ocean_simulator.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;
// Disable warning "conditional expression is constant"
#pragma warning(disable:4127)


#define HALF_SQRT_2	0.7071068f
#define GRAV_ACCEL	981.0f	// The acceleration of gravity, cm/s^2

#define BLOCK_SIZE_X 16
#define BLOCK_SIZE_Y 16

// HRESULT CompileShaderFromFile( WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut );

// Generating gaussian random number with mean 0 and standard deviation 1.
namespace {
  float Gauss()
  {
    float u1 = rand() / (float) RAND_MAX;
    float u2 = rand() / (float) RAND_MAX;
    if (u1 < 1e-6f)
      u1 = 1e-6f;
    return sqrtf(-2.0f * logf(u1)) * cosf(2.0f * XM_PI * u2);
  }

  // Phillips Spectrum
  // K: normalized wave vector, W: wind direction, v: wind velocity, a: amplitude constant
  float Phillips(Vector2 K, Vector2 W, float v, float a, float dir_depend)
  {
    // largest possible wave from constant wind of velocity v
    float l = v * v / GRAV_ACCEL;
    // damp out waves with very small length w << l
    float w = l / 1000;

    float Ksqr = K.x * K.x + K.y * K.y;
    float Kcos = K.x * W.x + K.y * W.y;
    float phillips = a * expf(-1 / (l * l * Ksqr)) / (Ksqr * Ksqr * Ksqr) * (Kcos * Kcos);

    // filter out waves moving opposite to wind
    if (Kcos < 0)
      phillips *= dir_depend;

    // damp out waves with very small length w << l
    return phillips * expf(-Ksqr * w * w);
  }

  void createBufferAndUAV(ID3D11Device2Ptr& pd3dDevice, void* data, UINT byte_width, UINT byte_stride,
    ID3D11BufferPtr& ppBuffer, ID3D11UnorderedAccessViewPtr& ppUAV, ID3D11ShaderResourceViewPtr& ppSRV)
  {
    // Create buffer
    D3D11_BUFFER_DESC buf_desc;
    buf_desc.ByteWidth = byte_width;
    buf_desc.Usage = D3D11_USAGE_DEFAULT;
    buf_desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
    buf_desc.CPUAccessFlags = 0;
    buf_desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    buf_desc.StructureByteStride = byte_stride;

    D3D11_SUBRESOURCE_DATA init_data = { data, 0, 0 };

    CHK(pd3dDevice->CreateBuffer(&buf_desc, data != NULL ? &init_data : NULL, &ppBuffer));

    // Create undordered access view
    D3D11_UNORDERED_ACCESS_VIEW_DESC uav_desc;
    uav_desc.Format = DXGI_FORMAT_UNKNOWN;
    uav_desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    uav_desc.Buffer.FirstElement = 0;
    uav_desc.Buffer.NumElements = byte_width / byte_stride;
    uav_desc.Buffer.Flags = 0;

    CHK(pd3dDevice->CreateUnorderedAccessView(ppBuffer.Get(), &uav_desc, &ppUAV));

    // Create shader resource view
    D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
    srv_desc.Format = DXGI_FORMAT_UNKNOWN;
    srv_desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    srv_desc.Buffer.FirstElement = 0;
    srv_desc.Buffer.NumElements = byte_width / byte_stride;

    CHK(pd3dDevice->CreateShaderResourceView(ppBuffer.Get(), &srv_desc, &ppSRV));
  }

  void createTextureAndViews(ID3D11Device2Ptr& pd3dDevice, UINT width, UINT height, DXGI_FORMAT format,
    ID3D11Texture2DPtr& pTex, ID3D11ShaderResourceViewPtr& ppSRV, ID3D11RenderTargetViewPtr& ppRTV)
  {
    // Create 2D texture
    D3D11_TEXTURE2D_DESC tex_desc;
    tex_desc.Width = width;
    tex_desc.Height = height;
    tex_desc.MipLevels = 0;
    tex_desc.ArraySize = 1;
    tex_desc.Format = format;
    tex_desc.SampleDesc.Count = 1;
    tex_desc.SampleDesc.Quality = 0;
    tex_desc.Usage = D3D11_USAGE_DEFAULT;
    tex_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    tex_desc.CPUAccessFlags = 0;
    tex_desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

    CHK(pd3dDevice->CreateTexture2D(&tex_desc, NULL, &pTex));

    // Create shader resource view
    pTex->GetDesc(&tex_desc);
      D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
      srv_desc.Format = format;
      srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
      srv_desc.Texture2D.MipLevels = tex_desc.MipLevels;
      srv_desc.Texture2D.MostDetailedMip = 0;

      CHK(pd3dDevice->CreateShaderResourceView(pTex.Get(), &srv_desc, &ppSRV));

    // Create render target view
      D3D11_RENDER_TARGET_VIEW_DESC rtv_desc;
      rtv_desc.Format = format;
      rtv_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
      rtv_desc.Texture2D.MipSlice = 0;

      CHK(pd3dDevice->CreateRenderTargetView(pTex.Get(), &rtv_desc, &ppRTV));
  }
}

OceanSimulator::OceanSimulator(OceanParameter& params, ID3D11Device2Ptr& pd3dDevice,ID3D11DeviceContext2Ptr& context)
: m_pd3dDevice(pd3dDevice),  m_fft_plan(pd3dDevice,context,3)
{

	// Height map H(0)
  int height_map_size = (params.dmap_dim + 4) * (params.dmap_dim + 1);
  int hmap_dim = params.dmap_dim;
  int input_full_size = (hmap_dim + 4) * (hmap_dim + 1);
  // This value should be (hmap_dim / 2 + 1) * hmap_dim, but we use full sized buffer here for simplicity.
  int input_half_size = hmap_dim * hmap_dim;
  int output_size = hmap_dim * hmap_dim;

  {

    std::unique_ptr<Vector2[]> h0_data(new Vector2[height_map_size * sizeof(Vector2)]);
    std::unique_ptr<float[]> omega_data(new float[height_map_size * sizeof(float)]);
    initHeightMap(params, h0_data, omega_data);

    m_param = params;


    // For filling the buffer with zeroes.

    std::unique_ptr<char []> zero_data(new char[3 * output_size * sizeof(float) * 2]);
    memset(zero_data.get(), 0, 3 * output_size * sizeof(float) * 2);

    // RW buffer allocations
    // H0
    UINT float2_stride = 2 * sizeof(float);
    createBufferAndUAV(m_pd3dDevice, h0_data.get(), input_full_size * float2_stride, float2_stride, m_pBuffer_Float2_H0, m_pUAV_H0, m_pSRV_H0);

    // Notice: The following 3 buffers should be half sized buffer because of conjugate symmetric input. But
    // we use full sized buffers due to the CS4.0 restriction.

    // Put H(t), Dx(t) and Dy(t) into one buffer because CS4.0 allows only 1 UAV at a time
    createBufferAndUAV(m_pd3dDevice, zero_data.get(), 3 * input_half_size * float2_stride, float2_stride, m_pBuffer_Float2_Ht, m_pUAV_Ht, m_pSRV_Ht);

    // omega
    createBufferAndUAV(m_pd3dDevice, omega_data.get(), input_full_size * sizeof(float), sizeof(float), m_pBuffer_Float_Omega, m_pUAV_Omega, m_pSRV_Omega);

    // Notice: The following 3 should be real number data. But here we use the complex numbers and C2C FFT
    // due to the CS4.0 restriction.
    // Put Dz, Dx and Dy into one buffer because CS4.0 allows only 1 UAV at a time
    createBufferAndUAV(m_pd3dDevice, zero_data.get(), 3 * output_size * float2_stride, float2_stride, m_pBuffer_Float_Dxyz, m_pUAV_Dxyz, m_pSRV_Dxyz);

  }

	// D3D11 Textures
	::createTextureAndViews(m_pd3dDevice, hmap_dim, hmap_dim, DXGI_FORMAT_R32G32B32A32_FLOAT, m_pDisplacementMap, m_pDisplacementSRV, m_pDisplacementRTV);
	::createTextureAndViews(m_pd3dDevice, hmap_dim, hmap_dim, DXGI_FORMAT_R16G16B16A16_FLOAT, m_pGradientMap, m_pGradientSRV, m_pGradientRTV);

	// Samplers
	D3D11_SAMPLER_DESC sam_desc;
	sam_desc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
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
	sam_desc.MinLOD = -FLT_MAX;
	sam_desc.MaxLOD = FLT_MAX;

  CHK(m_pd3dDevice->CreateSamplerState(&sam_desc, &m_pPointSamplerState));

  // Compute shaders
  std::wstring shader_dir(sf::app_base_directory::instance()->shader_dir());
  sf::create_shader_from_file(m_pd3dDevice, shader_dir + L"ocean_simulator_cs.hlsl", "UpdateSpectrumCS", "cs_5_0", m_pUpdateSpectrumCS);

  {	// Vertex & pixel shaders
    ID3DBlobPtr pBlobQuadVS;
    sf::create_shader_blob_from_file(shader_dir + L"ocean_simulator_vs_ps.hlsl", "QuadVS", "vs_5_0", pBlobQuadVS);
    sf::create_shader(m_pd3dDevice, pBlobQuadVS->GetBufferPointer(), pBlobQuadVS->GetBufferSize(), nullptr, m_pQuadVS);
    //sf::create_shader_from_file(m_pd3dDevice, shader_dir + L"ocean_simulator_vs_ps.hlsl", "QuadVS", "vs_5_0", m_pQuadVS);
    sf::create_shader_from_file(m_pd3dDevice, shader_dir + L"ocean_simulator_vs_ps.hlsl", "UpdateDisplacementPS", "ps_5_0", m_pUpdateDisplacementPS);
    sf::create_shader_from_file(m_pd3dDevice, shader_dir + L"ocean_simulator_vs_ps.hlsl", "GenGradientFoldingPS", "ps_5_0", m_pGenGradientFoldingPS);

    // Input layout
    D3D11_INPUT_ELEMENT_DESC quad_layout_desc [] =
    {
      { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    CHK(m_pd3dDevice->CreateInputLayout(quad_layout_desc, 1, pBlobQuadVS->GetBufferPointer(), pBlobQuadVS->GetBufferSize(), &m_pQuadLayout));
  }

	// Quad vertex buffer
	D3D11_BUFFER_DESC vb_desc;
	vb_desc.ByteWidth = 4 * sizeof(XMFLOAT4);
	vb_desc.Usage = D3D11_USAGE_IMMUTABLE;
	vb_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vb_desc.CPUAccessFlags = 0;
	vb_desc.MiscFlags = 0;

	float quad_verts[] =
	{
		-1, -1, 0, 1,
		-1,  1, 0, 1,
		 1, -1, 0, 1,
		 1,  1, 0, 1,
	};
	D3D11_SUBRESOURCE_DATA init_data;
	init_data.pSysMem = &quad_verts[0];
	init_data.SysMemPitch = 0;
	init_data.SysMemSlicePitch = 0;
	
	CHK(m_pd3dDevice->CreateBuffer(&vb_desc, &init_data, &m_pQuadVB));

	// Constant buffers
	UINT actual_dim = m_param.dmap_dim;
	UINT input_width = actual_dim + 4;
	// We use full sized data here. The value "output_width" should be actual_dim/2+1 though.
	UINT output_width = actual_dim;
	UINT output_height = actual_dim;
	UINT dtx_offset = actual_dim * actual_dim;
	UINT dty_offset = actual_dim * actual_dim * 2;
	UINT immutable_consts[] = {actual_dim, input_width, output_width, output_height, dtx_offset, dty_offset};
	D3D11_SUBRESOURCE_DATA init_cb0 = {&immutable_consts[0], 0, 0};

	D3D11_BUFFER_DESC cb_desc;
	cb_desc.Usage = D3D11_USAGE_IMMUTABLE;
	cb_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb_desc.CPUAccessFlags = 0;
	cb_desc.MiscFlags = 0;    
	cb_desc.ByteWidth = PAD16(sizeof(immutable_consts));
	CHK(m_pd3dDevice->CreateBuffer(&cb_desc, &init_cb0, &m_pImmutableCB));

	ID3D11Buffer* cbs[1] = {m_pImmutableCB.Get()};
	context->CSSetConstantBuffers(0, 1, cbs);
  context->PSSetConstantBuffers(0, 1, cbs);

	cb_desc.Usage = D3D11_USAGE_DYNAMIC;
	cb_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb_desc.MiscFlags = 0;    
	cb_desc.ByteWidth = PAD16(sizeof(float) * 3);
	CHK(m_pd3dDevice->CreateBuffer(&cb_desc, NULL, &m_pPerFrameCB));

	// FFT

#ifdef CS_DEBUG_BUFFER
	D3D11_BUFFER_DESC buf_desc;
	buf_desc.ByteWidth = 3 * input_half_size * float2_stride;
    buf_desc.Usage = D3D11_USAGE_STAGING;
    buf_desc.BindFlags = 0;
    buf_desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    buf_desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    buf_desc.StructureByteStride = float2_stride;

	m_pd3dDevice->CreateBuffer(&buf_desc, NULL, &m_pDebugBuffer);
	assert(m_pDebugBuffer);
#endif

  CheckObjects();
}

void OceanSimulator::CheckObjects()
{
  // オブジェクトのチェック

  assert(m_pBuffer_Float2_H0);
  assert(m_pBuffer_Float_Omega);
  assert(m_pBuffer_Float2_Ht);
  assert(m_pBuffer_Float_Dxyz);

  assert(m_pPointSamplerState);

  assert(m_pQuadVB);

  assert(m_pUAV_H0);
  assert(m_pUAV_Omega);
  assert(m_pUAV_Ht);
  assert(m_pUAV_Dxyz);

  assert(m_pSRV_H0);
  assert(m_pSRV_Omega);
  assert(m_pSRV_Ht);
  assert(m_pSRV_Dxyz);

  assert(m_pDisplacementMap);
  assert(m_pDisplacementSRV);
  assert(m_pDisplacementRTV);

  assert(m_pGradientMap);
  assert(m_pGradientSRV);
  assert(m_pGradientRTV);

  assert(m_pUpdateSpectrumCS);
  assert(m_pQuadVS);
  assert(m_pUpdateDisplacementPS);
  assert(m_pGenGradientFoldingPS);

  assert(m_pQuadLayout);

  assert(m_pImmutableCB);
  assert(m_pPerFrameCB);

}

OceanSimulator::~OceanSimulator()
{

	m_pBuffer_Float2_H0.Reset();
	m_pBuffer_Float_Omega.Reset();
	m_pBuffer_Float2_Ht.Reset();
	m_pBuffer_Float_Dxyz.Reset();

	m_pPointSamplerState.Reset();

	m_pQuadVB.Reset();

	m_pUAV_H0.Reset();
	m_pUAV_Omega.Reset();
	m_pUAV_Ht.Reset();
	m_pUAV_Dxyz.Reset();

	m_pSRV_H0.Reset();
	m_pSRV_Omega.Reset();
	m_pSRV_Ht.Reset();
	m_pSRV_Dxyz.Reset();

	m_pDisplacementMap.Reset();
	m_pDisplacementSRV.Reset();
	m_pDisplacementRTV.Reset();

	m_pGradientMap.Reset();
	m_pGradientSRV.Reset();
	m_pGradientRTV.Reset();

	m_pUpdateSpectrumCS.Reset();
	m_pQuadVS.Reset();
	m_pUpdateDisplacementPS.Reset();
	m_pGenGradientFoldingPS.Reset();

	m_pQuadLayout.Reset();

	m_pImmutableCB.Reset();
	m_pPerFrameCB.Reset();

//	m_pd3dImmediateContext.Reset();

#ifdef CS_DEBUG_BUFFER
	m_pDebugBuffer.Reset();
#endif
}


// Initialize the vector field.
// wlen_x: width of wave tile, in meters
// wlen_y: length of wave tile, in meters
void OceanSimulator::initHeightMap(OceanParameter& params, std::unique_ptr<Vector2[]>& out_h0, std::unique_ptr<float[]>& out_omega)
{
	int i, j;
	Vector2 K, Kn;

  Vector2 wind_dir;

  params.wind_dir.Normalize(wind_dir);
  
	float a = params.wave_amplitude * 1e-7f;	// It is too small. We must scale it for editing.
	float v = params.wind_speed;
	float dir_depend = params.wind_dependency;

	int height_map_dim = params.dmap_dim;
	float patch_length = params.patch_length;

	// initialize random generator.
	srand(0);

	for (i = 0; i <= height_map_dim; i++)
	{
		// K is wave-vector, range [-|DX/W, |DX/W], [-|DY/H, |DY/H]
		K.y = (-height_map_dim / 2.0f + i) * (2 * XM_PI / patch_length);

		for (j = 0; j <= height_map_dim; j++)
		{
			K.x = (-height_map_dim / 2.0f + j) * (2 * XM_PI / patch_length);

			float phil = (K.x == 0 && K.y == 0) ? 0 : sqrtf(Phillips(K, wind_dir, v, a, dir_depend));

			out_h0[i * (height_map_dim + 4) + j].x = float(phil * Gauss() * HALF_SQRT_2);
			out_h0[i * (height_map_dim + 4) + j].y = float(phil * Gauss() * HALF_SQRT_2);

			// The angular frequency is following the dispersion relation:
			//            out_omega^2 = g*k
			// The equation of Gerstner wave:
			//            x = x0 - K/k * A * sin(dot(K, x0) - sqrt(g * k) * t), x is a 2D vector.
			//            z = A * cos(dot(K, x0) - sqrt(g * k) * t)
			// Gerstner wave shows that a point on a simple sinusoid wave is doing a uniform circular
			// motion with the center (x0, y0, z0), radius A, and the circular plane is parallel to
			// vector K.
			out_omega[i * (height_map_dim + 4) + j] = sqrtf(GRAV_ACCEL * sqrtf(K.x * K.x + K.y * K.y));
		}
	}
}

void OceanSimulator::updateDisplacementMap(float time,ID3D11DeviceContext2Ptr& d3d_context)
{
	// ---------------------------- H(0) -> H(t), D(x, t), D(y, t) --------------------------------
	// Compute shader
	d3d_context->CSSetShader(m_pUpdateSpectrumCS.Get(), NULL, 0);

	// Buffers
	ID3D11ShaderResourceView* cs0_srvs[2] = {m_pSRV_H0.Get(), m_pSRV_Omega.Get()};
	d3d_context->CSSetShaderResources(0, 2, cs0_srvs);

	ID3D11UnorderedAccessView* cs0_uavs[1] = {m_pUAV_Ht.Get()};
	d3d_context->CSSetUnorderedAccessViews(0, 1, cs0_uavs, (UINT*)(&cs0_uavs[0]));

	// Consts
	D3D11_MAPPED_SUBRESOURCE mapped_res;            
	d3d_context->Map(m_pPerFrameCB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_res);
	assert(mapped_res.pData);
	float* per_frame_data = (float*)mapped_res.pData;
	// g_Time
	per_frame_data[0] = time * m_param.time_scale;
	// g_ChoppyScale
	per_frame_data[1] = m_param.choppy_scale;
	// g_GridLen
	per_frame_data[2] = m_param.dmap_dim / m_param.patch_length;
	d3d_context->Unmap(m_pPerFrameCB.Get(), 0);

	ID3D11Buffer* cs_cbs[2] = {m_pImmutableCB.Get(), m_pPerFrameCB.Get()};
	d3d_context->CSSetConstantBuffers(0, 2, cs_cbs);

	// Run the CS
	UINT group_count_x = (m_param.dmap_dim + BLOCK_SIZE_X - 1) / BLOCK_SIZE_X;
	UINT group_count_y = (m_param.dmap_dim + BLOCK_SIZE_Y - 1) / BLOCK_SIZE_Y;
	d3d_context->Dispatch(group_count_x, group_count_y, 1);

	// Unbind resources for CS
	cs0_uavs[0] = NULL;
	d3d_context->CSSetUnorderedAccessViews(0, 1, cs0_uavs, (UINT*)(&cs0_uavs[0]));
	cs0_srvs[0] = NULL;
	cs0_srvs[1] = NULL;
	d3d_context->CSSetShaderResources(0, 2, cs0_srvs);


	// ------------------------------------ Perform FFT -------------------------------------------
  m_fft_plan.fft_512x512_c2c(d3d_context,m_pUAV_Dxyz, m_pSRV_Dxyz, m_pSRV_Ht);

  // --------------------------------- Wrap Dx, Dy and Dz ---------------------------------------
  // Push RT
  {
    ID3D11RenderTargetView* old_target;
    ID3D11DepthStencilView* old_depth;
    d3d_context->OMGetRenderTargets(1, &old_target, &old_depth);
    D3D11_VIEWPORT old_viewport;
    UINT num_viewport = 1;
    d3d_context->RSGetViewports(&num_viewport, &old_viewport);

    D3D11_VIEWPORT new_vp = { 0, 0, (float) m_param.dmap_dim, (float) m_param.dmap_dim, 0.0f, 1.0f };
    d3d_context->RSSetViewports(1, &new_vp);

    // Set RT
    ID3D11RenderTargetView* rt_views[1] = { m_pDisplacementRTV.Get() };
    d3d_context->OMSetRenderTargets(1, rt_views, NULL);

    // VS & PS
    d3d_context->VSSetShader(m_pQuadVS.Get(), NULL, 0);
    d3d_context->PSSetShader(m_pUpdateDisplacementPS.Get(), NULL, 0);

    // Constants
    ID3D11Buffer* ps_cbs[2] = { m_pImmutableCB.Get(), m_pPerFrameCB.Get() };
    d3d_context->PSSetConstantBuffers(0, 2, ps_cbs);

    // Buffer resources
    ID3D11ShaderResourceView* ps_srvs[1] = { m_pSRV_Dxyz.Get() };
    d3d_context->PSSetShaderResources(0, 1, ps_srvs);

    // IA setup
    ID3D11Buffer* vbs[1] = { m_pQuadVB.Get() };
    UINT strides[1] = { sizeof(XMFLOAT4) };
    UINT offsets[1] = { 0 };
    d3d_context->IASetVertexBuffers(0, 1, &vbs[0], &strides[0], &offsets[0]);

    d3d_context->IASetInputLayout(m_pQuadLayout.Get());
    d3d_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    // Perform draw call
    d3d_context->Draw(4, 0);

    // Unbind
    ps_srvs[0] = NULL;
    d3d_context->PSSetShaderResources(0, 1, ps_srvs);


    // ----------------------------------- Generate Normal ----------------------------------------
    // Set RT
    rt_views[0] = m_pGradientRTV.Get();
    d3d_context->OMSetRenderTargets(1, rt_views, NULL);

    // VS & PS
    d3d_context->VSSetShader(m_pQuadVS.Get(), NULL, 0);
    d3d_context->PSSetShader(m_pGenGradientFoldingPS.Get(), NULL, 0);

    // Texture resource and sampler
    ps_srvs[0] = m_pDisplacementSRV.Get();
    d3d_context->PSSetShaderResources(0, 1, ps_srvs);

    ID3D11SamplerState* samplers[1] = { m_pPointSamplerState.Get() };
    d3d_context->PSSetSamplers(0, 1, &samplers[0]);

    // Perform draw call
    d3d_context->Draw(4, 0);

    // Unbind
    ps_srvs[0] = NULL;
    d3d_context->PSSetShaderResources(0, 1, ps_srvs);

    // Pop RT
    d3d_context->RSSetViewports(1, &old_viewport);
    d3d_context->OMSetRenderTargets(1, &old_target, old_depth);
    old_target->Release();
    old_depth->Release();
  }

	d3d_context->GenerateMips(m_pGradientSRV.Get());

	// Define CS_DEBUG_BUFFER to enable writing a buffer into a file.
#ifdef CS_DEBUG_BUFFER
    {
		d3d_context->CopyResource(m_pDebugBuffer, m_pBuffer_Float_Dxyz);
        D3D11_MAPPED_SUBRESOURCE mapped_res;
        d3d_context->Map(m_pDebugBuffer, 0, D3D11_MAP_READ, 0, &mapped_res);
        
		// set a break point below, and drag MappedResource.pData into in your Watch window
		// and cast it as (float*)

		// Write to disk
		Vector2* v = (Vector2*)mapped_res.pData;

		FILE* fp = fopen(".\\tmp\\Ht_raw.dat", "wb");
		fwrite(v, 512*512*sizeof(float)*2*3, 1, fp);
		fclose(fp);

		d3d_context->Unmap(m_pDebugBuffer, 0);
    }
#endif
#ifdef _DEBUG
  CheckObjects();
#endif
}

ID3D11ShaderResourceViewPtr& OceanSimulator::getD3D11DisplacementMap()
{
	return m_pDisplacementSRV;
}

ID3D11ShaderResourceViewPtr& OceanSimulator::getD3D11GradientMap()
{
	return m_pGradientSRV;
}


const OceanParameter& OceanSimulator::getParameters()
{
	return m_param;
}

