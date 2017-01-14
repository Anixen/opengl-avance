#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
#include "tiny_obj_loader.h"

#include <glmlv/simple_geometry.hpp>
#include <glm/gtc/constants.hpp>
#include <iostream>
#include <tiny_obj_loader.h>

namespace glmlv
{

SimpleGeometry makeTriangle()
{
    std::vector<Vertex3f3f2f> vertexBuffer =
    {
        { glm::vec3(-0.5, -0.5, 0), glm::vec3(0, 0, 1), glm::vec2(0, 0) },
        { glm::vec3(0.5, -0.5, 0), glm::vec3(0, 0, 1), glm::vec2(1, 0) },
        { glm::vec3(0., 0.5, 0), glm::vec3(0, 0, 1), glm::vec2(0.5, 1) }
    };

    std::vector<uint32_t> indexBuffer =
    {
        0, 1, 2
    };

    return{ vertexBuffer, indexBuffer };
}

SimpleGeometry makeCube()
{
    std::vector<Vertex3f3f2f> vertexBuffer =
    {
        // Bottom side
        { glm::vec3(-0.5, -0.5, -0.5), glm::vec3(0, -1, 0), glm::vec2(0, 0) },
        { glm::vec3(0.5, -0.5, -0.5), glm::vec3(0, -1, 0), glm::vec2(0, 1) },
        { glm::vec3(0.5, -0.5, 0.5), glm::vec3(0, -1, 0), glm::vec2(1, 1) },
        { glm::vec3(-0.5, -0.5, 0.5), glm::vec3(0, -1, 0), glm::vec2(1, 0) },
        // Right side
        { glm::vec3(0.5, -0.5, 0.5), glm::vec3(1, 0, 0), glm::vec2(0, 0) },
        { glm::vec3(0.5, -0.5, -0.5), glm::vec3(1, 0, 0), glm::vec2(0, 1) },
        { glm::vec3(0.5, 0.5, -0.5), glm::vec3(1, 0, 0), glm::vec2(1, 1) },
        { glm::vec3(0.5, 0.5, 0.5), glm::vec3(1, 0, 0), glm::vec2(1, 0) },
        // Back side
        { glm::vec3(0.5, -0.5, -0.5), glm::vec3(0, 0, -1), glm::vec2(0, 0) },
        { glm::vec3(-0.5, -0.5, -0.5), glm::vec3(0, 0, -1), glm::vec2(0, 1) },
        { glm::vec3(-0.5, 0.5, -0.5), glm::vec3(0, 0, -1), glm::vec2(1, 1) },
        { glm::vec3(0.5, 0.5, -0.5), glm::vec3(0, 0, -1), glm::vec2(1, 0) },
        // Left side
        { glm::vec3(-0.5, -0.5, -0.5), glm::vec3(-1, 0, 0), glm::vec2(0, 0) },
        { glm::vec3(-0.5, -0.5, 0.5), glm::vec3(-1, 0, 0), glm::vec2(0, 1) },
        { glm::vec3(-0.5, 0.5, 0.5), glm::vec3(-1, 0, 0), glm::vec2(1, 1) },
        { glm::vec3(-0.5, 0.5, -0.5), glm::vec3(-1, 0, 0), glm::vec2(1, 0) },
        // Front side
        { glm::vec3(-0.5, -0.5, 0.5), glm::vec3(0, 0, 1), glm::vec2(0, 0) },
        { glm::vec3(0.5, -0.5, 0.5), glm::vec3(0, 0, 1), glm::vec2(0, 1) },
        { glm::vec3(0.5, 0.5, 0.5), glm::vec3(0, 0, 1), glm::vec2(1, 1) },
        { glm::vec3(-0.5, 0.5, 0.5), glm::vec3(0, 0, 1), glm::vec2(1, 0) },
        // Top side
        { glm::vec3(-0.5, 0.5, 0.5), glm::vec3(0, 1, 0), glm::vec2(0, 0) },
        { glm::vec3(0.5, 0.5, 0.5), glm::vec3(0, 1, 0), glm::vec2(0, 1) },
        { glm::vec3(0.5, 0.5, -0.5), glm::vec3(0, 1, 0), glm::vec2(1, 1) },
        { glm::vec3(-0.5, 0.5, -0.5), glm::vec3(0, 1, 0), glm::vec2(1, 0) }
    };

    std::vector<uint32_t> indexBuffer =
    {
        0, 1, 2,
        0, 2, 3,
        4, 5, 6,
        4, 6, 7,
        8, 9, 10,
        8, 10, 11,
        12, 13, 14,
        12, 14, 15,
        16, 17, 18,
        16, 18, 19,
        20, 21, 22,
        20, 22, 23
    };

    return{ vertexBuffer, indexBuffer };
}

SimpleGeometry makeSphere(uint32_t subdivLongitude)
{
    const auto discLong = subdivLongitude;
    const auto discLat = 2 * discLong;

    float rcpLat = 1.f / discLat, rcpLong = 1.f / discLong;
    float dPhi = glm::pi<float>() * 2.f * rcpLat, dTheta = glm::pi<float>() * rcpLong;

    std::vector<Vertex3f3f2f> vertexBuffer;

    for (uint32_t j = 0; j <= discLong; ++j)
    {
        float cosTheta = cos(-glm::half_pi<float>() + j * dTheta);
        float sinTheta = sin(-glm::half_pi<float>() + j * dTheta);

        for (uint32_t i = 0; i <= discLat; ++i) {
            glm::vec3 coords;

            coords.x = sin(i * dPhi) * cosTheta;
            coords.y = sinTheta;
            coords.z = cos(i * dPhi) * cosTheta;

            vertexBuffer.emplace_back(coords, coords, glm::vec2(i * rcpLat, j * rcpLong));
        }
    }

    std::vector<uint32_t> indexBuffer;

    for (uint32_t j = 0; j < discLong; ++j)
    {
        uint32_t offset = j * (discLat + 1);
        for (uint32_t i = 0; i < discLat; ++i)
        {
            indexBuffer.push_back(offset + i);
            indexBuffer.push_back(offset + (i + 1));
            indexBuffer.push_back(offset + discLat + 1 + (i + 1));

            indexBuffer.push_back(offset + i);
            indexBuffer.push_back(offset + discLat + 1 + (i + 1));
            indexBuffer.push_back(offset + i + discLat + 1);
        }
    }

    return{ vertexBuffer, indexBuffer };
}

ObjGeometry loadObj(tinyobj::attrib_t attrib, std::vector<tinyobj::shape_t> shapes)
{
//    unsigned long nb_vertices = attrib.vertices.size() / 3;
//    std::vector<Vertex3f3f2f> vertexBuffer(nb_vertices);
    std::vector<Vertex3f3f2f> vertexBuffer;
    std::vector<std::vector<int32_t>> indexBuffers;
    std::vector<uint32_t> materialIndexes;

    for(size_t s = 0; s < shapes.size(); ++s) {
        std::vector<int32_t> indexBuffer;
        for (size_t i = 0; i < shapes[s].mesh.indices.size(); ++i) {

            tinyobj::index_t idx = shapes[s].mesh.indices[i];

            Vertex3f3f2f v = {
                    {
                          attrib.vertices[idx.vertex_index * 3 + 0],
                          attrib.vertices[idx.vertex_index * 3 + 1],
                          attrib.vertices[idx.vertex_index * 3 + 2]
                    },
                    {
                            attrib.normals[idx.normal_index * 3 + 0],
                            attrib.normals[idx.normal_index * 3 + 1],
                            attrib.normals[idx.normal_index * 3 + 2]
                    },
                    {
                            attrib.texcoords[idx.texcoord_index * 2 + 0],
                            1.f - attrib.texcoords[idx.texcoord_index * 2 + 1]
                    }
            };

//            indexBuffer.push_back(idx.vertex_index);
//            vertexBuffer[idx.vertex_index] = v;

            indexBuffer.push_back(vertexBuffer.size());
            vertexBuffer.push_back(v);
        }
        indexBuffers.push_back(indexBuffer);
        materialIndexes.push_back(shapes[s].mesh.material_ids[0]);
    }

    return {vertexBuffer, indexBuffers, materialIndexes};
}
}