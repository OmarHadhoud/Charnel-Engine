#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec2 tex_coord;
layout(location = 3) in vec3 normal;

out Varyings {
    vec4 color;
    vec2 tex_coord;
    vec3 world;
    vec3 view;
    vec3 normal;
} vs_out;

// transforms model from object space to world space
uniform mat4 model;
// used to transofrm the normals from object to world space
uniform mat4 model_inv_transpose;
// transforms the view from world space to camera space and then projects it
uniform mat4 view_proj;

uniform vec3 camera_pos;

void main(){
    // the position of the vertex in world space
    vs_out.world = (model * vec4(position, 1.0)).xyz;
    // the view vector of the vertex in world space
    vs_out.view = camera_pos - vs_out.world;
    // the normal of the vertex in world space
    vs_out.normal = normalize((model_inv_transpose * vec4(normal, 0.0)).xyz);

    gl_Position = view_proj * vec4(vs_out.world, 1.0);
    vs_out.color = color;
    vs_out.tex_coord = tex_coord;
}