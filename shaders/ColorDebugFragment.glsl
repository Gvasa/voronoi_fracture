#version 330 core

out vec4 fragColor;

in vec4 outColor;

void main() {
	fragColor = outColor;
}