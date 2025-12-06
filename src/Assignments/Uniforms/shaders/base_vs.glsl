#version 420

layout(std140, binding = 1) uniform Transformations {
    vec2 scale;
    vec2 translation;
    mat2 rotation;
};

layout(location=0) in  vec3 a_vertex_position;
layout(location=1) in  vec3 aColor; // the color variable has attribute position 1

out vec3 vColor; /// output a color to the fragment shader


void main() {
    vec2 p = rotation * (scale * a_vertex_position.xy) + translation;
    gl_Position = vec4(p, a_vertex_position.z, 1.0);
    vColor = aColor;
}