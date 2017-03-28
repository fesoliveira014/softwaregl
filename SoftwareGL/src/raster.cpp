#include "raster.hpp"

Rasterizer::Rasterizer(TGAImage &image)
{
    m_width = image.GetWidth();
    m_height = image.GetHeight();

    m_zBuffer = new float[m_width * m_height];
    for (uint i = 0; i < m_width * m_height; ++i)
        m_zBuffer[i] = -std::numeric_limits<float>::max();
}

Rasterizer::~Rasterizer()
{
    delete[] m_zBuffer;
}

void Rasterizer::line(const glm::ivec2 & aa, const glm::ivec2 & bb, TGAImage *image, TGAColor color)
{
    glm::ivec2 a(aa);
    glm::ivec2 b(bb);


    bool steep = false;

    if (std::abs(a.x - b.x) < std::abs(a.y - b.y)) {
        std::swap(a.x, a.y);
        std::swap(b.x, b.y);
        steep = true;
    }

    if (a.x > b.x) {
        std::swap(a, b);
    }

    int dx = b.x - a.x;
    int dy = b.y - a.y;
    int derror2 = std::abs(dy) * 2;
    int error2 = 0;
    int y = a.y;

    for (int x = a.x; x <= b.x; ++x) {
        if (steep) image->Set(y, x, color);
        else image->Set(x, y, color);
        error2 += derror2;
        if (error2 > dx) {
            y += (b.y > a.y ? 1 : -1);
            error2 -= dx * 2;
        }
    }
}

void Rasterizer::getTriangleBbox2D(const glm::vec3 &a, const glm::vec3 &b,
                                const glm::vec3 &c, const glm::vec2 &clamp, bbox2d &bbox)
{
    std::vector<glm::vec3> vertices;
    vertices.push_back(a);
    vertices.push_back(b);
    vertices.push_back(c);

    for (auto vertex : vertices) {
        for (int i = 0; i < 2; ++i) {
            bbox.min[i] = std::fmax(0.0f    , std::fmin(bbox.min[i], vertex[i]));
            bbox.max[i] = std::fmin(clamp[i], std::fmax(bbox.max[i], vertex[i]));
        }
    }
}

glm::vec3 Rasterizer::barycentric(const glm::vec3 &a, const glm::vec3 &b, 
                                const glm::vec3 &c, const glm::vec3 &p)
{
    glm::vec3 ab = b - a;
    glm::vec3 ac = c - a;
    glm::vec3 pa = a - p;

    glm::vec3 u = glm::cross(glm::vec3(ac.x, ab.x, pa.x), glm::vec3(ac.y, ab.y, pa.y));
    
    if (std::abs(u.z) > 1e-2) 
        return glm::vec3(1.f - (u.x + u.y)/u.z, u.y/u.z, u.x/u.z);
    
    return glm::vec3(-1,1,1);
}

