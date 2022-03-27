#shader vertex
#version 330 core

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vUV;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;


void main() { gl_Position = P * V * M * vec4(vPos, 1.0); }

#shader fragment
#version 330 core

out vec4 FragColor;

const vec3 objectColor = vec3(1.0, 1.0, 1.0);

void main() { FragColor = vec4(objectColor, 1.0); }