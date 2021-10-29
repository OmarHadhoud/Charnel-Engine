#version 330 core

out vec4 frag_color;

uniform vec2 center = vec2(0.0, 0.0);
uniform float radius = 1;
uniform vec4 inside_color = vec4(1.0, 0.0, 0.0, 1.0);
uniform vec4 outside_color = vec4(0.0, 0.0, 0.0, 1.0);

void main(){
    //TODO: Write code that will draw the circle
    float x = gl_FragCoord.x;
    float y = gl_FragCoord.y;
    float distance_sqrd = (x - center.x) * (x - center.x) + (y - center.y) * (y - center.y);
    if (distance_sqrd <= radius * radius)
        frag_color = inside_color;
    else
        frag_color = outside_color;
}