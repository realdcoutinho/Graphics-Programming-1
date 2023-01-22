//--------------------------------
// GLOBAL VARIABLES
//--------------------------------


//Textures2D 
Texture2D gDiffuseMap : DiffuseMap;
Texture2D gNormalMap : NormalMap;
Texture2D gSpecularMap : SpecularMap;
Texture2D gGlossinessMap : GlossinessMap;

//Matrices
float4x4 gWorldViewProj : WorldViewProjection;
float4x4 gWorldMatrix : WORLD;
float4x4 gViewInverseMatrix : VIEWINVERSE;

//Light
float3 gLightDirection = float3( 0.577f, -0.577f, 0.577f ); // check this agian later
float LightIntensity = float(7.0f);
float Shininess = float(25.0f);
float3 Ambient = float3(0.025f, 0.025f, 0.025f);

//Maths
float PI = float(3.14159265f);

RasterizerState gRasterizerStateFront
{
	CullMode = front;
	FrontCounterClockWise = true; // default
};

RasterizerState gRasterizerStateBack
{
	CullMode = back;
	FrontCounterClockWise = true; // default
};

RasterizerState gRasterizerStateNone
{
	CullMode = none;
	FrontCounterClockWise = true; // default
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
	DepthWriteMask = true;
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


struct Light
{
	//float x = 0.0; //Initialized values result in error, why?
	float3 lightDirection;
	float3 lightintensity;
	float3 ambient;
	float shininess;
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
	VS_OUTPUT output	= (VS_OUTPUT)0;
	output.Position		= float4(input.Position, 1.0f);
	output.Position		= mul(output.Position, gWorldViewProj);
	//output.WorldPosition = input.Color;
	output.TextureUV	= input.TextureUV;
	output.Normal		= mul(normalize(input.Normal), (float3x3)gWorldMatrix);
	output.Tangent		= mul(normalize(input.Tangent), (float3x3)gWorldMatrix);
	

	output.WorldPosition = mul(input.Position, gWorldMatrix);
	
	return output;
}

//--------------------------------
// Shading Functions
//--------------------------------

float Dot(float3 A, float3 B)
{
	return float(A.x * B.x + A.y * B.y + A.z * B.z);
}

float3 Lambert(float kd, float3 cd)
{
	float x = cd.x * kd;
	float y = cd.y * kd;
	float z = cd.z * kd;

	float3 lambert = float3(x, y, z) / PI;

	return lambert;

}

float3 Phong(float3 ks, float exp, float3 lightVector, float3 viewDirection, float3 normal)
{
	float3 reflect = (lightVector - 2 * (dot(normal, lightVector)) * normal);
	float cosineAlpha = (max(0.0f, dot(reflect, viewDirection)));
	float3 phongSpecularReflection = (ks * (pow(cosineAlpha, exp)));
	return phongSpecularReflection;
}



//--------------------------------
// Pixel Shader
//--------------------------------
float4 PS(VS_OUTPUT input) : SV_TARGET //change name in PS.
{
	input.Color = gDiffuseMap.Sample(MeshTextureSampler, input.TextureUV);
	return float4(input.Color, 1.0f);
}

float4 PS_Point(VS_OUTPUT input) : SV_TARGET //change name in PS.
{
		//calculate observed Area
	float observedArea = dot(input.Normal, -gLightDirection);
	if (observedArea < 0)
	{
		return float4(0.0f, 0.0f, 0.0f, 1.0f);
	}

	float3 viewDirection = normalize(input.WorldPosition.xyz - gViewInverseMatrix[3].xyz);

	//calculate normals based on normal maps
	float3 biNormal = normalize(cross(input.Normal, input.Tangent));
	float3 vectorZero = float3(0.0f, 0.0f, 0.0f);
	float4x3 tangentSpaceAxis = float4x3(input.Tangent, biNormal, input.Normal, vectorZero);
	float3 normalSample = gNormalMap.Sample(MeshTextureSamplerPoint, input.TextureUV);

	normalSample = 2.0f * normalSample - float3(1.0f, 1.0f, 1.0f);
	normalSample = normalSample / 255.f;
	normalSample = normalize(mul(normalSample, tangentSpaceAxis));
	observedArea = dot(normalSample, -gLightDirection);
	if (observedArea < 0)
	{
		return float4(0.0f, 0.0f, 0.0f, 1.0f);
	}

	//continue from here tomorrow.
	// time to sample the texture from color, which you have done. next, you need to create a lamber from it
	float3 sampleDiffuseMap = gDiffuseMap.Sample(MeshTextureSamplerPoint, input.TextureUV);
	float3 diffuse = Lambert(LightIntensity, sampleDiffuseMap);

	//sample specular map
	float3 sampleSpecularMap = gSpecularMap.Sample(MeshTextureSamplerPoint, input.TextureUV);

	//sample glossMap
	float3 sampleGlossMap = gGlossinessMap.Sample(MeshTextureSamplerPoint, input.TextureUV);
	float phongExponent = sampleGlossMap.x * Shininess;

	float3 phong = Phong(sampleSpecularMap, phongExponent, -gLightDirection, -viewDirection, input.Normal);

	//combined
	return float4((Ambient + diffuse + phong) * observedArea, 1.0f);
	//observed area
	return float4(observedArea, observedArea, observedArea, 1.0f);
	//diffuse
	return float4(diffuse * observedArea, 1.0f);
	//Specular
	return float4(phong * observedArea, 1.0f);
}


//MAIN!!!
float4 PS_Linear(VS_OUTPUT input) : SV_TARGET //change name PS.
{

	//calculate observed Area
	float observedArea = dot(input.Normal, -gLightDirection);
	if (observedArea < 0)
	{
		return float4( 0.0f, 0.0f, 0.0f, 1.0f );
	}

	float3 viewDirection = normalize(input.WorldPosition.xyz - gViewInverseMatrix[3].xyz);

	//calculate normals based on normal maps
	float3 biNormal = normalize(cross(input.Normal, input.Tangent));
	float3 vectorZero = float3(0.0f, 0.0f, 0.0f);
	float4x3 tangentSpaceAxis = float4x3(input.Tangent, biNormal, input.Normal, vectorZero);
	float3 normalSample = gNormalMap.Sample(MeshTextureSamplerLinear, input.TextureUV);

	normalSample = 2.0f * normalSample - float3(1.0f, 1.0f, 1.0f);
	normalSample = normalSample / 255.f;
	normalSample = normalize(mul(normalSample, tangentSpaceAxis));
	observedArea = dot(normalSample, -gLightDirection);
	if (observedArea < 0)
	{
		return float4(0.0f, 0.0f, 0.0f, 1.0f);
	}

	//continue from here tomorrow.
	// time to sample the texture from color, which you have done. next, you need to create a lamber from it
	float3 sampleDiffuseMap = gDiffuseMap.Sample(MeshTextureSamplerLinear, input.TextureUV);
	float3 diffuse = Lambert(LightIntensity, sampleDiffuseMap);

	//sample specular map
	float3 sampleSpecularMap = gSpecularMap.Sample(MeshTextureSamplerLinear, input.TextureUV);

	//sample glossMap
	float3 sampleGlossMap = gGlossinessMap.Sample(MeshTextureSamplerLinear, input.TextureUV);
	float phongExponent = sampleGlossMap.x * Shininess;

	float3 phong = Phong(sampleSpecularMap, phongExponent, -gLightDirection, -viewDirection, input.Normal);
	
	//return float4(1.0f, 1.0f, 1.0f, 1.0f);

	//combined
	return float4((Ambient + diffuse + phong) * observedArea, 1.0f);
	//observed area
	return float4(observedArea, observedArea, observedArea, 1.0f);
	//diffuse
	return float4(diffuse , 1.0f);
	//Specular
	return float4(phong * observedArea, 1.0f);
}






float4 PS_Anisotropic(VS_OUTPUT input) : SV_TARGET //change name PS.
{
		//calculate observed Area
	float observedArea = dot(input.Normal, -gLightDirection);
	if (observedArea < 0)
	{
		return float4(0.0f, 0.0f, 0.0f, 1.0f);
	}

	float3 viewDirection = normalize(input.WorldPosition.xyz - gViewInverseMatrix[3].xyz);

	//calculate normals based on normal maps
	float3 biNormal = normalize(cross(input.Normal, input.Tangent));
	float3 vectorZero = float3(0.0f, 0.0f, 0.0f);
	float4x3 tangentSpaceAxis = float4x3(input.Tangent, biNormal, input.Normal, vectorZero);
	float3 normalSample = gNormalMap.Sample(MeshTextureSamplerAnisotropic, input.TextureUV);

	normalSample = 2.0f * normalSample - float3(1.0f, 1.0f, 1.0f);
	normalSample = normalSample / 255.f;
	normalSample = normalize(mul(normalSample, tangentSpaceAxis));
	observedArea = dot(normalSample, -gLightDirection);
	if (observedArea < 0)
	{
		return float4(0.0f, 0.0f, 0.0f, 1.0f);
	}

	//continue from here tomorrow.
	// time to sample the texture from color, which you have done. next, you need to create a lamber from it
	float3 sampleDiffuseMap = gDiffuseMap.Sample(MeshTextureSamplerAnisotropic, input.TextureUV);
	float3 diffuse = Lambert(LightIntensity, sampleDiffuseMap);

	//sample specular map
	float3 sampleSpecularMap = gSpecularMap.Sample(MeshTextureSamplerAnisotropic, input.TextureUV);

	//sample glossMap
	float3 sampleGlossMap = gGlossinessMap.Sample(MeshTextureSamplerAnisotropic, input.TextureUV);
	float phongExponent = sampleGlossMap.x * Shininess;

	float3 phong = Phong(sampleSpecularMap, phongExponent, -gLightDirection, -viewDirection, input.Normal);

	//combined
	return float4((Ambient + diffuse + phong) * observedArea, 1.0f);
	//observed area
	return float4(observedArea, observedArea, observedArea, 1.0f);
	//diffuse
	return float4(diffuse * observedArea, 1.0f);
	//Specular
	return float4(phong * observedArea, 1.0f);
}

//--------------------------------
// Technique
//--------------------------------


//FRONT
technique11 TechniquePointFront
{
	pass p0
	{
		SetRasterizerState(gRasterizerStateFront);
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
		SetRasterizerState(gRasterizerStateFront);
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
		SetRasterizerState(gRasterizerStateFront);
		SetDepthStencilState(gDepthStencilState, 0);
		SetBlendState(gBlendState, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_Anisotropic()));
	}
}


