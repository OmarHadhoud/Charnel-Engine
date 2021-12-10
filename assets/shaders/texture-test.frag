#version 330 core

in Varyings {
    vec3 position;
    vec4 color;
    vec2 tex_coord;
    vec3 normal;
} fs_in;

out vec4 frag_color;

uniform sampler2D tex;

void main(){
    //TODO: Change the following line to read the fragment color
    // from the texture at the received texture coordinates
    // use 'texture' function to sample from the sent sampler 'tex' by sampling with the coordinate of the tex_coord for this fragment
    frag_color = texture(tex, fs_in.tex_coord);
}