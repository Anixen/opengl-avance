#include "Application.hpp"

#include <iostream>

#include <imgui.h>
#include <glmlv/imgui_impl_glfw_gl3.hpp>
#include <glmlv/simple_geometry.hpp>
#include <glmlv/Image2DRGBA.hpp>
#include <tiny_obj_loader.h>
//#include <tiny_obj_loader.h>



using namespace glmlv;


int Application::run()
{
    float clearColor[3] = { 0.3, 0.3, 0.3 };
    glClearColor(clearColor[0], clearColor[1], clearColor[2], 1.f);

    glm::vec3 directionalLightDir = {1, 1, 2};
    glm::vec3 directionalLightColor = {1, 1, 1};
    float directionalLightIntensity = 1.f;

    glm::vec3 pointLightPosition = {5, 0, 3};
    glm::vec3 pointLightColor = {1, 1, 1};
    float pointLightIntensity = -256.f;

    // Loop until the user closes the window
    for (auto iterationCount = 0u; !m_GLFWHandle.shouldClose(); ++iterationCount)
    {
        const auto seconds = glfwGetTime();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Put here rendering code

        glm::mat4 viewMatrix = m_viewController.getViewMatrix();
        glm::mat4 projMatrix = glm::perspective(70.f, (float) m_nWindowWidth / m_nWindowHeight, 0.1f, 10000.f);

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

        // Display the model
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(m_uKdSampler_location, 0); // Set the uniform to 0 because we use texture unit 0
        glBindSampler(0, m_textureSampler); // Tell to OpenGL what sampler we want to use on this texture unit

        for(size_t s = 0; s < m_objVAOs.size(); ++s) {
            glBindVertexArray(m_objVAOs[s]);


            // Get material properties
            int32_t matIndex = materialIndexes[s];
            std::string kdTexname = materials[matIndex].diffuse_texname;
            glm::vec3 kd = {materials[matIndex].diffuse[0], materials[matIndex].diffuse[0], materials[matIndex].diffuse[0]};
            glBindTextureUnit(0, m_textures[kdTexname]);

            glm::mat4 modelMatrix = glm::mat4();
            //modelMatrix = glm::translate(modelMatrix, glm::vec3(1, 0, -3));
            //modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1, 0.1, 0.1));

            glm::mat4 modelViewMatrix = viewMatrix * modelMatrix;
            glm::mat4 normalMatrix = glm::transpose(glm::inverse(modelViewMatrix));
            glm::mat4 modelViewProjMatrix = projMatrix * modelViewMatrix;
            glUniformMatrix4fv(m_uModelViewProjMatrix_location, 1, GL_FALSE, &modelViewProjMatrix[0][0]);
            glUniformMatrix4fv(m_uModelViewMatrix_location, 1, GL_FALSE, &modelViewMatrix[0][0]);
            glUniformMatrix4fv(m_uNormalMatrix_location, 1, GL_FALSE, &normalMatrix[0][0]);

            glUniform3f(m_uKd_location, kd[0], kd[1], kd[2]);

            glDrawElements(GL_TRIANGLES, shapes[s].mesh.indices.size(), GL_UNSIGNED_INT, nullptr);
            glBindVertexArray(0);
        }

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

            if(ImGui::DragFloat("cameraSpeed", &m_ViewControllerSpeed)) {
                m_viewController.setSpeed(m_ViewControllerSpeed);
            }

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

static std::string GetBaseDir(const std::string &filepath) {
    if (filepath.find_last_of("/\\") != std::string::npos)
        return filepath.substr(0, filepath.find_last_of("/\\"));
    return "";
}

static bool FileExists(const std::string &abs_filename) {
    bool ret;
    FILE *fp = fopen(abs_filename.c_str(), "rb");
    if (fp) {
        ret = true;
        fclose(fp);
    } else {
        ret = false;
    }

    return ret;
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

    //Load the shapes and materials from OBJ

    // Initialize buffers

    std::string err;
    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, inputfile.c_str(), basedir.c_str(), true);

    if (!err.empty()) { // `err` may contain warning message.
        std::cerr << err << std::endl;
    }

    if (!ret) {
        exit(1);
    }

    // Display file contents
    /*
    printf("# of vertices  = %d\n", (int)(attrib.vertices.size()) / 3);
    printf("# of normals   = %d\n", (int)(attrib.normals.size()) / 3);
    printf("# of texcoords = %d\n", (int)(attrib.texcoords.size()) / 2);
    printf("# of materials = %d\n", (int)materials.size());
    printf("# of shapes    = %d\n", (int)shapes.size());

    int nb_indexes = 0;

    for (size_t s = 0; s < shapes.size(); s++) {
        printf("shape[%ld].name = %s\n", s, shapes[s].name.c_str());
        printf("Size of shape[%ld].indices: %ld\n", s, shapes[s].mesh.indices.size());
        printf("Size of shape[%ld].material_ids: %ld\n", s, shapes[s].mesh.material_ids.size());
        assert((shapes[s].mesh.indices.size() % 3) == 0);

        for (size_t i = 0; i < shapes[s].mesh.indices.size(); i++) {
            ++nb_indexes;
//            printf("  indice[%d] = { %d, %d, %d}\n", i,
//                   shapes[s].mesh.indices[i].vertex_index,
//                   shapes[s].mesh.indices[i].normal_index,
//                   shapes[s].mesh.indices[i].texcoord_index);
        }


//        for (size_t f = 0; f < shapes[s].mesh.indices.size() / 3; f++) {
//            printf("  face[%ld] vertex indexes = %d, %d, %d\n", f, shapes[s].mesh.indices[3*f+0].vertex_index, shapes[s].mesh.indices[3*f+1].vertex_index, shapes[s].mesh.indices[3*f+2].vertex_index);
//            printf("  face[%ld] normal indexes = %d, %d, %d\n", f, shapes[s].mesh.indices[3*f+0].normal_index, shapes[s].mesh.indices[3*f+1].normal_index, shapes[s].mesh.indices[3*f+2].normal_index);
//            printf("  face[%ld] texcoord indexes = %d, %d, %d\n", f, shapes[s].mesh.indices[3*f+0].texcoord_index, shapes[s].mesh.indices[3*f+1].texcoord_index, shapes[s].mesh.indices[3*f+2].texcoord_index);
//
//            printf("  face[%ld] material index = %d\n\n", f, shapes[s].mesh.material_ids[f]);
//
//
//            printf("  face[%ld] vertices = (%f %f %f) (%f %f %f) (%f %f %f)\n\n", f,
//                   attrib.vertices[shapes[s].mesh.indices[3*f+0].vertex_index * 3 + 0], attrib.vertices[shapes[s].mesh.indices[3*f+0].vertex_index * 3 + 1], attrib.vertices[shapes[s].mesh.indices[3*f+0].vertex_index * 3 + 2],
//                   attrib.vertices[shapes[s].mesh.indices[3*f+1].vertex_index * 3 + 0], attrib.vertices[shapes[s].mesh.indices[3*f+1].vertex_index * 3 + 1], attrib.vertices[shapes[s].mesh.indices[3*f+1].vertex_index * 3 + 2],
//                   attrib.vertices[shapes[s].mesh.indices[3*f+2].vertex_index * 3 + 0], attrib.vertices[shapes[s].mesh.indices[3*f+2].vertex_index * 3 + 1], attrib.vertices[shapes[s].mesh.indices[3*f+2].vertex_index * 3 + 2]
//            );
//        }
    }

    printf("# of indexes    = %d\n", nb_indexes);

    for (size_t i = 0; i < materials.size(); i++) {
        printf("material[%ld].name = %s\n", i, materials[i].name.c_str());
        printf("  material.Ka = (%f, %f ,%f)\n", materials[i].ambient[0], materials[i].ambient[1], materials[i].ambient[2]);
        printf("  material.Kd = (%f, %f ,%f)\n", materials[i].diffuse[0], materials[i].diffuse[1], materials[i].diffuse[2]);
        printf("  material.Ks = (%f, %f ,%f)\n", materials[i].specular[0], materials[i].specular[1], materials[i].specular[2]);
        printf("  material.Tr = (%f, %f ,%f)\n", materials[i].transmittance[0], materials[i].transmittance[1], materials[i].transmittance[2]);
        printf("  material.Ke = (%f, %f ,%f)\n", materials[i].emission[0], materials[i].emission[1], materials[i].emission[2]);
        printf("  material.Ns = %f\n", materials[i].shininess);
        printf("  material.Ni = %f\n", materials[i].ior);
        printf("  material.dissolve = %f\n", materials[i].dissolve);
        printf("  material.illum = %d\n", materials[i].illum);
        printf("  material.map_Ka = %s\n", materials[i].ambient_texname.c_str());
        printf("  material.map_Kd = %s\n", materials[i].diffuse_texname.c_str());
        printf("  material.map_Ks = %s\n", materials[i].specular_texname.c_str());
        printf("  material.map_Ns = %s\n", materials[i].specular_highlight_texname.c_str());
        std::map<std::string, std::string>::const_iterator it(materials[i].unknown_parameter.begin());
        std::map<std::string, std::string>::const_iterator itEnd(materials[i].unknown_parameter.end());
        for (; it != itEnd; it++) {
            printf("  material.%s = %s\n", it->first.c_str(), it->second.c_str());
        }
        printf("\n");
    }
    //*/

    // Load diffuse textures

    glActiveTexture(GL_TEXTURE0);
    {
        for (size_t m = 0; m < materials.size(); ++m) {
            tinyobj::material_t* mp = &materials[m];

            if (mp->diffuse_texname.length() > 0) {
                // Only load the texture if it is not already loaded
                if (m_textures.find(mp->diffuse_texname) == m_textures.end()) {

                    std::string texture_filename = mp->diffuse_texname;
                    if (!FileExists(texture_filename)) {
                        // Append base dir.
                        texture_filename = basedir + mp->diffuse_texname;
                        if (!FileExists(texture_filename)) {
                            std::cerr << "Unable to find file: " << mp->diffuse_texname << std::endl;
                            exit(1);
                        }
                    }

                    auto image = glmlv::readImage(basedir + mp->diffuse_texname);
                    GLuint texture_id;

                    glGenTextures(1, &texture_id);
                    glBindTexture(GL_TEXTURE_2D, texture_id);
                    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, image.width(), image.height());

                    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image.width(), image.height(), GL_RGBA, GL_UNSIGNED_BYTE, image.data());
                    glBindTexture(GL_TEXTURE_2D, 0);

                    m_textures.insert(std::make_pair(mp->diffuse_texname, texture_id));
                }
            }
        }
    }
    //*/

    // Load geometry and build buffers
    ObjGeometry obj = loadObj(attrib, shapes);

    glCreateBuffers(1, &m_objVBO);
    glNamedBufferStorage(m_objVBO, obj.vertexBuffer.size() * sizeof(Vertex3f3f2f), obj.vertexBuffer.data(), 0);

    for(size_t s = 0; s < shapes.size(); ++s) {

        // Build IBO for the shape

        GLuint IBO;
        glCreateBuffers(1, &IBO);
        glNamedBufferStorage(IBO, obj.indexBuffers[s].size() * sizeof(int32_t), obj.indexBuffers[s].data(), 0);
        m_objIBOs.push_back(IBO);


        // Create VAO and bind attributes
        GLuint VAO;
        glCreateVertexArrays(1, &VAO);

        glVertexArrayVertexBuffer(VAO, vboBindingIndex, m_objVBO, 0, sizeof(Vertex3f3f2f));

        glVertexArrayAttribBinding(VAO, positionAttr_location, vboBindingIndex);
        glEnableVertexArrayAttrib(VAO, positionAttr_location);
        glVertexArrayAttribFormat(VAO, positionAttr_location, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex3f3f2f, position));

        glVertexArrayAttribBinding(VAO, normalAttr_location, vboBindingIndex);
        glEnableVertexArrayAttrib(VAO, normalAttr_location);
        glVertexArrayAttribFormat(VAO, normalAttr_location, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex3f3f2f, normal));

        glVertexArrayAttribBinding(VAO, texCoordsAttr_location, vboBindingIndex);
        glEnableVertexArrayAttrib(VAO, texCoordsAttr_location);
        glVertexArrayAttribFormat(VAO, texCoordsAttr_location, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex3f3f2f, texCoords));

        glVertexArrayElementBuffer(VAO, IBO);
        m_objVAOs.push_back(VAO);

        // TODO handle material index
        materialIndexes.push_back(obj.materialIndexes[s]);
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
}

