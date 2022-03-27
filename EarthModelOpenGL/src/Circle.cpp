#define _USE_MATH_DEFINES
#include "Circle.h"

#include <cmath>

Circle::Circle(): m_Radius(1.0f), m_SectorCount(3) {}

Circle::Circle(float radius, int sectorCount):
    m_Radius(radius), m_SectorCount(sectorCount) {
  GenerateData();
}

std::vector<Vertex> Circle::GetVertices() const { return m_Vertices; }

std::vector<unsigned int> Circle::GetIndices() const { return m_Indices; }

void Circle::GenerateData() {
  m_Vertices.clear();
  m_Indices.clear();
  GenerateVertices();
  GenerateIndices();
}

void Circle::Set(float radius, int sectorCount) {
  *this = Circle(radius, sectorCount);
}

void Circle::ClearData() {
  m_Vertices.clear();
  m_Indices.clear();
}

void Circle::GenerateVertices() {
  float x, y, z;
  float nx, ny, nz;
  float s, t;
  float lengthInv = 1.0f / m_Radius;

  float sectorStep = 2 * (float)M_PI / m_SectorCount;
  float sectorAngle;


  for (int j = 0; j < m_SectorCount; ++j) {
    sectorAngle = j * sectorStep;

    x  = m_Radius * cosf(sectorAngle);
    y  = m_Radius * sinf(sectorAngle);
    z  = 0;
    nx = x * lengthInv;
    ny = y * lengthInv;
    nz = 0;
    s  = 0;
    t  = 0;

    m_Vertices.push_back(
        {glm::vec3(x, y, z), glm::vec3(nx, ny, nz), glm::vec2(s, t)});
  }
}

void Circle::GenerateIndices() {
  for (int i = 0; i < m_SectorCount; ++i) {
    m_Indices.push_back(i % m_Vertices.size());
    m_Indices.push_back((i + 1) % m_Vertices.size());
  }
}
