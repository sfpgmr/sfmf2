//--------------------------------------------------------------------------------------
// File: Tutorial07.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// 定数バッファ
//--------------------------------------------------------------------------------------
Texture2D txDiffuse : register( t0 );
SamplerState samLinear : register( s0 );

cbuffer cbNeverChanges : register( b0 )
{
    matrix View;
	float4 vLightDir[2];
	float4 vLightColor[2];
};

cbuffer cbChangeOnResize : register( b1 )
{
    matrix Projection;
};

cbuffer cbChangesEveryFrame : register( b2 )
{
    matrix World;
    float4 vMeshColor;
};


//--------------------------------------------------------------------------------------
// 頂点情報
struct VS_INPUT
{
    float4 Pos : POSITION;
    float2 Tex : TEXCOORD0;
};

// ピクセル情報
struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
};


//--------------------------------------------------------------------------------------
// 頂点シェーダー
//--------------------------------------------------------------------------------------
PS_INPUT VS( VS_INPUT input )
{
    // 出力用変数の初期化
	PS_INPUT output = (PS_INPUT)0;

	// ローカル座標からワールド座標への変換
    output.Pos = mul( input.Pos, World ); 
	// カメラ
    output.Pos = mul( output.Pos, View );
	// 射影変換
    output.Pos = mul( output.Pos, Projection );
	// テクスチャマッピングの座標変換（今回は変換はしない）
    output.Tex = input.Tex;
    // 出力を返す。
    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( PS_INPUT input) : SV_Target
{
    return txDiffuse.Sample( samLinear, input.Tex ) * vMeshColor ;
}
