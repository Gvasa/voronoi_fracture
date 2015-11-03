#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertPositions;
layout(location = 1) in vec3 normals;

uniform mat4 MVP;
uniform mat4 MV;
uniform mat4 MV_light;
uniform vec4 lightPos;

out vec3 normal;
out vec3 v;
out vec3 light_direction;

void main() {

    gl_Position =  MVP * vec4(vertPositions, 1.0);
    
    v = vec3(MV * vec4(vertPositions, 1.0));

    normal = normals;

    vec3 l = vec3(MV * lightPos);

    light_direction = normalize(l - v);
}

