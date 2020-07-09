#version 330 core

layout(location = 0) in vec3 aVertPos;
layout(location = 1) in vec3 aPosition;
layout(location = 2) in vec4 aColor;
layout(location = 3) in vec2 aUV;
layout(location = 4) in vec2 aPivot;
layout(location = 5) in float aRotation;
layout(location = 6) in vec2 aSize;

out vec4 spriteColor;
out vec2 uv;

uniform mat4 u_mvp;

vec4 GetVertexPosition(vec3 position, vec2 pivot, vec3 vertPos, vec2 size, float angle) {
    float c = cos(angle);
    float s = sin(angle);
    mat2 m = mat2(c, s, -s, c);
    
    vec2 pivotSize = pivot * size;
    vec2 pos = position.xy + m * (vertPos.xy - pivotSize - position.xy);
    return u_mvp * vec4(pos, position.z, 1.0);
}

void main() {
    gl_Position = GetVertexPosition(aPosition, aPivot, aVertPos, aSize, aRotation);
    spriteColor = aColor;
    uv          = aUV;
}
