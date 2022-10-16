#pragma once

#include "Shader.h"

struct TerrainSettings{
    int N = 17;
    float L = 4.0f;
};

bool operator==(const TerrainSettings& lhs, const TerrainSettings& rhs);
bool operator!=(const TerrainSettings& lhs, const TerrainSettings& rhs);

class TerrainRenderer {
public:
    TerrainRenderer();
    ~TerrainRenderer();

    void setSettings(TerrainSettings x) {m_Settings = x;}
    TerrainSettings getSettings() {return m_Settings;}

    void DisplaceVertices(float pos_x, float pos_y, float scale_xz, float scale_y);
    void BindGeometry();
    void Draw();

private:
    unsigned int m_TerrainVAO, m_TerrainVBO, m_TerrainEBO;
    std::vector<float> m_TerrainVertexData;
    std::vector<unsigned int> m_TerrainIndexData;

    unsigned int m_TerrainVAO2, m_TerrainVBO2, m_TerrainEBO2;
    std::vector<float> m_TerrainVertexData2;
    std::vector<unsigned int> m_TerrainIndexData2;
    
    unsigned int m_TerrainVAO3, m_TerrainVBO3, m_TerrainEBO3;
    std::vector<float> m_TerrainVertexData3;
    std::vector<unsigned int> m_TerrainIndexData3;

    unsigned int m_TerrainVAO4, m_TerrainVBO4, m_TerrainEBO4;
    std::vector<float> m_TerrainVertexData4;
    std::vector<unsigned int> m_TerrainIndexData4;
    
    TerrainSettings m_Settings;

    Shader m_DisplaceShader;
};
