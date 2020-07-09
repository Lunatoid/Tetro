
#include "SpriteData.hlsl"

Texture2D tex;

SamplerState samp;

float4 main(in VSOut vso) : SV_TARGET {
	return vso.color * tex.Sample(samp, vso.uv);
}