void Rasterizer::triangle(const glvertex &t1, const glvertex &t2, const glvertex &t3, 
                        float *zBuffer, const TGAColor &color, TGAImage *image)
{
    glm::vec2 clamp(image->GetWidth() - 1, image->GetHeight() - 1);
    bbox2d bbox(std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
    
    glm::vec3 p;
    int index;

    getTriangleBbox2D(t1.position, t2.position, t3.position, clamp, bbox);

    for (p.x = bbox.min.x; p.x <= bbox.max.x; ++p.x) {
        for (p.y = bbox.min.y; p.y <= bbox.max.y; ++p.y) {
            glm::vec3 c = barycentric(t1.position, t2.position, t3.position, p);
            
            if (c.x < 0 || c.y < 0 || c.z < 0)
                continue;

            p.z = t1.position.z * c.x + t2.position.z * c.y + t3.position.z * c.z;
            index = int(p.x + p.y * image->GetWidth());
            
            if (zBuffer[index] < p.z) {
                zBuffer[index] = p.z;
                image->Set(p.x, p.y, color);
            }
        }
    }
}

void Rasterizer::triangle(const glvertex &t1, const glvertex &t2, const glvertex &t3,
                        float *zBuffer, Model& model, float intensity, TGAImage *image)
{
    glm::vec2 clamp(image->GetWidth() - 1, image->GetHeight() - 1);
    bbox2d bbox(std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());

    glm::vec3 p;
    glm::vec2 uv;
    int index;
    TGAColor color;

    getTriangleBbox2D(t1.position, t2.position, t3.position, clamp, bbox);

    for (p.x = bbox.min.x; p.x <= bbox.max.x; ++p.x) {
        for (p.y = bbox.min.y; p.y <= bbox.max.y; ++p.y) {
            glm::vec3 c = barycentric(t1.position, t2.position, t3.position, p);

            if (c.x < 0 || c.y < 0 || c.z < 0)
                continue;

            p.z = t1.position.z * c.x + t2.position.z * c.y + t3.position.z * c.z;
            uv = t1.uv * c.x + t2.uv * c.y + t3.uv * c.z;
            index = int(p.x + p.y * image->GetWidth());

            color = model.GetDiffuse(uv) * intensity;

            if (zBuffer[index] < p.z) {
                zBuffer[index] = p.z;
                image->Set((int)p.x, (int)p.y, color);
            }
        }
    }
}

void Rasterizer::render(Model& model, TGAImage *image, bool wireframe)
{
    TGAColor colors[4] = {TGAColor(255, 255, 255, 255),
                            TGAColor(0, 0, 255, 255),
                            TGAColor(0, 255, 0, 255),
                            TGAColor(255, 0, 0, 255)};

    TGAColor shade;

    glm::vec3 lightDir(0, 0, -1);

    for (uint i = 0; i < m_width * m_height; ++i)
        m_zBuffer[i] = -std::numeric_limits<float>::max();

    for (uint i = 0; i < model.GetNumOfFaces(); ++i) {
        Model::face face = model.GetFace(i);

        if (wireframe) {        
            for (uint j = 0; j < 3; ++j) {
                glm::vec3 v0 = model.GetVertex(face[j].vertex);
                glm::vec3 v1 = model.GetVertex(face[(j + 1) % 3].vertex);

                glm::ivec2 a((v0.x + 1.0f)* image->GetWidth() / 2.0f, (v0.y + 1.0f)* image->GetHeight() / 2.0f);
                glm::ivec2 b((v1.x + 1.0f)* image->GetWidth() / 2.0f, (v1.y + 1.0f)* image->GetHeight() / 2.0f);

                //std::cerr << "p1: " << glm::to_string(a) << ", p2: " << glm::to_string(b) << std::endl;

                line(a, b, image, colors[0]);
            }
        }

        else {
            glvertex vertices[3] = {
                {
                    model.GetVertex(face[0].vertex),
                    model.UV(i, 0),
                    glm::vec3 (0)
                    //model.normal(i, 0),
                },
                {
                    model.GetVertex(face[1].vertex),
                    model.UV(i, 1),
                    glm::vec3(0)
                    //model.normal(i, 1),
                },
                {
                    model.GetVertex(face[2].vertex),
                    model.UV(i, 2),
                    glm::vec3(0)
                    //model.normal(i, 2),
                } 
            };

            glm::vec3 a_pos(int((vertices[0].position.x + 1.0f)* image->GetWidth() / 2.0f + 0.5f), int((vertices[0].position.y + 1.0f)* image->GetHeight() / 2.0f + 0.5f), vertices[0].position.z);
            glm::vec3 b_pos(int((vertices[1].position.x + 1.0f)* image->GetWidth() / 2.0f + 0.5f), int((vertices[1].position.y + 1.0f)* image->GetHeight() / 2.0f + 0.5f), vertices[1].position.z);
            glm::vec3 c_pos(int((vertices[2].position.x + 1.0f)* image->GetWidth() / 2.0f + 0.5f), int((vertices[2].position.y + 1.0f)* image->GetHeight() / 2.0f + 0.5f), vertices[2].position.z);

            glm::vec3 normal = glm::cross((vertices[2].position - vertices[0].position), (vertices[1].position - vertices[0].position));
            normal = glm::normalize(normal);

            glvertex a(a_pos, vertices[0].uv, normal);
            glvertex b(b_pos, vertices[1].uv, normal);
            glvertex c(c_pos, vertices[2].uv, normal);

            float intensity = glm::dot(normal, lightDir);
            
            shade = TGAColor((byte)(intensity * 255), (byte)(intensity * 255), (byte)(intensity * 255), 255);

            if (intensity > 0)
                //triangle(a, b, c, zBuffer, shade, image);
                triangle(a, b, c, m_zBuffer, model, intensity, image);
        }
    }
}