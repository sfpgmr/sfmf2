#pragma once
#include "video_renderer.h"
#include "ocean_simulator.h"
#include "skybox11.h"

#define FRESNEL_TEX_SIZE			256
#define PERLIN_TEX_SIZE				64

namespace sf{

  class csocean_renderer_base
  {
  public:
    struct init_params_t {
      std::wstring title;
      int sample_length;
    };
    explicit csocean_renderer_base(sf::video_renderer_resources& res, init_params_t& p);

    void init_view_matrix();
    void discard();
    void render(LONGLONG t, INT16* wave_data, int length);
    void render(LONGLONG t, int samplepos, audio_samples_t& audio_samples);
    virtual ~csocean_renderer_base();
  private:
    sf::video_renderer_resources res_;
    LONGLONG time_;

    // Direct3D リソース ///////////////////////

    //--------------------------------------------------------------------------------------
    // Direct3D11 Global variables
    //--------------------------------------------------------------------------------------
    ID3D11ShaderResourceView* const     g_pNullSRV = nullptr;       // Helper to Clear SRVs
    ID3D11UnorderedAccessView* const    g_pNullUAV = nullptr;       // Helper to Clear UAVs
    ID3D11Buffer* const                  g_pNullBuffer = nullptr;    // Helper to Clear Buffers
    UINT                                g_iNullUINT = 0;         // Helper to Clear Buffers

    struct ocean_vertex
    {
      float index_x;
      float index_y;
    };

    // Mesh properties:

    // Mesh grid dimension, must be 2^n. 4x4 ~ 256x256
    int g_MeshDim = 128;
    // Side length of square shaped mesh patch
    float g_PatchLength;
    // Dimension of displacement map
    int g_DisplaceMapDim;
    // Subdivision thredshold. Any quad covers more pixels than this value needs to be subdivided.
    float g_UpperGridCoverage = 64.0f;
    // Draw distance = g_PatchLength * 2^g_FurthestCover
    int g_FurthestCover = 8;


    // Shading properties:
    // Two colors for waterbody and sky color
    DirectX::SimpleMath::Vector3 g_SkyColor = DirectX::SimpleMath::Vector3(0.38f, 0.45f, 0.56f);
    DirectX::SimpleMath::Vector3 g_WaterbodyColor = DirectX::SimpleMath::Vector3(0.07f, 0.29f, 0.25f);
    // Blending term for sky cubemap
    float g_SkyBlending = 16.0f;

    // Perlin wave parameters
    float g_PerlinSize = 1.0f;
    float g_PerlinSpeed = 0.06f;
    DirectX::SimpleMath::Vector3 g_PerlinAmplitude = DirectX::SimpleMath::Vector3(35, 42, 57);
    DirectX::SimpleMath::Vector3 g_PerlinGradient = DirectX::SimpleMath::Vector3(1.4f, 1.6f, 2.2f);
    DirectX::SimpleMath::Vector3 g_PerlinOctave = DirectX::SimpleMath::Vector3(1.12f, 0.59f, 0.23f);
    DirectX::SimpleMath::Vector2 g_WindDir;

    DirectX::SimpleMath::Vector3 g_BendParam = DirectX::SimpleMath::Vector3(0.1f, -0.4f, 0.2f);

    // Sunspot parameters
    DirectX::SimpleMath::Vector3 g_SunDir = DirectX::SimpleMath::Vector3(0.936016f, -0.343206f, 0.0780013f);
    DirectX::SimpleMath::Vector3 g_SunColor = DirectX::SimpleMath::Vector3(1.0f, 1.0f, 0.6f);
    float g_Shineness = 400.0f;


    // Quadtree structures & routines
    struct QuadNode
    {
      DirectX::SimpleMath::Vector2 bottom_left;
      float length;
      int lod;
      int sub_node[4];
    };

    struct QuadRenderParam
    {
      UINT num_inner_verts;
      UINT num_inner_faces;
      UINT inner_start_index;

      UINT num_boundary_verts;
      UINT num_boundary_faces;
      UINT boundary_start_index;
    };

    std::vector<QuadNode> g_render_list;

    // Quad-tree LOD, 0 to 9 (1x1 ~ 512x512) 
    int g_Lods = 0;
    // Pattern lookup array. Filled at init time.
    QuadRenderParam g_mesh_patterns[9][3][3][3][3];
    // Pick a proper mesh pattern according to the adjacent patches.
    QuadRenderParam& selectMeshPattern(const QuadNode& quad_node);

    // Rendering list
    ID3D11BufferPtr g_pMeshVB;
    ID3D11BufferPtr g_pMeshIB;
    ID3D11InputLayoutPtr g_pMeshLayout;

    // Color look up 1D texture
    ID3D11Texture1DPtr g_pFresnelMap;
    ID3D11ShaderResourceViewPtr g_pSRV_Fresnel;

