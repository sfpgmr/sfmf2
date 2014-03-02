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

//#include "DXUT.h"
#include "stdafx.h"

#include "skybox11.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;
//HRESULT CompileShaderFromFile( WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut );

struct SKYBOX_VERTEX
{
    XMFLOAT4  pos;
};

const D3D11_INPUT_ELEMENT_DESC g_aVertexLayout[] =
{
    { "POSITION",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

CSkybox11::CSkybox11(ID3D11Device2Ptr& pd3dDevice, ID3D11DeviceContext2Ptr& context, float fSize,
  ID3D11Texture2DPtr& pCubeTexture, ID3D11ShaderResourceViewPtr& pCubeRV,float width,float height) 
  : m_pd3dDevice11(pd3dDevice), d3d_context_(context), m_fSize(fSize), m_pEnvironmentMap11(pCubeTexture), m_pEnvironmentRV11(pCubeRV)
{

  {
      ID3DBlobPtr pBlobVS = NULL;

      // Create the shaders
      std::wstring shader_path = sf::app_base_directory::instance()->shader_dir() + L"skybox11.hlsl";

      sf::create_shader_blob_from_file(shader_path, "SkyboxVS", "vs_5_0", pBlobVS);
      sf::create_shader_from_file(pd3dDevice, shader_path, "SkyboxPS", "ps_5_0", m_pPixelShader);

      CHK(pd3dDevice->CreateVertexShader(pBlobVS->GetBufferPointer(), pBlobVS->GetBufferSize(), NULL, &m_pVertexShader));

      // Create an input layout
      CHK(pd3dDevice->CreateInputLayout(g_aVertexLayout, 1, pBlobVS->GetBufferPointer(),
        pBlobVS->GetBufferSize(), &m_pVertexLayout11));

    }


    // Query support for linear filtering on DXGI_FORMAT_R32G32B32A32
    UINT FormatSupport = 0;
    CHK( pd3dDevice->CheckFormatSupport( DXGI_FORMAT_R32G32B32A32_FLOAT, &FormatSupport ) );

    // Setup linear or point sampler according to the format Query result
    D3D11_SAMPLER_DESC SamDesc = {};
    SamDesc.Filter = ( FormatSupport & D3D11_FORMAT_SUPPORT_SHADER_SAMPLE ) > 0 ? D3D11_FILTER_MIN_MAG_MIP_LINEAR : D3D11_FILTER_MIN_MAG_MIP_POINT;
    SamDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    SamDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    SamDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    SamDesc.MipLODBias = 0.0f;
    SamDesc.MaxAnisotropy = 1;
    SamDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    SamDesc.BorderColor[0] = SamDesc.BorderColor[1] = SamDesc.BorderColor[2] = SamDesc.BorderColor[3] = 0;
    SamDesc.MinLOD = 0;
    SamDesc.MaxLOD = D3D11_FLOAT32_MAX;
    CHK( pd3dDevice->CreateSamplerState( &SamDesc, &m_pSam ) );  

    // Setup constant buffer
    D3D11_BUFFER_DESC Desc;
    Desc.Usage = D3D11_USAGE_DYNAMIC;
    Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    Desc.MiscFlags = 0;
    Desc.ByteWidth = sizeof( CB_VS_PER_OBJECT );
    CHK( pd3dDevice->CreateBuffer( &Desc, NULL, &m_pcbVSPerObject ) );
    
    // Depth stencil state
    D3D11_DEPTH_STENCIL_DESC DSDesc;
    ZeroMemory( &DSDesc, sizeof( D3D11_DEPTH_STENCIL_DESC ) );
    DSDesc.DepthEnable = FALSE;
    DSDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    DSDesc.DepthFunc = D3D11_COMPARISON_LESS;
    DSDesc.StencilEnable = FALSE;
    CHK( pd3dDevice->CreateDepthStencilState( &DSDesc, &m_pDepthStencilState11 ) );

    create_vb(width,height);
    assert(m_pVertexShader);
    assert(m_pPixelShader);
    assert(m_pSam);
    assert(m_pVertexLayout11);
    assert(m_pcbVSPerObject);
    assert(m_pVB11);
    assert(m_pDepthStencilState11);

}

void CSkybox11::create_vb(float width, float height)
{
  // Fill the vertex buffer
  std::unique_ptr<SKYBOX_VERTEX[]> pVertex(new SKYBOX_VERTEX[4]);

  // Map texels to pixels 
  float fHighW = -1.0f - (1.0f / (float) width);
  float fHighH = -1.0f - (1.0f / (float) height);
  float fLowW = 1.0f + (1.0f / (float) width);
  float fLowH = 1.0f + (1.0f / (float) height);

  pVertex[0].pos = XMFLOAT4(fLowW, fLowH, 1.0f, 1.0f);
  pVertex[1].pos = XMFLOAT4(fLowW, fHighH, 1.0f, 1.0f);
  pVertex[2].pos = XMFLOAT4(fHighW, fLowH, 1.0f, 1.0f);
  pVertex[3].pos = XMFLOAT4(fHighW, fHighH, 1.0f, 1.0f);

  UINT uiVertBufSize = 4 * sizeof(SKYBOX_VERTEX);
  //Vertex Buffer
  D3D11_BUFFER_DESC vbdesc;
  vbdesc.ByteWidth = uiVertBufSize;
  vbdesc.Usage = D3D11_USAGE_IMMUTABLE;
  vbdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
  vbdesc.CPUAccessFlags = 0;
  vbdesc.MiscFlags = 0;

  D3D11_SUBRESOURCE_DATA InitData;
  InitData.pSysMem = pVertex.get();
  CHK(m_pd3dDevice11->CreateBuffer(&vbdesc, &InitData, &m_pVB11));

}

void CSkybox11::OnD3D11ResizedSwapChain(const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc)
{

  create_vb((float) pBackBufferSurfaceDesc->Width, (float) pBackBufferSurfaceDesc->Height);

}
 

void CSkybox11::D3D11Render( XMMATRIX& pmWorldViewProj, ID3D11DeviceContext2Ptr& pd3dImmediateContext )
{
    pd3dImmediateContext->IASetInputLayout( m_pVertexLayout11.Get() );

    UINT uStrides = sizeof( SKYBOX_VERTEX );
    UINT uOffsets = 0;
    ID3D11Buffer* pBuffers[1] = { m_pVB11.Get() };
    pd3dImmediateContext->IASetVertexBuffers( 0, 1, pBuffers, &uStrides, &uOffsets );
    pd3dImmediateContext->IASetIndexBuffer( NULL, DXGI_FORMAT_R32_UINT, 0 );
    pd3dImmediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );

    pd3dImmediateContext->VSSetShader( m_pVertexShader.Get(), NULL, 0 );
    pd3dImmediateContext->PSSetShader( m_pPixelShader.Get(), NULL, 0 );

    D3D11_MAPPED_SUBRESOURCE MappedResource;
    CHK( pd3dImmediateContext->Map( m_pcbVSPerObject.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource ) );
    CB_VS_PER_OBJECT* pVSPerObject = ( CB_VS_PER_OBJECT* )MappedResource.pData;  
    pVSPerObject->m_WorldViewProj = XMMatrixInverse(nullptr,pmWorldViewProj);
    pd3dImmediateContext->Unmap( m_pcbVSPerObject.Get(), 0 );
    pd3dImmediateContext->VSSetConstantBuffers( 0, 1, m_pcbVSPerObject.GetAddressOf() );

    pd3dImmediateContext->PSSetSamplers( 0, 1, m_pSam.GetAddressOf() );
    pd3dImmediateContext->PSSetShaderResources( 0, 1, m_pEnvironmentRV11.GetAddressOf() );

    ID3D11DepthStencilStatePtr pDepthStencilStateStored11;
    UINT StencilRef;
    pd3dImmediateContext->OMGetDepthStencilState( &pDepthStencilStateStored11, &StencilRef );
    pd3dImmediateContext->OMSetDepthStencilState( m_pDepthStencilState11.Get(), 0 );

    pd3dImmediateContext->Draw( 4, 0 );

    pd3dImmediateContext->OMSetDepthStencilState( pDepthStencilStateStored11.Get(), StencilRef );
}

void CSkybox11::OnD3D11ReleasingSwapChain()
{
    m_pVB11.Reset();
}

void CSkybox11::OnD3D11DestroyDevice()
{
//    m_pd3dDevice11 = NULL;
    m_pEnvironmentMap11.Reset();
    m_pEnvironmentRV11.Reset();
    m_pSam.Reset();
    m_pVertexShader.Reset();
    m_pPixelShader.Reset();
    m_pVertexLayout11.Reset();
    m_pcbVSPerObject.Reset();
    m_pDepthStencilState11.Reset();
}