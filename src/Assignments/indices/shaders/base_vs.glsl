#version 420

layout(location = 0) in vec3 a_vertex_position;
layout(location = 1) in vec3 a_vertex_color;

out vec3 vertexColor;

uniform Transformations {
    mat4 PVM;
};

void main() {
    gl_Position = PVM * vec4(a_vertex_position, 1.0);
    vertexColor = a_vertex_color;
}