    // Distant perlin wave
    ID3D11ShaderResourceViewPtr g_pSRV_Perlin;

    // Environment maps
    ID3D11ShaderResourceViewPtr g_pSRV_ReflectCube;

    // HLSL shaders
    ID3D11VertexShaderPtr g_pOceanSurfVS;
    ID3D11PixelShaderPtr g_pOceanSurfPS;
    ID3D11PixelShaderPtr g_pWireframePS;

    // Samplers
    ID3D11SamplerStatePtr g_pHeightSampler;
    ID3D11SamplerStatePtr g_pGradientSampler;
    ID3D11SamplerStatePtr g_pFresnelSampler;
    ID3D11SamplerStatePtr g_pPerlinSampler;
    ID3D11SamplerStatePtr g_pCubeSampler;

    // Constant buffer
    struct Const_Per_Call
    {
      DirectX::XMMATRIX	g_matLocal;
      DirectX::XMMATRIX	g_matWorldViewProj;
      DirectX::XMFLOAT2 g_UVBase;
      DirectX::XMFLOAT2 g_PerlinMovement;
      DirectX::XMFLOAT3	g_LocalEye;
    };

    struct Const_Shading
    {
      // Water-reflected sky color
      DirectX::XMFLOAT3		g_SkyColor;
      float			unused0;
      // The color of bottomless water body
      DirectX::XMFLOAT3		g_WaterbodyColor;

      // The strength, direction and color of sun streak
      float			g_Shineness;
      DirectX::XMFLOAT3		g_SunDir;
      float			unused1;
      DirectX::XMFLOAT3		g_SunColor;
      float			unused2;

      // The parameter is used for fixing an artifact
      DirectX::XMFLOAT3		g_BendParam;

      // Perlin noise for distant wave crest
      float			g_PerlinSize;
      DirectX::XMFLOAT3		g_PerlinAmplitude;
      float			unused3;
      DirectX::XMFLOAT3		g_PerlinOctave;
      float			unused4;
      DirectX::XMFLOAT3		g_PerlinGradient;

      // Constants for calculating texcoord from position
      float			g_TexelLength_x2;
      float			g_UVScale;
      float			g_UVOffset;
    };

    ID3D11BufferPtr g_pPerCallCB;
    ID3D11BufferPtr g_pPerFrameCB;
    ID3D11BufferPtr g_pShadingCB;

    // State blocks
    ID3D11RasterizerStatePtr g_pRSState_Solid;
    ID3D11RasterizerStatePtr g_pRSState_Wireframe;
    ID3D11DepthStencilStatePtr g_pDSState_Disable;
    ID3D11BlendStatePtr g_pBState_Transparent;

    // init & cleanup
    void initRenderResource(const OceanParameter& ocean_param);
    void cleanupRenderResource();
    // create a triangle strip mesh for ocean surface.
    void createSurfaceMesh();
    // create color/fresnel lookup table.
    void createFresnelMap();
    // create perlin noise texture for far-sight rendering
    void loadTextures();
    // Rendering routines
    void renderShaded(ID3D11ShaderResourceViewPtr& displacemnet_map, ID3D11ShaderResourceViewPtr& gradient_map,float time);
    void renderWireframe(ID3D11ShaderResourceViewPtr& displacemnet_map,float time);
    int generateBoundaryMesh(int left_degree, int right_degree, int bottom_degree, int top_degree,
      RECT vert_rect, DWORD* output);
    int generateInnerMesh(RECT vert_rect, DWORD* output);
    bool checkNodeVisibility(const QuadNode& quad_node);
    float estimateGridCoverage(const QuadNode& quad_node, float screen_area);
    bool isLeaf(const QuadNode& quad_node);
    int searchLeaf(const std::vector<QuadNode>& node_list, const DirectX::SimpleMath::Vector2& point);
    int buildNodeList(QuadNode& quad_node);
    void CreateOceanSimAndRender();

    DirectX::SimpleMath::Vector3 eye_;
    DirectX::SimpleMath::Vector3 at_;
    DirectX::SimpleMath::Vector3 up_;
    DirectX::SimpleMath::Matrix mat_world_;
    DirectX::SimpleMath::Matrix mat_view_;
    DirectX::SimpleMath::Matrix mat_projection_;

    // Ocean simulation variables
    std::unique_ptr<OceanSimulator> g_pOceanSimulator;

    bool g_RenderWireframe = false;
    bool g_PauseSimulation = false;
    int g_BufferType = 0;

    // Skybox
    ID3D11Texture2DPtr g_pSkyCubeMap;
    ID3D11ShaderResourceViewPtr g_pSRV_SkyCube;

    std::unique_ptr<CSkybox11> g_Skybox;

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

  typedef h264_renderer2<csocean_renderer_base> csocean_renderer;

}

