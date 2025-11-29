#version 420

in vec3 vertexColor;

layout(location = 0) out vec4 vFragColor;

uniform Color {
    vec4 Kd;
    bool use_map_Kd;
};

void main() {
    // bierzemy tylko kolor z wierzchołków
    vFragColor = vec4(vertexColor, 1.0);
}