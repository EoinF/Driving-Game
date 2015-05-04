#version 400

layout(location = 0) in vec3 vp; // points
layout(location = 1) in vec2 vt; //texture coordinates
layout(location = 2) in vec3 vn; // normals

uniform mat4 M; // model matrix
uniform mat4 V; // view matrix
uniform mat4 P; // projection matrix

uniform vec4 colour;

out vec2 texture_coordinates;
out vec3 Normal0;
out vec4 basecolour;

void main () {
	gl_Position = P * V * M * vec4 (vp, 1.0);
	
	texture_coordinates = vt;
    Normal0 = (M * vec4(vn, 0.0)).xyz;
	
	basecolour = colour;
}
