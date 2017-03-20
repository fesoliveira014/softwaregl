#ifndef RASTER_H
#define RASTER_H

#include "common.hpp"
#include "tga.hpp"

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
}

#endif
