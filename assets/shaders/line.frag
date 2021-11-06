#version 330 core

out vec4 frag_color;

//TODO: Define uniforms for the slope and the intercept
uniform float slope = 1;
uniform float intercept = 3;

uniform vec4 inside_color = vec4(1.0, 0.0, 0.0, 1.0);
uniform vec4 outside_color = vec4(0.0, 0.0, 0.0, 1.0);

void main(){
    //TODO: Write code that will draw the line
    float x = gl_FragCoord.x;
    float y = gl_FragCoord.y;

    if(y <= slope * x + intercept){
        frag_color = inside_color;
    } else {
        frag_color = outside_color;
    }
}