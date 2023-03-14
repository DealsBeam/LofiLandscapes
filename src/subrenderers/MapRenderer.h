#pragma once

#include "Shader.h"
#include "GLUtils.h"
#include "TextureEditor.h"

struct ScaleSettings{
    float ScaleXZ = 100.0f;
    float ScaleY = 20.0f;
};

struct AOSettings{
    int Samples = 16;
    float R = 0.01;
};

struct ShadowmapSettings{
    int MinLevel = 5;
    int StartCell = 32;
    int MipOffset = 0;

    float NudgeFac = 1.02f;

    bool Soft = true;
    float Sharpness = 1.0f;
};

class MapRenderer {
public:
    MapRenderer();
    ~MapRenderer();

    void Init(int height_res, int shadow_res, int wrap_type);
    void Update(const glm::vec3& sun_dir);

    void BindHeightmap(int id=0);
    void BindNormalmap(int id=0);
    void BindShadowmap(int id=0);
    void BindMaterialmap(int id=0);

    void ImGuiTerrain(bool &open, bool update_shadows);
    void ImGuiShadowmap(bool &open, bool update_shadows);
    void ImGuiMaterials(bool& open);
    void RequestShadowUpdate();

    bool GeometryShouldUpdate();

    ScaleSettings getScaleSettings() {return m_ScaleSettings;}

private:

    enum UpdateFlags {
        None     =  0,
        Height   = (1 << 0),
        Normal   = (1 << 1),
        Shadow   = (1 << 2),
        Material = (1 << 3)
    };

    Texture m_Heightmap, m_Normalmap, m_Shadowmap, m_Materialmap; 
    TextureEditor m_HeightEditor, m_MaterialEditor;
    Shader m_NormalmapShader, m_ShadowmapShader;

    Shader m_MipShader;
    
    ScaleSettings     m_ScaleSettings;
    ShadowmapSettings m_ShadowSettings;
    AOSettings        m_AOSettings;

    int m_UpdateFlags = None;

    int m_MipLevels = 0;
    
    void UpdateHeight();
    void UpdateNormal();
    void UpdateShadow(const glm::vec3& sun_dir);
    void UpdateMaterial();

    void GenMaxMips();
};

bool operator==(const ScaleSettings& lhs, const ScaleSettings& rhs);
bool operator!=(const ScaleSettings& lhs, const ScaleSettings& rhs);

bool operator==(const ShadowmapSettings& lhs, const ShadowmapSettings& rhs);
bool operator!=(const ShadowmapSettings& lhs, const ShadowmapSettings& rhs);

bool operator==(const AOSettings& lhs, const AOSettings& rhs);
bool operator!=(const AOSettings& lhs, const AOSettings& rhs);
