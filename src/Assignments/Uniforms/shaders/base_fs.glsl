#version 420

layout(std140, binding = 0) uniform Modifier {
    float strength;
    vec3  color;
    };

in vec3 vColor;
layout(location=0) out vec4 vFragColor;


void main() {
    vec3 c = vColor * strength * color;
    vFragColor = vec4(c, 1.0);
}

    