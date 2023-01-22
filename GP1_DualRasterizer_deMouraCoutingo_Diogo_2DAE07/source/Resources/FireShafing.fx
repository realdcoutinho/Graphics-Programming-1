//--------------------------------
// GLOBAL VARIABLES
//--------------------------------



//Textures2D 
Texture2D gDiffuseMap : DiffuseMap;

//Matrices
float4x4 gWorldViewProj : WorldViewProjection;
float4x4 gWorldMatrix : WORLD;
float4x4 gViewInverseMatrix : VIEWINVERSE;

//Maths
float PI = float(3.14159265f);

RasterizerState gRasterizerState
{
	CullMode = none;
	FrontCounterClockWise = false; // default
};

BlendState gBlendState
{
	BlendEnable[0] = true;
	SrcBlend = src_alpha;
	DestBlend = inv_src_alpha;
	BlendOp = add;
	SrcBlendAlpha = zero;
	DestBlendAlpha = zero;
	BlendOpAlpha = add;
	RenderTargetWriteMask[0] = 0x0F;
};

DepthStencilState gDepthStencilState
{
	DepthEnable = true;
	DepthWriteMask = zero;
	DepthFunc = less;
	StencilEnable = false;

	StencilReadMask = 0x0F;
	StencilWriteMask = 0x0F;

	FrontFaceStencilFunc = always;
	BackFaceStencilFunc = always;

	FrontFaceStencilDepthFail = keep;
	BackFaceStencilDepthFail = keep;

	FrontFaceStencilPass = keep;
	BackFaceStencilPass = keep;

	FrontFaceStencilFail = keep;
	BackFaceStencilFail = keep;
};


SamplerState MeshTextureSampler //OLDD ONE
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Wrap; // or Mirror, Clamp, Border
	AddressV = Wrap; // or Mirror, Clamp, Border
};

SamplerState MeshTextureSamplerPoint
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Wrap; // or Mirror, Clamp, Border
	AddressV = Wrap; // or Mirror, Clamp, Border
};

SamplerState MeshTextureSamplerLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap; // or Mirror, Clamp, Border
	AddressV = Wrap; // or Mirror, Clamp, Border
};

SamplerState MeshTextureSamplerAnisotropic
{
	Filter = ANISOTROPIC;
	AddressU = Wrap; // or Mirror, Clamp, Border
	AddressV = Wrap; // or Mirror, Clamp, Border
};

//--------------------------------
// INPUT/OUTPUT STRUCTS
//--------------------------------
struct VS_INPUT
{
	float3 Position		: POSITION;
	float3 Color		: COLOR;
	float3 Normal		: NORMAL;
	float3 Tangent		: TANGENT;
	float2 TextureUV	: TEXCOORD;
};


struct VS_OUTPUT
{
	float4 Position			: SV_POSITION;
	float3 Color			: COLOR;
	float3 Normal			: NORMAL;
	float3 Tangent			: TANGENT;
	float2 TextureUV		: TEXCOORD;

	float4 WorldPosition: WORLDPOSITION;

};

//--------------------------------
// Vertex Shader
//--------------------------------
VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.Position = float4(input.Position, 1.0f);
	output.Position = mul(output.Position, gWorldViewProj);
	//output.WorldPosition = input.Color;
	output.TextureUV = input.TextureUV;
	output.Normal = mul(normalize(input.Normal), (float3x3)gWorldMatrix);
	output.Tangent = mul(normalize(input.Tangent), (float3x3)gWorldMatrix);


	output.WorldPosition = mul(input.Position, gWorldMatrix);

	return output;
}

//--------------------------------
// Pixel Shader
//--------------------------------

float4 PS_Point(VS_OUTPUT input) : SV_TARGET //change name in PS.
{
	return gDiffuseMap.Sample(MeshTextureSamplerPoint, input.TextureUV);
}

float4 PS_Linear(VS_OUTPUT input) : SV_TARGET
{
	return gDiffuseMap.Sample(MeshTextureSamplerPoint, input.TextureUV);

}

float4 PS_Anisotropic(VS_OUTPUT input) : SV_TARGET
{
	return gDiffuseMap.Sample(MeshTextureSamplerPoint, input.TextureUV);

}


//--------------------------------
// Technique
//--------------------------------

technique11 TechniquePointFront
{
	pass p0
	{
		SetRasterizerState(gRasterizerState);
		SetDepthStencilState(gDepthStencilState, 0);
		SetBlendState(gBlendState, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_Point()));
	}
}

technique11 TechniqueLinearFront
{
	pass p0
	{
		SetRasterizerState(gRasterizerState);
		SetDepthStencilState(gDepthStencilState, 0);
		SetBlendState(gBlendState, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_Linear()));
	}
}

technique11 TechniqueAnsotropicFront
{
	pass p0
	{
		SetRasterizerState(gRasterizerState);
		SetDepthStencilState(gDepthStencilState, 0);
		SetBlendState(gBlendState, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetRasterizerState(gRasterizerState);
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_Anisotropic()));
	}
}