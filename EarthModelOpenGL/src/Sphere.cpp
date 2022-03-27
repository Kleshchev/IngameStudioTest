#define _USE_MATH_DEFINES
#include "Sphere.h"

#include <cmath>

Sphere::Sphere(): m_Radius(1.0f), m_StackCount(3), m_SectorCount(3) {}

Sphere::Sphere(float radius, int longtitude, int latitude) {
  this->m_Radius      = radius;
  this->m_StackCount  = longtitude;
  this->m_SectorCount = latitude;
  GenerateData();
}

std::vector<Vertex> Sphere::GetVertices() const { return m_Vertices; }

std::vector<unsigned int> Sphere::GetIndices() const { return m_Indices; }

void Sphere::GenerateData() {
  m_Vertices.clear();
  m_Indices.clear();
  GenerateVertices();
  GenerateIndices();
}

void Sphere::Set(float radius, int longtitude, int latitude) {
  *this = Sphere(radius, longtitude, latitude);
}

void Sphere::ClearData() {
  m_Vertices.clear();
  m_Indices.clear();
}

void Sphere::GenerateVertices() {
  float x, y, z, xy;
  float nx, ny, nz, lengthInv = 1.0f / m_Radius;
  float s, t;

  float sectorStep = 2 * (float)M_PI / m_SectorCount;
  float stackStep  = (float)M_PI / m_StackCount;
  float sectorAngle, stackAngle;

  for (int i = 0; i <= m_StackCount; ++i) {
    stackAngle = (float)M_PI / 2 - i * stackStep;
    xy         = m_Radius * cosf(stackAngle);
    z          = m_Radius * sinf(stackAngle);

    for (int j = 0; j <= m_SectorCount; ++j) {
      sectorAngle = j * sectorStep;

      x  = xy * cosf(sectorAngle);
      y  = xy * sinf(sectorAngle);
      nx = x * lengthInv;
      ny = y * lengthInv;
      nz = z * lengthInv;
      s  = (float)j / m_SectorCount;
      t  = (float)i / m_StackCount;

      m_Vertices.push_back(
          {glm::vec3(x, y, z), glm::vec3(nx, ny, nz), glm::vec2(s, t)});
    }
  }
}

void Sphere::GenerateIndices() {
  int k1, k2;
  for (int i = 0; i < m_StackCount; ++i) {
    k1 = i * (m_SectorCount + 1);
    k2 = k1 + m_SectorCount + 1;

    for (int j = 0; j < m_SectorCount; ++j, ++k1, ++k2) {
      if (i != 0) {
        m_Indices.push_back(k1);
        m_Indices.push_back(k2);
        m_Indices.push_back(k1 + 1);
      }

      if (i != (m_StackCount - 1)) {
        m_Indices.push_back(k1 + 1);
        m_Indices.push_back(k2);
        m_Indices.push_back(k2 + 1);
      }
    }
  }
}
