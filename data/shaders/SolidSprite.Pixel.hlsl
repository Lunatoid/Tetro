
#include "SpriteData.hlsl"

Texture2D tex;

SamplerState samp;

float4 main(in VSOut vso) : SV_TARGET {
    float alpha = tex.Sample(samp, vso.uv).w;
	return float4(vso.color.xyz, alpha);
}
