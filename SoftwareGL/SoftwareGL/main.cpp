#include "common.hpp"
#include "tga.hpp"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);

int main(int argc, char * argv[])
{
    TGAImage img(100, 100, TGAImage::RGB);

    img.Set(52, 41, red);
    img.FlipVertically();
    img.WriteFile("output.tga");

    return 0;
}