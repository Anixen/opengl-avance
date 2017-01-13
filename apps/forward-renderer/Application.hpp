#pragma once

#include <glmlv/filesystem.hpp>
#include <glmlv/GLFWHandle.hpp>
#include <glmlv/GLProgram.hpp>
#include <glmlv/ViewController.hpp>
#include <glmlv/Image2DRGBA.hpp>

#include <map>
#include <tiny_obj_loader.h>

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

    float m_ViewControllerSpeed = 3.f;
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

    std::string basedir = m_AssetsRootPath/ m_AppName / "models/nanosuit/";
    std::string inputfile = basedir + "nanosuit.obj";

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


    GLuint m_VBO_vertices;
    GLuint m_VBO_normals;
    GLuint m_VBO_texcoords;

    //std::vector<GLuint> m_IBOs;
    std::vector<GLuint> m_VAOs;
    std::vector<GLsizei> m_nbIndexes;

    std::map<std::string, GLuint> m_textures;


    GLuint m_nbCubeIndexes;
    GLuint m_cubeVBO = 0;
    GLuint m_cubeIBO = 0;
    GLuint m_cubeVAO = 0;
    GLuint m_cubeTextureKd = 0;

    GLuint m_nbSphereIndexes;
    GLuint m_sphereVBO = 0;
    GLuint m_sphereIBO = 0;
    GLuint m_sphereVAO = 0;
    GLuint m_sphereTextureKd = 0;
    //*/

    GLuint m_textureSampler = 0;

    glmlv::GLProgram m_program;
};