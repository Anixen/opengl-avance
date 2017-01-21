#pragma once

#include <glmlv/filesystem.hpp>
#include <glmlv/GLFWHandle.hpp>
#include <glmlv/GLProgram.hpp>
#include <glmlv/ViewController.hpp>
#include <glmlv/Image2DRGBA.hpp>
#include <tiny_obj_loader.h>

#include <map>


struct PointLight {
    glm::vec3 position;
    glm::vec3 color;
    float intensity;
    bool enabled = true;
};


class Application
{
public:
    Application(int argc, char** argv);

    int run();
private:

    void loadTexture(std::string texName);
    void updatePointLightSSBO(const std::vector<PointLight>& pointLights, const glm::mat4& viewMatrix);

    void initShadersData();

    const size_t m_nWindowWidth = 1280;
    const size_t m_nWindowHeight = 720;
    glmlv::GLFWHandle m_GLFWHandle{ m_nWindowWidth, m_nWindowHeight, "Template" }; // Note: the handle must be declared before the creation of any object managing OpenGL resource (e.g. GLProgram, GLShader)

    float m_near = 0.1f;
    float m_far = 1000.f;
    float m_ViewControllerSpeed = 80.f;
    float m_ViewControllerRotationSpeed = 0.01f;
    glmlv::ViewController m_viewController{m_GLFWHandle.window(), m_ViewControllerSpeed, m_ViewControllerRotationSpeed};

    const glmlv::fs::path m_AppPath;
    const std::string m_AppName;
    const std::string m_ImGuiIniFilename;
    const glmlv::fs::path m_ShadersRootPath;
    const glmlv::fs::path m_AssetsRootPath;

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string basedir = m_AssetsRootPath/ m_AppName / "models/crytek-sponza/";
    std::string inputfile = basedir + "sponza.obj";


    // Geometry program locations
    glmlv::GLProgram m_geometryProgram;
    const GLint positionAttr_location_geometry = 0;
    const GLint normalAttr_location = 1;
    const GLint tangentAttr_location = 2;
    const GLint texCoordsAttr_location = 3;

    GLint m_uModelViewProjMatrix_location;
    GLint m_uModelViewMatrix_location;
    GLint m_uNormalMatrix_location;

    GLint m_uKaSampler_location;
    GLint m_uKdSampler_location;
    GLint m_uKsSampler_location;
    GLint m_uNsSampler_location;
    GLint m_udSampler_location;
    GLint m_uNormalSampler_location;

    GLint m_uKaMap_location;
    GLint m_uKdMap_location;
    GLint m_uKsMap_location;
    GLint m_uNsMap_location;
    GLint m_udMap_location;
    GLint m_uNormalMap_location;

    GLint m_uKa_location;
    GLint m_uKd_location;
    GLint m_uKs_location;
    GLint m_uNs_location;
    GLint m_ud_location;
    // Normal is sent through attributes

    GLuint m_objVBO;
    std::vector<GLuint> m_objIBOs;
    std::vector<GLuint> m_objVAOs;

    std::vector<int32_t> materialIndexes;
    std::map<std::string, GLuint> m_textures;

    GLuint m_textureSampler;

    // For Deferred Shading
    enum GBufferTextureType
    {
        GPosition = 0,
        GNormal,
        GAmbient,
        GDiffuse,
        GGlossyShininess,
        GDepth, // On doit créer une texture de depth mais on écrit pas directement dedans dans le FS. OpenGL le fait pour nous (et l'utilise).
        GBufferTextureCount
    };

    const GLenum m_GBufferTextureFormat[GBufferTextureCount] = {
            GL_RGB32F, GL_RGB32F, GL_RGB32F,
            GL_RGB32F, GL_RGBA32F, GL_DEPTH_COMPONENT32F };

    GLuint m_GBufferTextures[GBufferTextureCount];
    GLuint m_FBO;

    const char * m_GBufferTexNames[GBufferTextureCount] = { "position", "normal", "ambient", "diffuse", "glossyShininess", "depth" };
    GBufferTextureType m_CurrentlyDisplayed = GDiffuse;
    bool m_displayGBuffer = false;

    GLuint m_displayVB0;
    GLuint m_displayVAO;

    // Shading program locations
    glmlv::GLProgram m_shadingProgram;

    const GLint positionAttr_location_shading = 0;

    const GLint pointLightPosition_binding = 4;
    const GLint pointLightColor_binding = 5;
    const GLint pointLightIntensity_binding = 6;
    const GLint pointLightEnabled_binding = 7;

    GLint m_uDirectionalLightDir_location;
    GLint m_uDirectionalLightColor_location;
    GLint m_uDirectionalLightIntensity_location;

    GLint m_uEnablePointLights_location;
    GLint m_uNbPointLights_location;

    GLuint m_pointLightPositionSSBO;
    GLuint m_pointLightColorSSBO;
    GLuint m_pointLightIntensitySSBO;
    GLuint m_pointLightEnabledSSBO;

    GLint m_uGPositionSampler_location;
    GLint m_uGNormalSampler_location;
    GLint m_uGAmbientSampler_location;
    GLint m_uGDiffuseSampler_location;
    GLint m_uGlossyShininessSampler_location;

    // Depth program locations
    glmlv::GLProgram m_depthProgram;

    GLint m_uAdjustment_location_depth;
    GLint m_uGDepthSampler_location;

    // Position program locations
    glmlv::GLProgram m_positionProgram;

    GLint m_uAdjustment_location_position;
    GLint m_uGPositionSampler_location_position;

    GLuint m_depthTexture;
};