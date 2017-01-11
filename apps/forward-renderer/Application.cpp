#include "Application.hpp"

#include <iostream>

#include <imgui.h>
#include <glmlv/imgui_impl_glfw_gl3.hpp>
#include <glmlv/simple_geometry.hpp>
#include <glmlv/Image2DRGBA.hpp>

using namespace glmlv;

int Application::run()
{
    float clearColor[3] = { 0.3, 0.3, 0.3 };
    glClearColor(clearColor[0], clearColor[1], clearColor[2], 1.f);

    glm::vec3 directionalLightDir = {1, 1, 0};
    glm::vec3 directionalLightColor = {0, 0, 0};
    float directionalLightIntensity = 1.f;
    glm::vec3 pointLightPosition = {5, 0, 3};
    glm::vec3 pointLightColor = {1, 1, 1};
    float pointLightIntensity = 1.f;

    glm::vec3 cubeKd = {1, 0, 0};
    glm::vec3 sphereKd = {0, 1, 0};

    // Loop until the user closes the window
    for (auto iterationCount = 0u; !m_GLFWHandle.shouldClose(); ++iterationCount)
    {
        const auto seconds = glfwGetTime();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Put here rendering code

        glm::mat4 viewMatrix = m_viewController.getViewMatrix();
        glm::mat4 projMatrix = glm::perspective(70.f, (float) m_nWindowWidth / m_nWindowHeight, 0.1f, 100.f);

        // Lighting
        glm::vec4 viewDirectionalLightDir = viewMatrix * glm::vec4(directionalLightDir[0], directionalLightDir[1], directionalLightDir[2], 0);
        viewDirectionalLightDir = glm::normalize(viewDirectionalLightDir);
        glm::vec4 viewPointLightPosition = viewMatrix * glm::vec4(pointLightPosition[0], pointLightPosition[1], pointLightPosition[2], 1);

        glUniform3f(m_uDirectionalLightDir_location, viewDirectionalLightDir[0], viewDirectionalLightDir[1], viewDirectionalLightDir[2]);
        glUniform3f(m_uDirectionalLightColor_location, directionalLightColor[0], directionalLightColor[1], directionalLightColor[2]);
        glUniform1f(m_uDirectionalLightIntensity_location, directionalLightIntensity);
        glUniform3f(m_uPointLightPosition_location, viewPointLightPosition[0], viewPointLightPosition[1], viewPointLightPosition[2]);
        glUniform3f(m_uPointLightColor_location, pointLightColor[0], pointLightColor[1], pointLightColor[2]);
        glUniform1f(m_uPointLightIntensity_location, pointLightIntensity);

        // Models
        glm::mat4 ModelViewMatrix;
        glm::mat4 NormalMatrix;
        glm::mat4 ModelViewProjMatrix;

        glActiveTexture(GL_TEXTURE0);
        glUniform1i(m_uKdSampler_location, 0); // Set the uniform to 0 because we use texture unit 0
        glBindSampler(0, m_textureSampler); // Tell to OpenGL what sampler we want to use on this texture unit
        glBindTextureUnit(0, m_cubeTextureKd);
        glBindVertexArray(m_cubeVAO);

        glm::mat4 cubeModelMatrix = glm::mat4();
        cubeModelMatrix = glm::translate(cubeModelMatrix, glm::vec3(0, 0, -5));

        ModelViewMatrix = viewMatrix * cubeModelMatrix;
        NormalMatrix = glm::transpose(glm::inverse(ModelViewMatrix));
        ModelViewProjMatrix = projMatrix * ModelViewMatrix;
        glUniformMatrix4fv(m_uModelViewProjMatrix_location, 1, GL_FALSE, &ModelViewProjMatrix[0][0]);
        glUniformMatrix4fv(m_uModelViewMatrix_location, 1, GL_FALSE, &ModelViewMatrix[0][0]);
        glUniformMatrix4fv(m_uNormalMatrix_location, 1, GL_FALSE, &NormalMatrix[0][0]);

        glUniform3f(m_uKd_location, cubeKd[0], cubeKd[1], cubeKd[2]);

        glDrawElements(GL_TRIANGLES, m_nbCubeIndexes, GL_UNSIGNED_INT, nullptr);
        //glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);


        glActiveTexture(GL_TEXTURE0);
        glUniform1i(m_uKdSampler_location, 0); // Set the uniform to 0 because we use texture unit 0
        glBindSampler(0, m_textureSampler); // Tell to OpenGL what sampler we want to use on this texture unit
        glBindTextureUnit(0, m_sphereTextureKd);
        glBindVertexArray(m_sphereVAO);

        glm::mat4 sphereModelMatrix = glm::mat4();
        sphereModelMatrix = glm::translate(cubeModelMatrix, glm::vec3(1, 0, -3));

        ModelViewMatrix = viewMatrix * sphereModelMatrix;
        NormalMatrix = glm::transpose(glm::inverse(ModelViewMatrix));
        ModelViewProjMatrix = projMatrix * ModelViewMatrix;
        glUniformMatrix4fv(m_uModelViewProjMatrix_location, 1, GL_FALSE, &ModelViewProjMatrix[0][0]);
        glUniformMatrix4fv(m_uModelViewMatrix_location, 1, GL_FALSE, &ModelViewMatrix[0][0]);
        glUniformMatrix4fv(m_uNormalMatrix_location, 1, GL_FALSE, &NormalMatrix[0][0]);

        glUniform3f(m_uKd_location, sphereKd[0], sphereKd[1], sphereKd[2]);

        glDrawElements(GL_TRIANGLES, m_nbSphereIndexes, GL_UNSIGNED_INT, nullptr);
        //glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        // GUI code:
        ImGui_ImplGlfwGL3_NewFrame();

        {
            ImGui::Begin("GUI");
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::ColorEditMode(ImGuiColorEditMode_RGB);
            if (ImGui::ColorEdit3("clearColor", clearColor)) {
                glClearColor(clearColor[0], clearColor[1], clearColor[2], 1.f);
            }

            ImGui::DragFloat3("directionalLightDir", &directionalLightDir[0]);
            ImGui::ColorEdit3("directionalLightColor", &directionalLightColor[0]);
            ImGui::DragFloat("directionalLightIntensity", &directionalLightIntensity);
            ImGui::DragFloat3("pointLightPosition", &pointLightPosition[0]);
            ImGui::ColorEdit3("pointLightColor", &pointLightColor[0]);
            ImGui::DragFloat("pointLightIntensity", &pointLightIntensity);

            ImGui::ColorEdit3("cubeKd", &cubeKd[0]);
            ImGui::ColorEdit3("sphereKd", &sphereKd[0]);

            ImGui::End();
        }

        const auto viewportSize = m_GLFWHandle.framebufferSize();
        glViewport(0, 0, viewportSize.x, viewportSize.y);
        ImGui::Render();

        /* Poll for and process events */
        glfwPollEvents();

        /* Swap front and back buffers*/
        m_GLFWHandle.swapBuffers();

        auto ellapsedTime = glfwGetTime() - seconds;
        auto guiHasFocus = ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard;
        if (!guiHasFocus) {
            m_viewController.update(ellapsedTime);
        }
    }

    return 0;
}

