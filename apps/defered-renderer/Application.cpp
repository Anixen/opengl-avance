#include "Application.hpp"

#include <iostream>

#include <imgui.h>
#include <glmlv/imgui_impl_glfw_gl3.hpp>
#include <glmlv/obj_geometry.hpp>
#include <glmlv/Image2DRGBA.hpp>
#include <strstream>
#include <tiny_obj_loader.h>
//#include <tiny_obj_loader.h>

using namespace glmlv;


int Application::run()
{
    float clearColor[3] = { 0.3, 0.3, 0.3 };
    glClearColor(clearColor[0], clearColor[1], clearColor[2], 1.f);

    // Initalise lights
    // TODO Might be moved to member variables

    glm::vec3 directionalLightDir = {1, 1, 2};
    glm::vec3 directionalLightColor = {1, 1, 1};
    float directionalLightIntensity = 1.f;

    bool enablePointLights = true;
    std::vector<PointLight> pointLights = {
            {{64, 8, 0}, {1, 0, 0}, 100.f, true},
            {{0, 16, 0}, {0, 1, 0}, 750.f, true},
            {{0, 8, 32}, {0, 0, 1}, 500.f, true}
    };

    // Loop until the user closes the window
    for (auto iterationCount = 0u; !m_GLFWHandle.shouldClose(); ++iterationCount)
    {
        const auto seconds = glfwGetTime();

        // Put here rendering code




        // Geometry Pass
        {
            m_geometryProgram.use();
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_FBO);

            glViewport(0, 0, m_nWindowWidth, m_nWindowHeight);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glm::mat4 viewMatrix = m_viewController.getViewMatrix();
            glm::mat4 projMatrix = glm::perspective(70.f, (float) m_nWindowWidth / m_nWindowHeight, 0.1f, 10000.f);

            // Send uniforms
            // Matrices
            glm::mat4 modelMatrix = glm::mat4();
            modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0, -5));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1, 0.1, 0.1));

            glm::mat4 modelViewMatrix = viewMatrix * modelMatrix;
            glm::mat4 normalMatrix = glm::transpose(glm::inverse(modelViewMatrix));
            glm::mat4 modelViewProjMatrix = projMatrix * modelViewMatrix;
            glUniformMatrix4fv(m_uModelViewProjMatrix_location, 1, GL_FALSE, &modelViewProjMatrix[0][0]);
            glUniformMatrix4fv(m_uModelViewMatrix_location, 1, GL_FALSE, &modelViewMatrix[0][0]);
            glUniformMatrix4fv(m_uNormalMatrix_location, 1, GL_FALSE, &normalMatrix[0][0]);

            // Textures
            glBindSampler(0, m_KaSampler);
            glBindSampler(1, m_KdSampler);
            glBindSampler(2, m_KsSampler);
            glBindSampler(3, m_NsSampler);
            glBindSampler(4, m_dSampler);
            glBindSampler(5, m_NormalSampler);

            glUniform1i(m_uKaSampler_location,0);
            glUniform1i(m_uKdSampler_location, 1);
            glUniform1i(m_uKsSampler_location, 2);
            glUniform1i(m_uNsSampler_location, 3);
            glUniform1i(m_udSampler_location, 4);
            glUniform1i(m_uNormalSampler_location, 5);

            // Display the model
            for(size_t s = 0; s < m_objVAOs.size(); ++s) {

                glBindVertexArray(m_objVAOs[s]);

                // Get material properties
                int32_t matIndex = materialIndexes[s];

                glm::vec3 ka = {materials[matIndex].ambient[0], materials[matIndex].ambient[1], materials[matIndex].ambient[2]};
                std::string kaTexname = materials[matIndex].ambient_texname;
                bool kaMap = !(kaTexname == "");

                glm::vec3 kd = {materials[matIndex].diffuse[0], materials[matIndex].diffuse[1], materials[matIndex].diffuse[2]};
                std::string kdTexname = materials[matIndex].diffuse_texname;
                bool kdMap = !(kdTexname == "");

                glm::vec3 ks = {materials[matIndex].specular[0], materials[matIndex].specular[1], materials[matIndex].specular[2]};
                std::string ksTexname = materials[matIndex].specular_texname;
                bool ksMap = !(ksTexname == "");

                float ns = materials[matIndex].shininess;
                std::string nsTexname = materials[matIndex].specular_highlight_texname;
                bool nsMap = !(nsTexname == "");

                float d = materials[matIndex].dissolve;
                std::string dTexname = materials[matIndex].alpha_texname;
                bool dMap = !(dTexname == "");

                std::string NormalTexname = materials[matIndex].bump_texname;
                bool NormalMap = !(NormalTexname == "");

//                glActiveTexture(GL_TEXTURE0);
                glUniform3f(m_uKa_location, ka[0], ka[1], ka[2]);
                glUniform1i(m_uKaMap_location, kaMap);
                if(kaMap)
                    glBindTextureUnit(0, m_textures[kaTexname]);

//                glActiveTexture(GL_TEXTURE1);
                glUniform3f(m_uKd_location, kd[0], kd[1], kd[2]);
                glUniform1i(m_uKdMap_location, kdMap);
                if(kdMap)
                    glBindTextureUnit(1, m_textures[kdTexname]);

//                glActiveTexture(GL_TEXTURE2);
                glUniform3f(m_uKs_location, ks[0], ks[1], ks[2]);
                glUniform1i(m_uKsMap_location, ksMap);
                if(ksMap)
                    glBindTextureUnit(2, m_textures[ksTexname]);

//                glActiveTexture(GL_TEXTURE3);
                glUniform1i(m_uNsMap_location, nsMap);
                glUniform1f(m_uNs_location, ns);
                if(nsMap)
                    glBindTextureUnit(3, m_textures[nsTexname]);

//                glActiveTexture(GL_TEXTURE4);
                glUniform1i(m_udMap_location, dMap);
                glUniform1f(m_ud_location, d);
                if(dMap)
                    glBindTextureUnit(4, m_textures[dTexname]);

//                glActiveTexture(GL_TEXTURE5);
                glUniform1i(m_uNormalMap_location, NormalMap);
                if(NormalMap)
                    glBindTextureUnit(5, m_textures[NormalTexname]);

                glDrawElements(GL_TRIANGLES, shapes[s].mesh.indices.size(), GL_UNSIGNED_INT, nullptr);
                glBindVertexArray(0);
            }
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        }



        // Blit GBuffer textures
        // TODO Change buffer through GUI
        const auto viewportSize = m_GLFWHandle.framebufferSize();
        glViewport(0, 0, viewportSize.x, viewportSize.y);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glNamedFramebufferReadBuffer(m_FBO, GL_COLOR_ATTACHMENT0 + m_CurrentlyDisplayed);
        //glNamedFramebufferDrawBuffer(0, GL_BACK);
        glBlitNamedFramebuffer(m_FBO, 0,
                               0, 0, m_nWindowWidth, m_nWindowHeight,
                               0, 0, m_nWindowWidth, m_nWindowHeight,
                               GL_COLOR_BUFFER_BIT, GL_LINEAR);

        // Shading pass
        /*
            // Lighting
            glm::vec4 viewDirectionalLightDir = viewMatrix * glm::vec4(directionalLightDir[0], directionalLightDir[1], directionalLightDir[2], 0);
            viewDirectionalLightDir = glm::normalize(viewDirectionalLightDir);

            glUniform3f(m_uDirectionalLightDir_location, viewDirectionalLightDir[0], viewDirectionalLightDir[1], viewDirectionalLightDir[2]);
            glUniform3f(m_uDirectionalLightColor_location, directionalLightColor[0], directionalLightColor[1], directionalLightColor[2]);
            glUniform1f(m_uDirectionalLightIntensity_location, directionalLightIntensity);

            glUniform1i(m_uEnablePointLights_location, enablePointLights);
            glUniform1i(m_uNbPointLights_location, pointLights.size());

            // Build SSBO
            //updatePointLightSSBO(pointLights, viewMatrix);

        //*/


        // GUI code:
        ImGui_ImplGlfwGL3_NewFrame();

        {
            ImGui::Begin("GUI_General");
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::ColorEditMode(ImGuiColorEditMode_RGB);
            if (ImGui::ColorEdit3("clearColor", clearColor)) {
                glClearColor(clearColor[0], clearColor[1], clearColor[2], 1.f);
            }

            if(ImGui::DragFloat("cameraSpeed", &m_ViewControllerSpeed, 1.f, 0.1f, 100000.f, "%.3f", 1.4f)) {
                m_viewController.setSpeed(m_ViewControllerSpeed);
            }

            if(ImGui::SliderFloat("cameraRotationSpeed", &m_ViewControllerRotationSpeed, 0.001f, 0.1f)) {
                m_viewController.setRotationSpeed(m_ViewControllerRotationSpeed);
            }

            if(ImGui::CollapsingHeader("GBuffer"))
            {
                for (int32_t i = GPosition; i < GDepth; ++i)
                {
                    if (ImGui::RadioButton(m_GBufferTexNames[i], m_CurrentlyDisplayed == i))
                        m_CurrentlyDisplayed = GBufferTextureType(i);
                }
            }

            ImGui::End();
        }

        {
            ImGui::Begin("GUI_Lights");
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::ColorEditMode(ImGuiColorEditMode_RGB);

            ImGui::DragFloat3("directionalLightDir", &directionalLightDir[0]);
            ImGui::ColorEdit3("directionalLightColor", &directionalLightColor[0]);
            ImGui::DragFloat("directionalLightIntensity", &directionalLightIntensity);

            ImGui::Checkbox("Enable point lights", &enablePointLights);
            for(int l = 0; l < pointLights.size(); ++l) {

                // TODO Check impact performance of ostringstream
                ImGui::Text("PointLight_%d", l);

                std::ostringstream prefix;
                prefix << "PointLight_" << l;
                std::ostringstream positionLabel, colorLabel, intensityLabel, enableLabel;
                positionLabel << prefix.str() << "_Position";
                colorLabel << prefix.str() << "_Color";
                intensityLabel << prefix.str() << "_Intensity";
                enableLabel << prefix.str() << "_Enabled";

                PointLight &light = pointLights[l];
                ImGui::DragFloat3(positionLabel.str().c_str(), &(light.position[0]));
                ImGui::ColorEdit3(colorLabel.str().c_str(), &(pointLights[l].color[0]));
                ImGui::DragFloat(intensityLabel.str().c_str(), &(light.intensity));
                ImGui::Checkbox(enableLabel.str().c_str(), &(light.enabled));
            }

            ImGui::End();
        }

