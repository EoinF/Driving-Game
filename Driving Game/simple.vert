#version 400

layout(location = 0) in vec3 vp; // points

uniform mat4 M; // model matrix


uniform vec4 colour;

out vec4 basecolour;

void main () {
	gl_Position = M * vec4 (vp, 1.0);
	basecolour = colour;
}
