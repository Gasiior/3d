#version 420

in vec3 vertexColor;
in vec2 vertex_texcoords;

layout(location=0) out vec4 vFragColor;

uniform Color {
    vec4 Kd;
    bool use_map_Kd;
};

uniform sampler2D map_Kd;

void main() {
    vFragColor = vec4(1.0, 0.0, 0.0, 1.0);
}