#pragma once

#include <vector>
#include "glm/glm.hpp";

#include "InternalTypes.h"
#include "Model.h"

class Sphere: public Model {
public:
  Sphere();
  Sphere(float radius, int longtitude, int latitude);
  std::vector<Vertex>       GetVertices() const override;
  std::vector<unsigned int> GetIndices() const override;
  void                      GenerateData();
  void                      Set(float radius, int longtitude, int latitude);
  void                      ClearData();

private:
  float m_Radius;
  int   m_StackCount;
  int   m_SectorCount;

  std::vector<Vertex>       m_Vertices;
  std::vector<unsigned int> m_Indices;

  void GenerateVertices();
  void GenerateIndices();
};