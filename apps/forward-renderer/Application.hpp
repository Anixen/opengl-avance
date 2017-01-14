#pragma once

#include <glmlv/filesystem.hpp>
#include <glmlv/GLFWHandle.hpp>
#include <glmlv/GLProgram.hpp>
#include <glmlv/ViewController.hpp>
#include <glmlv/Image2DRGBA.hpp>
#include <tiny_obj_loader.h>

#include <map>

typedef struct {
    GLuint vb;  // vertex buffer
    int numTriangles;
    size_t material_id;
} DrawObject;


class Application
{
public:
    Application(int argc, char** argv);

    int run();
private:
    const size_t m_nWindowWidth = 1280;
    const size_t m_nWindowHeight = 720;
    glmlv::GLFWHandle m_GLFWHandle{ m_nWindowWidth, m_nWindowHeight, "Template" }; // Note: the handle must be declared before the creation of any object managing OpenGL resource (e.g. GLProgram, GLShader)

    float m_ViewControllerSpeed = 100.f;
    glmlv::ViewController m_viewController{m_GLFWHandle.window(), m_ViewControllerSpeed};

    const glmlv::fs::path m_AppPath;
    const std::string m_AppName;
    const std::string m_ImGuiIniFilename;
    const glmlv::fs::path m_ShadersRootPath;
    const glmlv::fs::path m_AssetsRootPath;

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;


    std::vector<DrawObject> m_drawObjects;

    std::string basedir = m_AssetsRootPath/ m_AppName / "models/crytek-sponza/";
    std::string inputfile = basedir + "sponza.obj";

    const GLint positionAttr_location = 0;
    const GLint normalAttr_location = 1;
    const GLint texCoordsAttr_location = 2;

    GLint m_uModelViewProjMatrix_location;
    GLint m_uModelViewMatrix_location;
    GLint m_uNormalMatrix_location;

    GLint m_uDirectionalLightDir_location;
    GLint m_uDirectionalLightColor_location;
    GLint m_uDirectionalLightIntensity_location;
    GLint m_uPointLightPosition_location;
    GLint m_uPointLightColor_location;
    GLint m_uPointLightIntensity_location;

    GLint m_uKd_location;

    GLint m_uKdSampler_location;


    GLuint m_objVBO;
    std::vector<GLuint> m_objIBOs;
    std::vector<GLuint> m_objVAOs;

    std::vector<int32_t> materialIndexes;
    std::map<std::string, GLuint> m_textures;

    GLuint m_textureSampler = 0;

    glmlv::GLProgram m_program;
};