Application::Application(int argc, char** argv):
    m_AppPath { glmlv::fs::path{ argv[0] } },
    m_AppName { m_AppPath.stem().string() },
    m_ImGuiIniFilename { m_AppName + ".imgui.ini" },
    m_ShadersRootPath { m_AppPath.parent_path() / "shaders" },
    m_AssetsRootPath{ m_AppPath.parent_path() / "assets"}

{
    ImGui::GetIO().IniFilename = m_ImGuiIniFilename.c_str(); // At exit, ImGUI will store its windows positions in this file

    const GLint vboBindingIndex = 0; // Arbitrary choice between 0 and glGetIntegerv(GL_MAX_VERTEX_ATTRIB_BINDINGS)7


    SimpleGeometry cube = makeCube();
    m_nbCubeIndexes = cube.indexBuffer.size();

    //readImage()

    glCreateBuffers(1, &m_cubeVBO);
    glNamedBufferStorage(m_cubeVBO, cube.vertexBuffer.size() * sizeof(Vertex3f3f2f), cube.vertexBuffer.data(), 0);

    glCreateBuffers(1, &m_cubeIBO);
    glNamedBufferStorage(m_cubeIBO, cube.indexBuffer.size() * sizeof(uint32_t), cube.indexBuffer.data(), 0);
    glCreateVertexArrays(1, &m_cubeVAO);


    SimpleGeometry sphere = makeSphere(16);
    m_nbSphereIndexes = sphere.indexBuffer.size();

    glCreateBuffers(1, &m_sphereVBO);
    glNamedBufferStorage(m_sphereVBO, sphere.vertexBuffer.size() * sizeof(Vertex3f3f2f), sphere.vertexBuffer.data(), 0);

    glCreateBuffers(1, &m_sphereIBO);
    glNamedBufferStorage(m_sphereIBO, sphere.indexBuffer.size() * sizeof(uint32_t), sphere.indexBuffer.data(), 0);
    glCreateVertexArrays(1, &m_sphereVAO);

    glActiveTexture(GL_TEXTURE0);
    {
        auto image = glmlv::readImage(m_AssetsRootPath / m_AppName / "textures" / "CloudMap.jpg");

        glGenTextures(1, &m_cubeTextureKd);
        glBindTexture(GL_TEXTURE_2D, m_cubeTextureKd);
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, image.width(), image.height());
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image.width(), image.height(), GL_RGBA, GL_UNSIGNED_BYTE, image.data());
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    {
        auto image = glmlv::readImage(m_AssetsRootPath / m_AppName / "textures" / "EarthMap.jpg");

        glGenTextures(1, &m_sphereTextureKd);
        glBindTexture(GL_TEXTURE_2D, m_sphereTextureKd);
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, image.width(), image.height());
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image.width(), image.height(), GL_RGBA, GL_UNSIGNED_BYTE, image.data());
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    // Note: no need to bind a sampler for modifying it: the sampler API is already direct_state_access
    glGenSamplers(1, &m_textureSampler);
    glSamplerParameteri(m_textureSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glSamplerParameteri(m_textureSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    glEnable(GL_DEPTH_TEST);


    // Here we load and compile shaders from the library
    m_program = glmlv::compileProgram({ m_ShadersRootPath / m_AppName / "forward.vs.glsl", m_ShadersRootPath / m_AppName / "forward.fs.glsl" });
    m_program.use();

    m_uModelViewProjMatrix_location = glGetUniformLocation(m_program.glId(), "uModelViewProjMatrix");
    m_uModelViewMatrix_location = glGetUniformLocation(m_program.glId(), "uModelViewMatrix");
    m_uNormalMatrix_location = glGetUniformLocation(m_program.glId(), "uNormalMatrix");

    m_uDirectionalLightDir_location = glGetUniformLocation(m_program.glId(), "uDirectionalLightDir");
    m_uDirectionalLightColor_location = glGetUniformLocation(m_program.glId(), "uDirectionalLightColor");
    m_uDirectionalLightIntensity_location = glGetUniformLocation(m_program.glId(), "uDirectionalLightIntensity");
    m_uPointLightPosition_location = glGetUniformLocation(m_program.glId(), "uPointLightPosition");
    m_uPointLightColor_location = glGetUniformLocation(m_program.glId(), "uPointLightColor");
    m_uPointLightIntensity_location = glGetUniformLocation(m_program.glId(), "uPointLightIntensity");

    m_uKd_location = glGetUniformLocation(m_program.glId(), "uKd");

    m_uKdSampler_location = glGetUniformLocation(m_program.glId(), "uKdSampler");

    // Here we use glGetAttribLocation(program, attribname) to obtain attrib locations; We could also directly use locations if they are set in the vertex shader (cf. triangle app)
//    const GLint positionAttr_location = glGetAttribLocation(m_program.glId(), "aPosition");
//    const GLint normalAttr_location = glGetAttribLocation(m_program.glId(), "aNormal");
//    const GLint texCoordsAttr_location = glGetAttribLocation(m_program.glId(), "aTexCoords");
    const GLint positionAttr_location = 0;
    const GLint normalAttr_location = 1;
    const GLint texCoordsAttr_location = 2;

    glVertexArrayVertexBuffer(m_cubeVAO, vboBindingIndex, m_cubeVBO, vboBindingIndex, sizeof(Vertex3f3f2f));

    glVertexArrayAttribBinding(m_cubeVAO, positionAttr_location, vboBindingIndex);
    glEnableVertexArrayAttrib(m_cubeVAO, positionAttr_location);
    glVertexArrayAttribFormat(m_cubeVAO, positionAttr_location, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex3f3f2f, position));

    glVertexArrayAttribBinding(m_cubeVAO, normalAttr_location, vboBindingIndex);
    glEnableVertexArrayAttrib(m_cubeVAO, normalAttr_location);
    glVertexArrayAttribFormat(m_cubeVAO, normalAttr_location, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex3f3f2f, normal));

    glVertexArrayAttribBinding(m_cubeVAO, texCoordsAttr_location, vboBindingIndex);
    glEnableVertexArrayAttrib(m_cubeVAO, texCoordsAttr_location);
    glVertexArrayAttribFormat(m_cubeVAO, texCoordsAttr_location, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex3f3f2f, texCoords));

    glVertexArrayElementBuffer(m_cubeVAO, m_cubeIBO);


    glVertexArrayVertexBuffer(m_sphereVAO, vboBindingIndex, m_sphereVBO, vboBindingIndex, sizeof(Vertex3f3f2f));

    glVertexArrayAttribBinding(m_sphereVAO, positionAttr_location, vboBindingIndex);
    glEnableVertexArrayAttrib(m_sphereVAO, positionAttr_location);
    glVertexArrayAttribFormat(m_sphereVAO, positionAttr_location, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex3f3f2f, position));

    glVertexArrayAttribBinding(m_sphereVAO, normalAttr_location, vboBindingIndex);
    glEnableVertexArrayAttrib(m_sphereVAO, normalAttr_location);
    glVertexArrayAttribFormat(m_sphereVAO, normalAttr_location, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex3f3f2f, normal));

    glVertexArrayAttribBinding(m_sphereVAO, texCoordsAttr_location, vboBindingIndex);
    glEnableVertexArrayAttrib(m_sphereVAO, texCoordsAttr_location);
    glVertexArrayAttribFormat(m_sphereVAO, texCoordsAttr_location, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex3f3f2f, texCoords));

    glVertexArrayElementBuffer(m_sphereVAO, m_sphereIBO);
}