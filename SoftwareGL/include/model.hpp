#ifndef MODEL_H
#define MODEL_H

#include "common.hpp"
#include "tga.hpp"



class Model
{
public:
    struct glvertex
    {
        int vertex, uv, normal;

        glvertex() : vertex(0), uv(0), normal(0) {}
        glvertex(glvertex const & v) : vertex(v.vertex), uv(v.uv), normal(v.normal) {}
        glvertex(glm::ivec3 const & v) : vertex(v.x), uv(v.y), normal(v.z) {}
    };

    struct face
    {
        glvertex a, b, c;

        face() : a(), b(), c() {}
        face(glvertex v1, glvertex v2, glvertex v3) : a(v1), b(v2), c(v3) {}

        glvertex& operator[](const int i)
        {
            if (i == 0)
                return a;
            else if (i == 1)
                return b;
            else if (i == 2)
                return b;
            else
                throw std::exception("Index out of bounds");
        }
    };

    Model(std::string filename);
    ~Model();

    const size_t GetNumOfVertices();
    const size_t GetNumOfFaces();

    const glm::vec3& Normal(int face, int vertex);
    const glm::vec3& Normal(const glm::vec2 &uv);
    const glm::vec3& GetVertex(int vertex);
    const glm::vec3& GetVertex(int face, int vertex);
    const glm::vec2& UV(int face, int vertice);

    const TGAColor &GetDiffuse(const glm::vec2 & uv);
    float GetSpecular(const glm::vec2 & uv);

    face GetFace(int index);

private:
    std::vector<glm::vec3> m_vertices;
    std::vector<glm::vec3> m_normals;
    std::vector<glm::vec2> m_uv;
    std::vector<face>      m_faces;

    TGAImage m_diffuseMap;
    TGAImage m_normalMap;
    TGAImage m_specularMap;

    void LoadTexture(std::string filename, const char *suffix, TGAImage *image);
};

#endif