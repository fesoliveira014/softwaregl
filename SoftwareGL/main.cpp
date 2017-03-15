#include "common.hpp"
#include "tga.hpp"
#include "raster.hpp"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(0, 0, 255, 255);

int main(int argc, char * argv[])
{
    TGAImage image(100, 100, TGAImage::RGB);

    //img.ReadFile("floor.tga");

    raster::line(13, 20, 80, 40, image, white); 
    raster::line(20, 13, 40, 80, image, red); 
    raster::line(80, 40, 13, 20, image, red);

    image.WriteFile("output.tga", false);

    return 0;
}