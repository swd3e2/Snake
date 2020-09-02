struct VSInput {
	float3 pos : POSITION0;
	float2 texCoord : TEXCOORD;
};

VSOut main(VSInput vsin) {
	VSOut vsout;

	vsout.pos = float4(vsin.pos, 1.0);
	vsout.texCoord = vsin.texCoord;

	return vsout;
}