#version 330 core

out vec4 frag_color;

//TODO: Define uniforms for the center and the side-length
uniform vec2 center = vec2(0.0, 0.0);
uniform float side_length = 1;

uniform vec4 inside_color = vec4(1.0, 0.0, 0.0, 1.0);
uniform vec4 outside_color = vec4(0.0, 0.0, 0.0, 1.0);

void main(){
    //TODO: Write code that will draw the square
    float x = gl_FragCoord.x;
    float y = gl_FragCoord.y;

    float infinity_norm_distance = max(abs(x - center.x), abs(y - center.y));
    
    if(infinity_norm_distance <= side_length/2){
        frag_color = inside_color;
    } else {
        frag_color = outside_color;
    }
}