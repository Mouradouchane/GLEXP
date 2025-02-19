#version 330 core

in vec2 uv;
uniform sampler2D texture_0;

void main(){
	vec4 color = vec4(1.0f,0.0f,0.0f,1.0f);
	
	//gl_FragColor = texture(texture_0,uv) + 0.5;
	gl_FragColor = color;
}