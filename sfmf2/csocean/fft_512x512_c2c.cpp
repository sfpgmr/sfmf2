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
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "fft_512x512.h"

//HRESULT CompileShaderFromFile( WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut );


void CSFFT512x512_Plan::radix008A(
        ID3D11DeviceContext2Ptr& d3d_context,
			   ID3D11UnorderedAccessViewPtr& pUAV_Dst,
			   ID3D11ShaderResourceViewPtr& pSRV_Src,
			   UINT thread_count,
			   UINT istride)
{
    // Setup execution configuration
	UINT grid = thread_count / COHERENCY_GRANULARITY;

	// Buffers
  ID3D11ShaderResourceView* cs_srvs[1] = { pSRV_Src.Get() };
  d3d_context->CSSetShaderResources(0, 1, &cs_srvs[0]);

	ID3D11UnorderedAccessView* cs_uavs[1] = {pUAV_Dst.Get()};
  d3d_context->CSSetUnorderedAccessViews(0, 1, cs_uavs, (UINT*) (&cs_uavs[0]));

	// Shader
	if (istride > 1)
		d3d_context->CSSetShader(pRadix008A_CS.Get(), NULL, 0);
	else
		d3d_context->CSSetShader(pRadix008A_CS2.Get(), NULL, 0);

	// Execute
	d3d_context->Dispatch(grid, 1, 1);

	// Unbind resource
	cs_srvs[0] = NULL;
  d3d_context->CSSetShaderResources(0, 1, &cs_srvs[0]);

	cs_uavs[0] = NULL;
	d3d_context->CSSetUnorderedAccessViews(0, 1, cs_uavs, (UINT*)(&cs_uavs[0]));
}

void CSFFT512x512_Plan::fft_512x512_c2c(ID3D11DeviceContext2Ptr& d3d_context,
					 ID3D11UnorderedAccessViewPtr& pUAV_Dst,
					 ID3D11ShaderResourceViewPtr& pSRV_Dst,
					 ID3D11ShaderResourceViewPtr& pSRV_Src)
{
	const UINT thread_count = slices_ * (512 * 512) / 8;

	UINT istride = 512 * 512 / 8;
  d3d_context->CSSetConstantBuffers(0, 1, pRadix008A_CB[0].GetAddressOf());
  radix008A(d3d_context,pUAV_Tmp, pSRV_Src, thread_count, istride);

	istride /= 8;
  d3d_context->CSSetConstantBuffers(0, 1, pRadix008A_CB[1].GetAddressOf());
  radix008A(d3d_context, pUAV_Dst, pSRV_Tmp, thread_count, istride);

	istride /= 8;
  d3d_context->CSSetConstantBuffers(0, 1, pRadix008A_CB[2].GetAddressOf());
  radix008A(d3d_context, pUAV_Tmp, pSRV_Dst, thread_count, istride);

	istride /= 8;
  d3d_context->CSSetConstantBuffers(0, 1, pRadix008A_CB[3].GetAddressOf());
  radix008A(d3d_context, pUAV_Dst, pSRV_Tmp, thread_count, istride);

	istride /= 8;
  d3d_context->CSSetConstantBuffers(0, 1, pRadix008A_CB[4].GetAddressOf());
  radix008A(d3d_context, pUAV_Tmp, pSRV_Dst, thread_count, istride);

	istride /= 8;
  d3d_context->CSSetConstantBuffers(0, 1, pRadix008A_CB[5].GetAddressOf());
  radix008A(d3d_context, pUAV_Dst, pSRV_Tmp, thread_count, istride);
}

