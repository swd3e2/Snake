struct VSOut {
	float4 pos		 : SV_POSITION;
	float2 texCoord	 : TEXCOORD;
};

Texture2D tex : TEXTURE: register(t0);

SamplerState ClampSampler : register(s0);

float4 main(VSOut vsout) : SV_TARGET{
	return tex.Sample(ClampSampler, vsout.texCoord);
}