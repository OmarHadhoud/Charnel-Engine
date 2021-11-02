#version 330 core

out vec4 frag_color;

//Two uniforms defining the side-length and the center
uniform float side_length = 128.0;
uniform vec2 center = vec2(256.0, 256.0); 

uniform vec4 inside_color = vec4(1.0, 0.0, 0.0, 1.0);
uniform vec4 outside_color = vec4(0.0, 0.0, 0.0, 1.0);

void main(){
    //TODO: Write code that will draw the diamond
    float x = gl_FragCoord.x;
    float y = gl_FragCoord.y;
    float xc = center.x;
    float yc = center.y;
    float man_dist = abs(x - xc) + abs(y - yc);
    if(man_dist <= side_length / 2)
    {
        frag_color = inside_color;
    }
    else
    {
        frag_color = outside_color;
    }
    
}