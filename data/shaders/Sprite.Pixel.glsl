#version 330 core

in vec4 spriteColor;
in vec2 uv;

uniform sampler2D tex0;

out vec4 color;

void main() {
    color = spriteColor * texture(tex0, uv);
}
