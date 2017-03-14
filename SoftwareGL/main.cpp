#include "common.hpp"
#include "tga.hpp"
#include "raster.hpp"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(0, 0, 255, 255);

int main(int argc, char * argv[])
{
    TGAImage image(100, 100, TGAImage::RGBA);

    //img.ReadFile("floor.tga");

    raster::line(10, 20, 70, 40, image, white);

    image.WriteFile("output.tga", false);

    return 0;
}