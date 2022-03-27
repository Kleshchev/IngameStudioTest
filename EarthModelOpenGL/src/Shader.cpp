#include <GL/glew.h>

#include "Shader.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

Shader::Shader(const std::string& filepath):
    m_FilePath(filepath), m_RendererID(0) {
  ShaderProgramSource source = ParseShader(filepath);

  m_RendererID = CreateShader(source.VertexSource, source.FragmentSource);

  glUseProgram(m_RendererID);
}

Shader::~Shader() { glDeleteProgram(m_RendererID); }

void Shader::Bind() const { glUseProgram(m_RendererID); }

void Shader::Unbind() const { glUseProgram(0); }

int Shader::GetUniformLocation(const std::string& name) {
  if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
    return m_UniformLocationCache[name];

  int location = glGetUniformLocation(m_RendererID, name.c_str());

  m_UniformLocationCache[name] = location;

  return location;
}

void Shader::SetUniform1i(const std::string& name, int value) {
  glUniform1i(GetUniformLocation(name), value);
}

void Shader::SetUniform1f(const std::string& name, float value) {
  glUniform1f(GetUniformLocation(name), value);
}

void Shader::SetUniform3f(const std::string& name,
                          float              f0,
                          float              f1,
                          float              f2) {
  glUniform3f(GetUniformLocation(name), f0, f1, f2);
}

void Shader::SetUniform4f(const std::string& name,
                          float              f0,
                          float              f1,
                          float              f2,
                          float              f3) {
  glUniform4f(GetUniformLocation(name), f0, f1, f2, f3);
}

void Shader::SetUniformMat4f(const std::string& name, const glm::mat4& matrix) {
  glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]);
}



enum ShaderType { NONE = -1, VERTEX = 0, FRAGMENT = 1 };

struct ShaderProgramSource Shader::ParseShader(const std::string& filepath) {
  std::ifstream     stream(filepath);
  std::string       line;
  std::stringstream ss[2];
  ShaderType        type = NONE;

  while (getline(stream, line)) {
    if (line.find("#shader") != std::string::npos) {
      if (line.find("vertex") != std::string::npos)
        type = VERTEX;
      else if (line.find("fragment") != std::string::npos)
        type = FRAGMENT;
    } else {
      ss[(int)type] << line << '\n';
    }
  }

  struct ShaderProgramSource sps = {ss[0].str(), ss[1].str()};
  return sps;
}

unsigned int Shader::CompileShader(unsigned int       type,
                                   const std::string& source) {
  unsigned int id  = glCreateShader(type);
  const char*  src = source.c_str();
  glShaderSource(id, 1, &src, nullptr);
  glCompileShader(id);

  return id;
}

unsigned int Shader::CreateShader(const std::string& vertexShader,
                                  const std::string& fragmentShader) {
  unsigned int program = glCreateProgram();
  unsigned int vs      = CompileShader(GL_VERTEX_SHADER, vertexShader);
  unsigned int fs      = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

  glAttachShader(program, vs);
  glAttachShader(program, fs);

  glLinkProgram(program);

  GLint program_linked;

  glGetProgramiv(program, GL_LINK_STATUS, &program_linked);

  glValidateProgram(program);

  glDeleteShader(vs);
  glDeleteShader(fs);

  return program;
}