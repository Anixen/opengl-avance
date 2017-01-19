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

    const size_t m_nWindowWidth = 1280;
    const size_t m_nWindowHeight = 720;
    glmlv::GLFWHandle m_GLFWHandle{ m_nWindowWidth, m_nWindowHeight, "Template" }; // Note: the handle must be declared before the creation of any object managing OpenGL resource (e.g. GLProgram, GLShader)

    float m_ViewControllerSpeed = 100.f;
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

    const GLint positionAttr_location = 0;
    const GLint normalAttr_location = 1;
    const GLint tangentAttr_location = 2;
    const GLint texCoordsAttr_location = 3;

    const GLint pointLightPosition_binding = 4;
    const GLint pointLightColor_binding = 5;
    const GLint pointLightIntensity_binding = 6;
    const GLint pointLightEnabled_binding = 7;

    GLint m_uModelViewProjMatrix_location;
    GLint m_uModelViewMatrix_location;
    GLint m_uNormalMatrix_location;

    GLint m_uKa_location;
    GLuint m_KaSampler = 0;
    GLint m_uKaSampler_location;
    GLint m_uKaMap_location;

    GLint m_uKd_location;
    GLuint m_KdSampler = 0;
    GLint m_uKdSampler_location;
    GLint m_uKdMap_location;

    GLint m_uKs_location;
    GLuint m_KsSampler = 0;
    GLint m_uKsSampler_location;
    GLint m_uKsMap_location;

    GLint m_uNs_location;
    GLuint m_NsSampler = 0;
    GLint m_uNsSampler_location;
    GLint m_uNsMap_location;

    GLint m_ud_location;
    GLuint m_dSampler = 0;
    GLint m_udSampler_location;
    GLint m_udMap_location;

    GLuint m_NormalSampler = 0;
    GLint m_uNormalSampler_location;
    GLint m_uNormalMap_location;

    GLuint m_objVBO;
    std::vector<GLuint> m_objIBOs;
    std::vector<GLuint> m_objVAOs;

    GLint m_uDirectionalLightDir_location;
    GLint m_uDirectionalLightColor_location;
    GLint m_uDirectionalLightIntensity_location;

    GLint m_uEnablePointLights_location;
    GLint m_uNbPointLights_location;
    GLuint m_pointLightPositionSSBO;
    GLuint m_pointLightColorSSBO;
    GLuint m_pointLightIntensitySSBO;
    GLuint m_pointLightEnabledSSBO;

    std::vector<int32_t> materialIndexes;
    std::map<std::string, GLuint> m_textures;

    glmlv::GLProgram m_geometryProgram;


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
};