//back
technique11 TechniquePointBack
{
	pass p0
	{
		SetRasterizerState(gRasterizerStateBack);
		SetDepthStencilState(gDepthStencilState, 0);
		SetBlendState(gBlendState, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_Point()));
	}
}

technique11 TechniqueLinearBack
{
	pass p0
	{
		SetRasterizerState(gRasterizerStateBack);
		SetDepthStencilState(gDepthStencilState, 0);
		SetBlendState(gBlendState, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_Linear()));
	}
}

technique11 TechniqueAnsotropicBack
{
	pass p0
	{
		SetRasterizerState(gRasterizerStateBack);
		SetDepthStencilState(gDepthStencilState, 0);
		SetBlendState(gBlendState, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_Anisotropic()));
	}
}



//None
technique11 TechniquePointNone
{
	pass p0
	{
		SetRasterizerState(gRasterizerStateNone);
		SetDepthStencilState(gDepthStencilState, 0);
		SetBlendState(gBlendState, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_Point()));
	}
}

technique11 TechniqueLinearNone
{
	pass p0
	{
		SetRasterizerState(gRasterizerStateNone);
		SetDepthStencilState(gDepthStencilState, 0);
		SetBlendState(gBlendState, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_Linear()));
	}
}

technique11 TechniqueAnsotropicNone
{
	pass p0
	{
		SetRasterizerState(gRasterizerStateNone);
		SetDepthStencilState(gDepthStencilState, 0);
		SetBlendState(gBlendState, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_Anisotropic()));
	}
}