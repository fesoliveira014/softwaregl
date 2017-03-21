#ifndef RASTER_H
#define RASTER_H

#include "common.hpp"
#include "tga.hpp"
#include "model.hpp"

namespace raster
{
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

    static void render(Model& model, TGAImage *image, bool wireframe = false)
    {
        TGAColor white = TGAColor(255, 255, 255, 255);

        if (wireframe) {
            for (int i = 0; i < model.GetNumOfFaces(); ++i) {
                Model::face face = model.GetFace(i);
                for (int j = 0; j < 3; ++j) {
                    glm::vec3 v0 = model.GetVertex(face[j].vertex);
                    glm::vec3 v1 = model.GetVertex(face[(j + 1) % 3].vertex);

                    glm::ivec2 a((v0.x + 1.0f)* image->GetWidth() / 2.0f, (v0.y + 1.0f)* image->GetHeight() / 2.0f);
                    glm::ivec2 b((v1.x + 1.0f)* image->GetWidth() / 2.0f, (v1.y + 1.0f)* image->GetHeight() / 2.0f);

                    //std::cerr << "p1: " << glm::to_string(a) << ", p2: " << glm::to_string(b) << std::endl;

                    line(a, b, image, white);
                }
            }
        }
    }
}

#endif
