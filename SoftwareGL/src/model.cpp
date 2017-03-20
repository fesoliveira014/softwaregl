#include "model.hpp"

Model::Model(std::string filename)
    : m_vertices(), m_normals(), m_uv(), m_faces(), 
      m_diffuseMap(), m_normalMap(), m_specularMap()
{
    std::ifstream in;
    
    in.open(filename.c_str(), std::ifstream::in);
    if (in.fail())
        return;

    std::string line;
    while (!in.eof()) {
        std::getline(in, line);
        std::istringstream iss(line.c_str());
        char trash;

        if (!line.compare(0, 2, "v ")) {
            iss >> trash;
            glm::vec3 v;
            
            for (int i = 0; i < 3; ++i) iss >> v[i];
            m_vertices.push_back(v);
        }
        else if (!line.compare(0, 3, "vn ")) {
            iss >> trash;
            glm::vec3 n;

            for (int i = 0; i < 3; ++i) iss >> n[i];
            m_normals.push_back(n);
        }
        else if (!line.compare(0, 2, "vt ")) {
            iss >> trash;
            glm::vec2 uv;

            for (int i = 0; i < 2; ++i) iss >> uv[i];
            m_uv.push_back(uv);
        }
        else if (!line.compare(0, 2, "f ")) {
            glvertex v[3];
            glm::vec3 tmp;
            iss >> trash;
            int j = 0;

            while (iss >> tmp[0] >> trash >> tmp[1] >> trash >> tmp[2]) {
                for (int i = 0; i < 3; ++i) tmp[i]--;
                v[j] = glvertex(tmp);
                j++;
            }
            m_faces.push_back(face(v[0], v[1], v[2]));
        }
    }

    std::cerr << "# v# " << m_vertices.size() <<
                 " f# "  << m_faces.size() <<
                 " vt# " << m_uv.size() <<
                 " vn# " << m_normals.size() << std::endl;

    LoadTexture(filename, "_diffuse.tga", &m_diffuseMap);
    LoadTexture(filename, "_nm_tangent.tga", &m_normalMap);
    LoadTexture(filename, "_specular.tga", &m_specularMap);
}

Model::~Model() {}

size_t Model::GetNumOfVertices()
{
    return m_vertices.size();
}

size_t Model::GetNumOfFaces()
{
    return m_faces.size();
}

glm::vec3 Model::Normal(int face, int vertice)
{

}

glm::vec3 Model::Normal(const glm::vec2 &uv)
{

}

glm::vec3 Model::GetVertice(int vertice)
{

}

glm::vec3 Model::GetVertice(int face, int vertice)
{

}

glm::vec2 Model::UV(int face, int vertice)
{

}

TGAColor Model::GetDiffuse(const glm::vec2 & uv)
{

}

float Model::GetSpecular(const glm::vec2 & uv)
{

}

Model::face Model::GetFace(int index)
{

}

void Model::LoadTexture(std::string filename, const char *suffix, TGAImage *image)
{
    return;
}