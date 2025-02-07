#version 330 core
layout(location=0) in vec3 vertex;
layout(location=1) in vec2 texture_coord;

out vec2 uv;

void main(){
	// vcolor = vertex + 0.5;
	uv = texture_coord;
	gl_Position = vec4(vertex,1.0f);
}
