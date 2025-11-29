#version 420

layout(location=0) in  vec3 a_vertex_position;
layout(location=1) in  vec3 aColor; 

out vec3 ourColor; 
uniform Transformations {
    mat4 PVM;
 };



void main() {
    gl_Position = PVM * vec4(a_vertex_position, 1.0);

    ourColor = aColor;
}