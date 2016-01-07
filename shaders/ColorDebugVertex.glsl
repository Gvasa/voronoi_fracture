#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 color;

uniform mat4 MVP;

out vec4 outColor;

void main() {

    vec4 pos;
    pos.xyz = vertPos;
    pos.w = 1.0;

    outColor = vec4(color, 0.5);

    gl_Position = MVP * pos;
}

