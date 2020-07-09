
#include "SpriteData.hlsl"

cbuffer ShaderData {
    matrix u_mvp;
};

float4 GetVertexPosition(float3 vertPos, float3 position, float4 color, float2 uv, float2 pivot, float angle, float2 size) {
    float c = cos(angle);
    float s = sin(angle);

    float2x2 m = float2x2(c, s, -s, c);

    float2 pivotSize = pivot * size;
    float2 pos = position.xy + mul(m, (vertPos.xy - pivotSize - position.xy));
    return mul(u_mvp, float4(pos, position.z, 1.0f));
}

VSOut main(VSIn vsi) {
    VSOut vso;
    vso.color = vsi.color;
    vso.uv = vsi.uv;
    vso.pos = GetVertexPosition(vsi.vertPos, vsi.position, vsi.color, vsi.uv, vsi.pivot, vsi.rotation, vsi.size);
    return vso;
}