#include "TerrainGrid.h"
#include <algorithm>
#include <iostream>
#include "../Math.h"

Math terrainMath;

TerrainGrid::TerrainGrid()
{
}

TerrainGrid::TerrainGrid(std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, float LcellSize)
{
    this->cellSize = LcellSize;
    
    // Calculate terrain bounds
    terrainMin = glm::vec2(std::numeric_limits<float>::max());
    terrainMax = glm::vec2(std::numeric_limits<float>::lowest());

    for (const auto& vertex : vertices) {
        terrainMin.x = std::min(terrainMin.x, vertex.Position.x);
        terrainMin.y = std::min(terrainMin.y, vertex.Position.z);
        terrainMax.x = std::max(terrainMax.x, vertex.Position.x);
        terrainMax.y = std::max(terrainMax.y, vertex.Position.z);
    }

    std::cout << "Terrain bounds: Min(" << terrainMin.x << ", " << terrainMin.y 
              << "), Max(" << terrainMax.x << ", " << terrainMax.y << ")\n";

    // Add padding
    float padding = cellSize * 0.5f;
    terrainMin -= glm::vec2(padding);
    terrainMax += glm::vec2(padding);

    // Calculate grid dimensions
    gridWidth = static_cast<int>(ceil((terrainMax.x - terrainMin.x) / cellSize));
    gridHeight = static_cast<int>(ceil((terrainMax.y - terrainMin.y) / cellSize));

    std::cout << "Grid dimensions: " << gridWidth << "x" << gridHeight << " cells\n";

    // Initialize grid
    grid.resize(gridWidth * gridHeight);

    // Populate grid with triangles
    for (size_t i = 0; i < indices.size(); i += 3) {
        const glm::vec3& v0 = vertices[indices[i]].Position;
        const glm::vec3& v1 = vertices[indices[i + 1]].Position;
        const glm::vec3& v2 = vertices[indices[i + 2]].Position;

        // Calculate triangle bounds in XZ plane
        glm::vec2 triMin(
            std::min({v0.x, v1.x, v2.x}),
            std::min({v0.z, v1.z, v2.z})
        );
        glm::vec2 triMax(
            std::max({v0.x, v1.x, v2.x}),
            std::max({v0.z, v1.z, v2.z})
        );

        // Convert to grid coordinates
        int startX = static_cast<int>((triMin.x - terrainMin.x) / cellSize);
        int startY = static_cast<int>((triMin.y - terrainMin.y) / cellSize);
        int endX = static_cast<int>((triMax.x - terrainMin.x) / cellSize) + 1;
        int endY = static_cast<int>((triMax.y - terrainMin.y) / cellSize) + 1;

        // Clamp to grid bounds
        startX = std::max(0, std::min(startX, gridWidth - 1));
        startY = std::max(0, std::min(startY, gridHeight - 1));
        endX = std::max(0, std::min(endX, gridWidth));
        endY = std::max(0, std::min(endY, gridHeight));

        // Create GridTriangle
        GridTriangle gridTri(i / 3, triMin, triMax);
        
        // Add triangle to overlapping cells
        for (int y = startY; y < endY; ++y) {
            for (int x = startX; x < endX; ++x) {
                grid[y * gridWidth + x].triangles.push_back(gridTri);
            }
        }
    }

    // Debug output
    size_t totalTriRefs = 0;
    for (const auto& cell : grid) {
        totalTriRefs += cell.triangles.size();
    }
    std::cout << "Total triangle references in grid: " << totalTriRefs << "\n";
}

std::vector<size_t> TerrainGrid::getTrianglesInCell(const glm::vec3& position) 
{
    // Convert world position to grid coordinates
    int x = static_cast<int>((position.x - terrainMin.x) / cellSize);
    int y = static_cast<int>((position.z - terrainMin.y) / cellSize);
    
    std::vector<size_t> result;
    
    // Check if position is within grid bounds
    if (x < 0 || x >= gridWidth || y < 0 || y >= gridHeight) {
        return result;
    }

    //std::cout << "Checking grid cell (" << x << ", " << y << ")\n";

    // Get triangles from current cell
    const auto& cell = grid[y * gridWidth + x];
    
    // Convert position to 2D for bounds checking
    glm::vec2 pos2D(position.x, position.z);
    
    // Check each triangle in the cell
    for (const auto& tri : cell.triangles) {
        // Quick AABB check before adding triangle
        if (pos2D.x >= tri.minBound.x && pos2D.x <= tri.maxBound.x &&
            pos2D.y >= tri.minBound.y && pos2D.y <= tri.maxBound.y) {
            result.push_back(tri.index);
        }
    }

    //std::cout << "Found " << result.size() << " potential triangles in cell\n";
    return result;
}

float TerrainGrid::GetTerrainHeight(glm::vec3& position, TerrainGrid& grid,
    std::vector<Vertex>& vertices, std::vector<unsigned int>& indices)
{
    auto triangles = grid.getTrianglesInCell(position);
    
    float highestPoint = -1.0f;
    for (size_t triIndex : triangles) {
        size_t i0 = indices[triIndex * 3];
        size_t i1 = indices[triIndex * 3 + 1];
        size_t i2 = indices[triIndex * 3 + 2];

        float height = terrainMath.barycentricCoordinates(
            vertices[i0].Position,
            vertices[i1].Position,
            vertices[i2].Position,
            position
        );

        if (height > highestPoint) {
            highestPoint = height;
        }
    }

    return highestPoint;
}

glm::vec3 TerrainGrid::GetTerrainNormal(glm::vec3& position, TerrainGrid& grid,
    std::vector<Vertex>& vertices, std::vector<unsigned int>& indices)
{
    auto triangles = grid.getTrianglesInCell(position);

    //Default normal
    if (triangles.empty()) {
        return glm::vec3(0.0f, 1.0f, 0.0f);
    }

    //Find triangle
    for (size_t triIndex : triangles) {
        size_t i0 = indices[triIndex * 3];
        size_t i1 = indices[triIndex * 3 + 1];
        size_t i2 = indices[triIndex * 3 + 2];

        const glm::vec3& v0 = vertices[i0].Position;
        const glm::vec3& v1 = vertices[i1].Position;
        const glm::vec3& v2 = vertices[i2].Position;

        float height = terrainMath.barycentricCoordinates(v0, v1, v2, position);

        if (height != -1) {

            glm::vec3 edge1 = v1 - v0;
            glm::vec3 edge2 = v2 - v0;
            glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));


            if (normal.y < 0) {
                normal = -normal;
            }

            return normal;
        }
    }

    return glm::vec3(0.0f, 1.0f, 0.0f);
}