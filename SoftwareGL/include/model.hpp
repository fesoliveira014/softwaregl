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
        glvertex(glm::vec3 const & v) : vertex(v.x), uv(v.y), normal(v.z) {}
    };

    struct face
    {
        glvertex a, b, c;

        face() : a(), b(), c() {}
        face(glvertex v1, glvertex v2, glvertex v3) : a(v1), b(v2), c(v3) {}
    };

    Model(std::string filename);
    ~Model();

    size_t GetNumOfVertices();
    size_t GetNumOfFaces();

    glm::vec3 Normal(int face, int vertice);
    glm::vec3 Nomal(glm::vec2 uv);
    glm::vec3 GetVertice(int vertice);
    glm::vec3 GetVertice(int face, int vertice);
    glm::vec2 UV(int face, int vertice);

    TGAColor GetDiffuse(glm::vec2 uv);
    float GetSpecular(glm::vec2 uv);

    face GetFace(int index);

private:
    std::vector<glm::vec3> m_vertices;
    std::vector<glm::vec3> m_normals;
    std::vector<glm::vec2> m_uv;
    std::vector<face>      m_faces;

    TGAImage m_diffuseMap;
    TGAImage m_normalMap;
    TGAImage m_specularMap;

    void LoadTexture(std::string filename, const char *suffix, TGAImage &image);
};

#endif