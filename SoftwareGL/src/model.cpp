#include "model.hpp"

Model::Model(std::string filename)
    : m_vertices(), m_normals(), m_uv(), m_faces(), 
      m_diffuseMap(), m_normalMap(), m_specularMap()
{
    std::ifstream in;
    
    in.open(filename.c_str(), std::ifstream::in);
    if (in.fail()) {
        std::cerr << "Could not open the file " << filename << "." << std::endl;
        return;
    }

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

const size_t Model::GetNumOfVertices()
{
    return m_vertices.size();
}

const size_t Model::GetNumOfFaces()
{
    return m_faces.size();
}

const glm::vec3& Model::Normal(int face, int vertex)
{
    int index = m_faces[face][vertex].normal;
    return glm::normalize(m_normals[index]);
}

const glm::vec3& Model::Normal(const glm::vec2 &uv)
{
    glm::ivec2 uvi(uv[0] * m_normalMap.GetWidth(), uv[1] * m_normalMap.GetWidth());
    TGAColor c = m_normalMap.Get(uvi.x, uvi.y);

    glm::vec3 res;
    for (int i = 0; i < 3; ++i) 
        res[2 - i] = (float)c[i] / 255.0f * 2.0f - 1.0f;
    
    return res;
}

const glm::vec3& Model::GetVertex(int vertex)
{
    return m_vertices[vertex];
}

const glm::vec3& Model::GetVertex(int face, int vertex)
{
    int index = m_faces[face][vertex].vertex;
    return m_vertices[index];
}

const glm::vec2& Model::UV(int face, int vertex)
{
    int index = m_faces[face][vertex].uv;
    return m_uv[index];
}

const TGAColor &Model::GetDiffuse(const glm::vec2 & uv)
{
    glm::ivec2 uvi(uv[0] * m_diffuseMap.GetWidth(), uv[1] * m_diffuseMap.GetWidth());
    return m_diffuseMap.Get(uvi.x, uvi.y);
}

float Model::GetSpecular(const glm::vec2 & uv)
{
    glm::ivec2 uvi(uv[0] * m_specularMap.GetWidth(), uv[1] * m_specularMap.GetWidth());
    return m_specularMap.Get(uvi.x, uvi.y)[0] / 1.0f;
}

Model::face Model::GetFace(int index)
{
    return m_faces[index];
}

void Model::LoadTexture(std::string filename, const char *suffix, TGAImage *image)
{
    std::string texFile(filename);
    size_t dot = texFile.find_last_of(".");
    if (dot != std::string::npos) {
        texFile = texFile.substr(0, dot) + std::string(suffix);
        std::cerr << "Texture file " << texFile << " loading " << (image->ReadFile(texFile.c_str()) ? "ok" : "failed") << std::endl;
        image->FlipVertically();
    }
}