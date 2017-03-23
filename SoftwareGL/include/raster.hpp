#ifndef RASTER_H
#define RASTER_H

#include "common.hpp"
#include "tga.hpp"
#include "model.hpp"

namespace raster
{
    struct bbox2d
    {
        glm::ivec2 min, max;

        bbox2d() : min(glm::ivec2(0)), max(glm::ivec2(0)) {}
        bbox2d(glm::ivec2 a, glm::ivec2 b) : min(a), max(b) {}
    };

    static void line(const glm::ivec2 & aa, const glm::ivec2 & bb, TGAImage *image, TGAColor color)
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

    static glm::vec3 barycentric(const glm::ivec2 &a, const glm::ivec2 &b, const glm::ivec2 &c, glm::ivec2 &p)
    {
        glm::vec2 ab = b - a;
        glm::vec2 ca = c - a;
        glm::vec2 pa = a - p;

        glm::vec3 u = glm::cross(glm::vec3(ab.x, ca.x, pa.x), glm::vec3(ab.y, ca.y, pa.y));
        
        if (std::abs(u.z) < 1.f) 
            return glm::vec3(-1,1,1);
        
        return glm::vec3(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x/u.z);
    }

    static void triangle(const glm::ivec2 &t1, const glm::ivec2 &t2, const glm::ivec2 &t3, const TGAColor &color, TGAImage *image)
    {
        glm::ivec2 points[3] = {glm::ivec2(t1), glm::ivec2(t3), glm::ivec2(t3)};
        glm::ivec2 clamp(image->GetWidth() - 1, image->GetHeight() - 1);
        bbox2d bbox(clamp, glm::ivec2(0));

        for (int i = 0; i < 3; ++i) {
            bbox.min = glm::max(glm::ivec2(0), glm::min(bbox.min, points[i]));
            bbox.max = glm::min(clamp        , glm::max(bbox.min, points[i]));
        }

        glm::ivec2 p;

        for (p.x = bbox.min.x; p.x <= bbox.max.x; ++p.x) {
            for (p.y = bbox.min.y; p.y <= bbox.max.y; ++p.y) {
                glm::vec3 bcScreen = barycentric(t1, t2, t3, p);
                if (bcScreen.x >= 0 && bcScreen.y >= 0 && bcScreen.z >= 0) 
                    image->Set(p.x, p.y, color);
            }
        }
    }

    static void render(Model& model, TGAImage *image, bool wireframe = false)
    {
        TGAColor white = TGAColor(255, 255, 255, 255);

        
        for (uint i = 0; i < model.GetNumOfFaces(); ++i) {
            Model::face face = model.GetFace(i);

            if (wireframe) {        
                for (uint j = 0; j < 3; ++j) {
                    glm::vec3 v0 = model.GetVertex(face[j].vertex);
                    glm::vec3 v1 = model.GetVertex(face[(j + 1) % 3].vertex);

                    glm::ivec2 a((v0.x + 1.0f)* image->GetWidth() / 2.0f, (v0.y + 1.0f)* image->GetHeight() / 2.0f);
                    glm::ivec2 b((v1.x + 1.0f)* image->GetWidth() / 2.0f, (v1.y + 1.0f)* image->GetHeight() / 2.0f);

                    //std::cerr << "p1: " << glm::to_string(a) << ", p2: " << glm::to_string(b) << std::endl;

                    line(a, b, image, white);
                }
            }

            else {
                
            }
        }
    }
}

#endif
