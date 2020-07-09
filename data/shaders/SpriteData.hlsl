
struct VSOut {
    float4 color : COLOR;
    float2 uv : UV;
    float4 pos : SV_POSITION;
};

struct VSIn {
    float3 vertPos : VERTPOS;
    float3 position : POSITION;
    float4 color : COLOR;
    float2 uv : UV;
    float2 pivot : PIVOT;
    float rotation : ROTATION;
    float2 size : SIZE;
};
