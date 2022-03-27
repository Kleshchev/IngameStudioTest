#shader vertex
#version 330 core

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vUV;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;

void main() {
  FragPos     = vec3(M * vec4(vPos, 1.0));
  Normal      = vNormal;
  gl_Position = P * V * M * vec4(vPos, 1.0);
  TexCoord    = vUV;
}

#shader fragment
#version 330 core

out vec4 FragColor;

const vec3 lightPos    = vec3(3.0, 3.0, 3.0);
const vec3 lightColor  = vec3(0.91, 0.92, 0.6);
const vec3 objectColor = vec3(1.0, 1.0, 1.0);

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform sampler2D texture1;

void main() {
  vec3  ambient  = vec3(0.6) * lightColor;
  vec3  norm     = normalize(Normal);
  vec3  lightDir = normalize(lightPos - FragPos);
  float diff     = max(dot(norm, lightDir), 0.0);
  vec3  diffuse  = diff * lightColor;
  vec3  result   = (ambient + diffuse) * objectColor;
  FragColor      = vec4(result, 1.0) * texture(texture1, TexCoord);
}