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

#ifndef _SKYBOX11_H
#define _SKYBOX11_H

class CSkybox11
{
public:
  CSkybox11(ID3D11Device2Ptr& pd3dDevice, ID3D11DeviceContext2Ptr& context, float fSize,
    ID3D11Texture2DPtr& pCubeTexture, ID3D11ShaderResourceViewPtr& pCubeRV,float width,float height);

    void    OnD3D11ResizedSwapChain( const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc );
    void    D3D11Render(DirectX::XMMATRIX& pmWorldViewProj, ID3D11DeviceContext2Ptr& pd3dImmediateContext);
    void    OnD3D11ReleasingSwapChain();
    void    OnD3D11DestroyDevice();

    ID3D11Texture2D* GetD3D10EnvironmentMap()
    {
        return m_pEnvironmentMap11.Get();
    }
    ID3D11ShaderResourceView* GetD3D10EnvironmentMapRV()
    {
        return m_pEnvironmentRV11.Get();
    }
    void    SetD3D11EnvironmentMap( ID3D11Texture2D* pCubeTexture )
    {
        m_pEnvironmentMap11 = pCubeTexture;
    }

protected:

  void create_vb(float width, float height);

    struct CB_VS_PER_OBJECT
    {
      DirectX::XMMATRIX m_WorldViewProj;
    };    

    ID3D11Device2Ptr& m_pd3dDevice11;
    ID3D11DeviceContext2Ptr& d3d_context_;

    ID3D11Texture2DPtr& m_pEnvironmentMap11;
    ID3D11ShaderResourceViewPtr& m_pEnvironmentRV11;
    ID3D11VertexShaderPtr m_pVertexShader;
    ID3D11PixelShaderPtr m_pPixelShader;
    ID3D11SamplerStatePtr m_pSam;
    ID3D11InputLayoutPtr m_pVertexLayout11;
    ID3D11BufferPtr m_pcbVSPerObject;
    ID3D11BufferPtr m_pVB11;
    ID3D11DepthStencilStatePtr m_pDepthStencilState11;

    float m_fSize;
};

#endif