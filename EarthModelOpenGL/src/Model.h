#pragma once
#include <vector>

#include "InternalTypes.h"

class Model {
public:
  virtual std::vector<Vertex>       GetVertices() const = 0;
  virtual std::vector<unsigned int> GetIndices() const  = 0;

private:
  virtual void GenerateVertices() = 0;
  virtual void GenerateIndices()  = 0;
};