//        const auto viewportSize = m_GLFWHandle.framebufferSize();
//        glViewport(0, 0, viewportSize.x, viewportSize.y);
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


void Application::loadTexture(std::string texName)
{
    if (texName.length() > 0) {
        // Only load the texture if it is not already loaded
        if (m_textures.find(texName) == m_textures.end()) {

            std::string texture_filename = texName;
            if (!FileExists(texture_filename)) {
                // Append base dir.
                texture_filename = basedir + texName;
                if (!FileExists(texture_filename)) {
                    std::cerr << "Unable to find file: " << texName<< std::endl;
                    exit(1);
                }
            }

            auto image = glmlv::readImage(basedir + texName);
            GLuint texture_id;

            glCreateTextures( GL_TEXTURE_2D, 1,&texture_id);
            glTextureStorage2D(texture_id, 1, GL_RGB32F, image.width(), image.height());
            glTextureSubImage2D(texture_id, 0, 0, 0, image.width(), image.height(), GL_RGBA, GL_UNSIGNED_BYTE, image.data());

            m_textures.insert(std::make_pair(texName, texture_id));
        }
    }
}


void Application::updatePointLightSSBO(const std::vector<PointLight>& pointLights, const glm::mat4& viewMatrix)
{
    std::vector<glm::vec4> pointLightsPositionsStorageBuffer;
    std::vector<glm::vec4> pointLightsColorsStorageBuffer;
    std::vector<float> pointLightsIntensitiesStorageBuffer;
    std::vector<int> pointLightsEnabledStorageBuffer;

    for(int l = 0; l < pointLights.size(); ++l) {
        const PointLight &light = pointLights[l];

        pointLightsPositionsStorageBuffer.push_back(viewMatrix * glm::vec4(light.position, 1.f));
        pointLightsColorsStorageBuffer.push_back(glm::vec4(light.color, 1.f));
        pointLightsIntensitiesStorageBuffer.push_back(light.intensity);
        pointLightsEnabledStorageBuffer.push_back(light.enabled);
    }

    GLvoid  *p;

    p = glMapNamedBuffer(m_pointLightPositionSSBO, GL_WRITE_ONLY);
    memcpy(p, pointLightsPositionsStorageBuffer.data(), pointLightsPositionsStorageBuffer.size() * sizeof(glm::vec4));
    glUnmapNamedBuffer(m_pointLightPositionSSBO);

    p = glMapNamedBuffer(m_pointLightColorSSBO, GL_WRITE_ONLY);
    memcpy(p, pointLightsColorsStorageBuffer.data(), pointLightsColorsStorageBuffer.size() * sizeof(glm::vec4));
    glUnmapNamedBuffer(m_pointLightColorSSBO);

    p = glMapNamedBuffer(m_pointLightIntensitySSBO, GL_WRITE_ONLY);
    memcpy(p, pointLightsIntensitiesStorageBuffer.data(), pointLightsIntensitiesStorageBuffer.size() * sizeof(float));
    glUnmapNamedBuffer(m_pointLightIntensitySSBO);

    p = glMapNamedBuffer(m_pointLightEnabledSSBO, GL_WRITE_ONLY);
    memcpy(p, pointLightsEnabledStorageBuffer.data(), pointLightsEnabledStorageBuffer.size() * sizeof(int));
    glUnmapNamedBuffer(m_pointLightEnabledSSBO);
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
    /*
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

    // Load textures

    {
        for (size_t m = 0; m < materials.size(); ++m) {
            tinyobj::material_t* mp = &materials[m];

            loadTexture(mp->ambient_texname);
            loadTexture(mp->diffuse_texname);
            loadTexture(mp->specular_texname);
            loadTexture(mp->specular_highlight_texname);
            loadTexture(mp->alpha_texname);
            loadTexture(mp->bump_texname);
        }
    }

    // Note: no need to bind a sampler for modifying it: the sampler API is already direct_state_access
    glCreateSamplers(1, &m_KaSampler);
    glSamplerParameteri(m_KaSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glSamplerParameteri(m_KaSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glCreateSamplers(1, &m_KdSampler);
    glSamplerParameteri(m_KdSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glSamplerParameteri(m_KdSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glCreateSamplers(1, &m_KsSampler);
    glSamplerParameteri(m_KsSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glSamplerParameteri(m_KsSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glCreateSamplers(1, &m_NsSampler);
    glSamplerParameteri(m_NsSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glSamplerParameteri(m_NsSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glCreateSamplers(1, &m_dSampler);
    glSamplerParameteri(m_dSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glSamplerParameteri(m_dSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glCreateSamplers(1, &m_NormalSampler);
    glSamplerParameteri(m_NormalSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glSamplerParameteri(m_NormalSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load geometry and build buffers
    ObjGeometry obj = loadObj(attrib, shapes);

    glCreateBuffers(1, &m_objVBO);
    glNamedBufferStorage(m_objVBO, obj.vertexBuffer.size() * sizeof(Vertex3f3f3f2f), obj.vertexBuffer.data(), 0);

    for(size_t s = 0; s < shapes.size(); ++s) {

        // Build IBO for the shape

        GLuint IBO;
        glCreateBuffers(1, &IBO);
        glNamedBufferStorage(IBO, obj.indexBuffers[s].size() * sizeof(int32_t), obj.indexBuffers[s].data(), 0);
        m_objIBOs.push_back(IBO);


        // Create VAO and bind attributes
        GLuint VAO;
        glCreateVertexArrays(1, &VAO);

        glVertexArrayVertexBuffer(VAO, vboBindingIndex, m_objVBO, 0, sizeof(Vertex3f3f3f2f));

        glVertexArrayAttribBinding(VAO, positionAttr_location, vboBindingIndex);
        glEnableVertexArrayAttrib(VAO, positionAttr_location);
        glVertexArrayAttribFormat(VAO, positionAttr_location, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex3f3f3f2f, position));

        glVertexArrayAttribBinding(VAO, normalAttr_location, vboBindingIndex);
        glEnableVertexArrayAttrib(VAO, normalAttr_location);
        glVertexArrayAttribFormat(VAO, normalAttr_location, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex3f3f3f2f, normal));

        glVertexArrayAttribBinding(VAO, tangentAttr_location, vboBindingIndex);
        glEnableVertexArrayAttrib(VAO, tangentAttr_location);
        glVertexArrayAttribFormat(VAO, tangentAttr_location, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex3f3f3f2f, tangent));

        glVertexArrayAttribBinding(VAO, texCoordsAttr_location, vboBindingIndex);
        glEnableVertexArrayAttrib(VAO, texCoordsAttr_location);
        glVertexArrayAttribFormat(VAO, texCoordsAttr_location, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex3f3f3f2f, texCoords));

        glVertexArrayElementBuffer(VAO, IBO);
        m_objVAOs.push_back(VAO);

        // Handle material index
        materialIndexes.push_back(obj.materialIndexes[s]);
    }

    glEnable(GL_DEPTH_TEST);
//    glEnable (GL_BLEND);
//    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    // Here we load and compile shaders from the library
    m_geometryProgram = glmlv::compileProgram({ m_ShadersRootPath / m_AppName / "GeometryPass.vs.glsl", m_ShadersRootPath / m_AppName / "GeometryPass.fs.glsl" });
    m_geometryProgram.use();


    // Get Locations
    m_uModelViewProjMatrix_location = glGetUniformLocation(m_geometryProgram.glId(), "uModelViewProjMatrix");
    m_uModelViewMatrix_location = glGetUniformLocation(m_geometryProgram.glId(), "uModelViewMatrix");
    m_uNormalMatrix_location = glGetUniformLocation(m_geometryProgram.glId(), "uNormalMatrix");

    m_uKa_location = glGetUniformLocation(m_geometryProgram.glId(), "uKa");
    m_uKaSampler_location = glGetUniformLocation(m_geometryProgram.glId(), "uKaSampler");
    m_uKaMap_location = glGetUniformLocation(m_geometryProgram.glId(), "uKaMap");

    m_uKd_location = glGetUniformLocation(m_geometryProgram.glId(), "uKd");
    m_uKdSampler_location = glGetUniformLocation(m_geometryProgram.glId(), "uKdSampler");
    m_uKdMap_location = glGetUniformLocation(m_geometryProgram.glId(), "uKdMap");

    m_uKs_location = glGetUniformLocation(m_geometryProgram.glId(), "uKs");
    m_uKsSampler_location = glGetUniformLocation(m_geometryProgram.glId(), "uKsSampler");
    m_uKsMap_location = glGetUniformLocation(m_geometryProgram.glId(), "uKsMap");

    m_uNs_location = glGetUniformLocation(m_geometryProgram.glId(), "uNs");
    m_uNsSampler_location = glGetUniformLocation(m_geometryProgram.glId(), "uNsSampler");
    m_uNsMap_location = glGetUniformLocation(m_geometryProgram.glId(), "uNsMap");

    m_ud_location = glGetUniformLocation(m_geometryProgram.glId(), "ud");
    m_udSampler_location = glGetUniformLocation(m_geometryProgram.glId(), "udSampler");
    m_udMap_location = glGetUniformLocation(m_geometryProgram.glId(), "udMap");

    m_uNormalSampler_location = glGetUniformLocation(m_geometryProgram.glId(), "uNormalSampler");
    m_uNormalMap_location = glGetUniformLocation(m_geometryProgram.glId(), "uNormalMap");

    m_uDirectionalLightDir_location = glGetUniformLocation(m_geometryProgram.glId(), "uDirectionalLightDir");
    m_uDirectionalLightColor_location = glGetUniformLocation(m_geometryProgram.glId(), "uDirectionalLightColor");
    m_uDirectionalLightIntensity_location = glGetUniformLocation(m_geometryProgram.glId(), "uDirectionalLightIntensity");

    m_uEnablePointLights_location = glGetUniformLocation(m_geometryProgram.glId(), "uEnablePointLights");
    m_uNbPointLights_location = glGetUniformLocation(m_geometryProgram.glId(), "uNbPointLights");

    // Build and connect SSBOs
    /*
    GLuint block_index;
    GLuint binding_point_index;
    glCreateBuffers(1, &m_pointLightPositionSSBO);
    glNamedBufferData(m_pointLightPositionSSBO, 1024, nullptr, GL_DYNAMIC_DRAW);
    block_index = glGetProgramResourceIndex(m_geometryProgram.glId(), GL_SHADER_STORAGE_BLOCK, "aPointLightPosition");
    //glShaderStorageBlockBinding(m_geometryProgram.glId(), block_index, pointLightPosition_binding);
    binding_point_index = 0;
    glShaderStorageBlockBinding(m_geometryProgram.glId(), block_index, binding_point_index);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding_point_index, m_pointLightPositionSSBO);
    //glBindBufferRange(GL_SHADER_STORAGE_BUFFER, binding_point_index, m_pointLightPositionSSBO, 0, 1024);

    glCreateBuffers(1, &m_pointLightColorSSBO);
    glNamedBufferData(m_pointLightColorSSBO, 1024, nullptr, GL_DYNAMIC_DRAW);
    block_index = glGetProgramResourceIndex(m_geometryProgram.glId(), GL_SHADER_STORAGE_BLOCK, "aPointLightColor");
    //glShaderStorageBlockBinding(m_geometryProgram.glId(), block_index, pointLightColor_binding);
    binding_point_index = 1;
    glShaderStorageBlockBinding(m_geometryProgram.glId(), block_index, binding_point_index);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding_point_index, m_pointLightColorSSBO);
    //glBindBufferRange(GL_SHADER_STORAGE_BUFFER, binding_point_index, m_pointLightColorSSBO, 0, 1024);

    glCreateBuffers(1, &m_pointLightIntensitySSBO);
    glNamedBufferData(m_pointLightIntensitySSBO, 1024, nullptr, GL_DYNAMIC_DRAW);
    block_index = glGetProgramResourceIndex(m_geometryProgram.glId(), GL_SHADER_STORAGE_BLOCK, "aPointLightIntensity");
    //glShaderStorageBlockBinding(m_geometryProgram.glId(), block_index, pointLightIntensity_binding);
    binding_point_index = 2;
    glShaderStorageBlockBinding(m_geometryProgram.glId(), block_index, binding_point_index);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding_point_index, m_pointLightIntensitySSBO);
    //glBindBufferRange(GL_SHADER_STORAGE_BUFFER, binding_point_index, m_pointLightColorSSBO, 0, 1024);

    glCreateBuffers(1, &m_pointLightEnabledSSBO);
    glNamedBufferData(m_pointLightEnabledSSBO, 1024, nullptr, GL_DYNAMIC_DRAW);
    block_index = glGetProgramResourceIndex(m_geometryProgram.glId(), GL_SHADER_STORAGE_BLOCK, "aPointLightEnabled");
    //glShaderStorageBlockBinding(m_geometryProgram.glId(), block_index, pointLightEnabled_binding);
    binding_point_index = 3;
    glShaderStorageBlockBinding(m_geometryProgram.glId(), block_index, binding_point_index);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding_point_index, m_pointLightEnabledSSBO);
    //glBindBufferRange(GL_SHADER_STORAGE_BUFFER, binding_point_index, m_pointLightColorSSBO, 0, 1024);
    //*/

    // Deferred Rendering
    // Create GBuffer Textures
    glCreateTextures(GL_TEXTURE_2D, GBufferTextureCount, m_GBufferTextures);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    for(int i = GPosition; i < GBufferTextureCount; ++i)
        glTextureStorage2D(m_GBufferTextures[i], 1, m_GBufferTextureFormat[i], m_nWindowWidth, m_nWindowHeight);

    // Create Framebuffer
    glCreateFramebuffers(1, &m_FBO);
    for(int i = 0; i < GBufferTextureCount - 1; ++i)
        glNamedFramebufferTexture(m_FBO, GL_COLOR_ATTACHMENT0 + i, m_GBufferTextures[i], 0);
    glNamedFramebufferTexture(m_FBO, GL_DEPTH_ATTACHMENT, m_GBufferTextures[5], 0);

    GLenum drawBuffers[] =
            { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2,
              GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4};
    glNamedFramebufferDrawBuffers(m_FBO, 6, drawBuffers);

    GLenum frameBufferStatus = glCheckNamedFramebufferStatus(m_FBO, GL_DRAW_FRAMEBUFFER);
    switch (frameBufferStatus) {
        case GL_FRAMEBUFFER_COMPLETE :
            printf(ANSI_COLOR_GREEN  "Frame Buffer statuts : complete" ANSI_COLOR_RESET "\n");
            break;
        case GL_FRAMEBUFFER_UNDEFINED :
            printf(ANSI_COLOR_RED  "Frame Buffer statuts : undefined, does not exist" ANSI_COLOR_RESET "\n");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT :
            printf(ANSI_COLOR_RED  "Frame Buffer statuts : incomplete attachment" ANSI_COLOR_RESET "\n");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT :
            printf(ANSI_COLOR_RED  "Frame Buffer statuts : incomplete missing attachment" ANSI_COLOR_RESET "\n");
            break;
        default:
            printf(ANSI_COLOR_RED "Frame Buffer status : other/unknowm error" ANSI_COLOR_RESET "\n");
    }
}