void CSFFT512x512_Plan::create_cbuffers_512x512(UINT slices)
{
	// Create 6 cbuffers for 512x512 transform.

  D3D11_BUFFER_DESC cb_desc = {};
	cb_desc.Usage = D3D11_USAGE_IMMUTABLE;
	cb_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb_desc.CPUAccessFlags = 0;
	cb_desc.MiscFlags = 0;    
	cb_desc.ByteWidth = 32;//sizeof(float) * 5;
	cb_desc.StructureByteStride = 0;

  D3D11_SUBRESOURCE_DATA cb_data = {};
	cb_data.SysMemPitch = 0;
	cb_data.SysMemSlicePitch = 0;

	struct CB_Structure
	{
		UINT thread_count;
		UINT ostride;
		UINT istride;
		UINT pstride;
		float phase_base;
	};

	// Buffer 0
	const UINT thread_count = slices * (512 * 512) / 8;
	UINT ostride = 512 * 512 / 8;
	UINT istride = ostride;
	double phase_base = -TWO_PI / (512.0 * 512.0);
	
	CB_Structure cb_data_buf0 = {thread_count, ostride, istride, 512, (float)phase_base};
	cb_data.pSysMem = &cb_data_buf0;

	CHK(d3d_device_->CreateBuffer(&cb_desc, &cb_data, &pRadix008A_CB[0]));

	// Buffer 1
	istride /= 8;
	phase_base *= 8.0;
	
	CB_Structure cb_data_buf1 = {thread_count, ostride, istride, 512, (float)phase_base};
	cb_data.pSysMem = &cb_data_buf1;

	CHK(d3d_device_->CreateBuffer(&cb_desc, &cb_data, &pRadix008A_CB[1]));

	// Buffer 2
	istride /= 8;
	phase_base *= 8.0;
	
	CB_Structure cb_data_buf2 = {thread_count, ostride, istride, 512, (float)phase_base};
	cb_data.pSysMem = &cb_data_buf2;

	CHK(d3d_device_->CreateBuffer(&cb_desc, &cb_data, &pRadix008A_CB[2]));

	// Buffer 3
	istride /= 8;
	phase_base *= 8.0;
	ostride /= 512;
	
	CB_Structure cb_data_buf3 = {thread_count, ostride, istride, 1, (float)phase_base};
	cb_data.pSysMem = &cb_data_buf3;

	CHK(d3d_device_->CreateBuffer(&cb_desc, &cb_data, &pRadix008A_CB[3]));

	// Buffer 4
	istride /= 8;
	phase_base *= 8.0;
	
	CB_Structure cb_data_buf4 = {thread_count, ostride, istride, 1, (float)phase_base};
	cb_data.pSysMem = &cb_data_buf4;

	CHK(d3d_device_->CreateBuffer(&cb_desc, &cb_data, &pRadix008A_CB[4]));

	// Buffer 5
	istride /= 8;
	phase_base *= 8.0;
	
	CB_Structure cb_data_buf5 = {thread_count, ostride, istride, 1, (float)phase_base};
	cb_data.pSysMem = &cb_data_buf5;

	CHK(d3d_device_->CreateBuffer(&cb_desc, &cb_data, &pRadix008A_CB[5]));
}

CSFFT512x512_Plan::CSFFT512x512_Plan(ID3D11Device2Ptr& d3d_device, ID3D11DeviceContext2Ptr& d3d_context, UINT slices)
: d3d_device_(d3d_device), slices_(slices)
{

	// Compute shaders
    std::wstring shader_path(sf::app_base_directory::instance()->shader_dir() + L"fft_512x512_c2c.hlsl");

    sf::create_shader_from_file(d3d_device_, shader_path, "Radix008A_CS", "cs_4_0", pRadix008A_CS);
    sf::create_shader_from_file(d3d_device_, shader_path, "Radix008A_CS2", "cs_4_0", pRadix008A_CS2);

	// Constants
	// Create 6 cbuffers for 512x512 transform
	create_cbuffers_512x512(slices);

	// Temp buffer
	D3D11_BUFFER_DESC buf_desc;
	buf_desc.ByteWidth = sizeof(float) * 2 * (512 * slices) * 512;
    buf_desc.Usage = D3D11_USAGE_DEFAULT;
    buf_desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
    buf_desc.CPUAccessFlags = 0;
    buf_desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    buf_desc.StructureByteStride = sizeof(float) * 2;

	CHK(d3d_device_->CreateBuffer(&buf_desc, NULL, &pBuffer_Tmp));

	// Temp undordered access view
	D3D11_UNORDERED_ACCESS_VIEW_DESC uav_desc;
	uav_desc.Format = DXGI_FORMAT_UNKNOWN;
	uav_desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uav_desc.Buffer.FirstElement = 0;
	uav_desc.Buffer.NumElements = (512 * slices) * 512;
	uav_desc.Buffer.Flags = 0;

	CHK(d3d_device_->CreateUnorderedAccessView(pBuffer_Tmp.Get(), &uav_desc, &pUAV_Tmp));

	// Temp shader resource view
	D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
	srv_desc.Format = DXGI_FORMAT_UNKNOWN;
	srv_desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srv_desc.Buffer.FirstElement = 0;
	srv_desc.Buffer.NumElements = (512 * slices) * 512;

	CHK(d3d_device->CreateShaderResourceView(pBuffer_Tmp.Get(), &srv_desc, &pSRV_Tmp));

#ifdef _DEBUG
  // Check whether objects were created

  assert(pRadix008A_CS);
  assert(pRadix008A_CS2);

  // For 512x512 config, we need 6 constant buffers
  for (int i = 0; i < 6; ++i){
    assert(pRadix008A_CB[i]);
  }

  // Temporary buffers
  assert(pBuffer_Tmp);
  assert(pUAV_Tmp);
  assert(pSRV_Tmp);
#endif
}

CSFFT512x512_Plan::~CSFFT512x512_Plan()
{
	pSRV_Tmp.Reset();
	pUAV_Tmp.Reset();
  pBuffer_Tmp.Reset();
  pRadix008A_CS.Reset();
  pRadix008A_CS2.Reset();

	for (int i = 0; i < 6; i++)
		pRadix008A_CB[i].Reset();
}
