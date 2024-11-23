#pragma once
#include <vector>
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "../Vertex.h"

class TerrainGrid {
private:
    struct GridTriangle {
        size_t index;                              // Index of the triangle in the original mesh
        glm::vec2 minBound;                        // Minimum XZ bounds
        glm::vec2 maxBound;                        // Maximum XZ bounds
        
        GridTriangle(size_t idx, const glm::vec2& min, const glm::vec2& max)
            : index(idx), minBound(min), maxBound(max) {}
    };

    struct GridCell {
        std::vector<GridTriangle> triangles;
    };

    std::vector<GridCell> grid;
    glm::vec2 terrainMin;
    glm::vec2 terrainMax;
    float cellSize;
    int gridWidth;
    int gridHeight;

public:

    TerrainGrid();
    TerrainGrid(std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, float cellSize);
    
    // Gets triangle indices for a given position
    std::vector<size_t> getTrianglesInCell(const glm::vec3& position);
    
    // Gets the terrain height at a given position
    float GetTerrainHeight(glm::vec3& position, TerrainGrid& grid,
                          std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);
};
