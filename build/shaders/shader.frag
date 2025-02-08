#version 330 core

in vec2 uv;
uniform sampler2D texture_0;

void main(){

	gl_FragColor = texture(texture_0,uv) + 0.1;
}