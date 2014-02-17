#pragma once
#include "video_renderer.h"

namespace sf{



  class fluidcs11_renderer_base
  {
  public:
    struct init_params_t {
      std::wstring title;
      int sample_length;
    };
    explicit fluidcs11_renderer_base(sf::video_renderer_resources& res, init_params_t& p);

    void init_view_matrix();
    void discard();
    void render(LONGLONG t, INT16* wave_data, int length);
    void render(LONGLONG t, int samplepos, audio_samples_t& audio_samples);
    virtual ~fluidcs11_renderer_base();
  private:
    sf::video_renderer_resources res_;
    LONGLONG time_;
    // Direct3D メソッド
    void GPUSort(ID3D11UnorderedAccessViewPtr& inUAV, ID3D11ShaderResourceViewPtr& inSRV,
      ID3D11UnorderedAccessViewPtr& tempUAV, ID3D11ShaderResourceViewPtr& tempSRV);
    void SimulateFluid_Simple();
    void SimulateFluid_Shared();
    void SimulateFluid_Grid();
    void SimulateFluid(float fElapsedTime);
    void CreateSimulationBuffers();
    void RenderFluid(float fElapsedTime);


    // Direct3D リソース ///////////////////////

    //--------------------------------------------------------------------------------------
    // Direct3D11 Global variables
    //--------------------------------------------------------------------------------------
    ID3D11ShaderResourceView* const     g_pNullSRV = NULL;       // Helper to Clear SRVs
    ID3D11UnorderedAccessView* const    g_pNullUAV = NULL;       // Helper to Clear UAVs
    ID3D11Buffer* const                  g_pNullBuffer = NULL;    // Helper to Clear Buffers
    UINT                                g_iNullUINT = 0;         // Helper to Clear Buffers

    // Shaders
    ID3D11VertexShaderPtr                 g_pParticleVS;
    ID3D11GeometryShaderPtr               g_pParticleGS;
    ID3D11PixelShaderPtr                  g_pParticlePS;

    ID3D11ComputeShaderPtr                g_pBuildGridCS;
    ID3D11ComputeShaderPtr                g_pClearGridIndicesCS;
    ID3D11ComputeShaderPtr                g_pBuildGridIndicesCS;
    ID3D11ComputeShaderPtr                g_pRearrangeParticlesCS;
    ID3D11ComputeShaderPtr                g_pDensity_SimpleCS;
    ID3D11ComputeShaderPtr                g_pForce_SimpleCS;
    ID3D11ComputeShaderPtr                g_pDensity_SharedCS;
    ID3D11ComputeShaderPtr                g_pForce_SharedCS;
    ID3D11ComputeShaderPtr                g_pDensity_GridCS;
    ID3D11ComputeShaderPtr                g_pForce_GridCS;
    ID3D11ComputeShaderPtr                g_pIntegrateCS;

    ID3D11ComputeShaderPtr                g_pSortBitonic;
    ID3D11ComputeShaderPtr                g_pSortTranspose;

    // Structured Buffers
    ID3D11BufferPtr                       g_pParticles;
    ID3D11ShaderResourceViewPtr           g_pParticlesSRV;
    ID3D11UnorderedAccessViewPtr          g_pParticlesUAV;

    ID3D11BufferPtr                       g_pSortedParticles;
    ID3D11ShaderResourceViewPtr           g_pSortedParticlesSRV;
    ID3D11UnorderedAccessViewPtr          g_pSortedParticlesUAV;

    ID3D11BufferPtr                       g_pParticleDensity;
    ID3D11ShaderResourceViewPtr           g_pParticleDensitySRV;
    ID3D11UnorderedAccessViewPtr          g_pParticleDensityUAV;

    ID3D11BufferPtr                       g_pParticleForces;
    ID3D11ShaderResourceViewPtr           g_pParticleForcesSRV;
    ID3D11UnorderedAccessViewPtr          g_pParticleForcesUAV;

    ID3D11BufferPtr                       g_pGrid;
    ID3D11ShaderResourceViewPtr           g_pGridSRV;
    ID3D11UnorderedAccessViewPtr          g_pGridUAV;

    ID3D11BufferPtr                       g_pGridPingPong;
    ID3D11ShaderResourceViewPtr           g_pGridPingPongSRV;
    ID3D11UnorderedAccessViewPtr          g_pGridPingPongUAV;

    ID3D11BufferPtr                       g_pGridIndices;
    ID3D11ShaderResourceViewPtr           g_pGridIndicesSRV;
    ID3D11UnorderedAccessViewPtr          g_pGridIndicesUAV;

    // Constant Buffers
    ID3D11BufferPtr                       g_pcbSimulationConstants;
    ID3D11BufferPtr                       g_pcbRenderConstants;
    ID3D11BufferPtr                       g_pSortCB;

    // Direct2D リソース ///////////////////////

    std::wstring                                    text_;
    DWRITE_TEXT_METRICS	                            text_metrics_;
    ID2D1SolidColorBrushPtr white_brush_;
    ID2D1DrawingStateBlockPtr state_;
    IDWriteTextLayoutPtr       text_layout_;
    IDWriteTextFormatPtr		text_format_;

    int length_;

    std::unique_ptr<double []> a_;
    std::unique_ptr<double[]> a_buffer_[4];
    std::unique_ptr<double[]> w_;
    std::unique_ptr<int []> ip_;
    std::unique_ptr<double []> log_;

    std::unique_ptr<double []> a_1_;
    std::unique_ptr<double[]> a_1_buffer_[4];
    std::unique_ptr<double []> w_1_;
    std::unique_ptr<int []> ip_1_;
    std::unique_ptr<double []> log_1_;

    int buffer_index_;
    

  };

  typedef h264_renderer2<fluidcs11_renderer_base> fluidcs11_renderer;

}

