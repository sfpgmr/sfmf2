//--------------------------------------------------------------------------------------
// File: dxgi_test.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// �萔�o�b�t�@
//--------------------------------------------------------------------------------------
Texture2D txDiffuse : register( t0 );
SamplerState samLinear : register( s0 );

cbuffer cbNeverChanges : register( b0 )
{
    matrix View;
	float4 vLightDir;
};

cbuffer cbChangeOnResize : register( b1 )
{
    matrix Projection;
};

cbuffer cbChangesEveryFrame : register( b2 )
{
    matrix World;
	float4 vLightColor;
    //float4 vMeshColor;
};


//--------------------------------------------------------------------------------------
// ���_���
struct VS_INPUT
{
    float4 Pos : POSITION;
    float3 Norm : NORMAL;
    float2 Tex : TEXCOORD0;
};

// �s�N�Z�����
struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR0;
    float2 Tex : TEXCOORD0;
};


//--------------------------------------------------------------------------------------
// ���_�V�F�[�_�[
//--------------------------------------------------------------------------------------
PS_INPUT VS( VS_INPUT input )
{
    // �o�͗p�ϐ��̏�����
	PS_INPUT output = (PS_INPUT)0;

	// ���[�J�����W���烏�[���h���W�ւ̕ϊ�
    output.Pos = mul( input.Pos, World ); 
	// �J����
    output.Pos = mul( output.Pos, View );
	// �ˉe�ϊ�
    output.Pos = mul( output.Pos, Projection );
	// �e�N�X�`���}�b�s���O�̍��W�ϊ��i����͕ϊ��͂��Ȃ��j
    output.Tex = input.Tex;

	// �@���x�N�g���̌v�Z
    output.Color = mul( input.Norm, World );

	//���ς̌v�Z
	output.Color = dot( (float3)vLightDir,(float3)output.Color) * vLightColor;
	output.Color.a = 1;

    // �o�͂�Ԃ��B
	return output;

}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( PS_INPUT input) : SV_Target
{
    float4 finalColor = 0;
    finalColor = saturate(input.Color) * txDiffuse.Sample( samLinear, input.Tex );
    return finalColor;
}
